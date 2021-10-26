// Blot
// Copyright (C) 2017-2018 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Please email: vagabond @ hginn.co.uk for more details.

#include "Set.h"
#include "ImageWiggle.h"
#include "BlotGL.h"
#include "Properties.h"
#include "BlotObject.h"
#include "Delay.h"
#include "EditGroup.h"
#include "ImageProc.h"
#include "ImageMove.h"
#include "ImageHide.h"
#include "Instruction.h"
#include "Library.h"
#include "StartScreen.h"
#include "Tree.h"
#include <QInputDialog>
#include <QMenu>
#include <QApplication>
#include <algorithm>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QIcon>
#include <QPushButton>
#include <QStyle>
#include <QScreen>
#include <QWindow>
#include <QTimer>
#include <iostream>

void BlotGL::makeList(QWidget *p)
{
	if (_list)
	{
		return;
	}

	_list = new Tree(p);
	_list->setGeometry(0, MENU_HEIGHT, INSTRUCTION_WIDTH, 
	                   p->height() - MENU_HEIGHT - QUICK_BUTTON_HEIGHT);
	_list->show();
	
	connect(_list, &QTreeWidget::customContextMenuRequested,
	        this, &BlotGL::rightClickMenu);

	QIcon del = qApp->style()->standardIcon(QStyle::SP_TrashIcon);
	QIcon up = qApp->style()->standardIcon(QStyle::SP_ArrowUp);
	QIcon down = qApp->style()->standardIcon(QStyle::SP_ArrowDown);
	QIcon more = qApp->style()->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton);
	
	int x = 0;

	_bUp = new QPushButton(p);
	_bUp->setGeometry(x, p->height() - QUICK_BUTTON_HEIGHT, 
	                      QUICK_BUTTON_HEIGHT,
	                      QUICK_BUTTON_HEIGHT);
	_bUp->setIcon(up);
//	_bUp->show();
	
	x += QUICK_BUTTON_HEIGHT;

	_bDown = new QPushButton(p);
	_bDown->setGeometry(x, p->height() - QUICK_BUTTON_HEIGHT, 
	                      QUICK_BUTTON_HEIGHT,
	                      QUICK_BUTTON_HEIGHT);
	_bDown->setIcon(down);
//	_bDown->show();
	
	x += QUICK_BUTTON_HEIGHT;

	_bMore = new QPushButton(p);
	_bMore->setGeometry(x, p->height() - QUICK_BUTTON_HEIGHT, 
	                      QUICK_BUTTON_HEIGHT,
	                      QUICK_BUTTON_HEIGHT);
	_bMore->setText("...");
//	_bMore->show();

	x += QUICK_BUTTON_HEIGHT;

	_bView = new QPushButton(p);
	_bView->setGeometry(x, p->height() - QUICK_BUTTON_HEIGHT, 
	                      QUICK_BUTTON_HEIGHT,
	                      QUICK_BUTTON_HEIGHT);
	_bView->setIcon(more);
	_bView->show();

	x += QUICK_BUTTON_HEIGHT;

	_bDelete = new QPushButton(p);
	_bDelete->setGeometry(x, p->height() - QUICK_BUTTON_HEIGHT, 
	                      QUICK_BUTTON_HEIGHT,
	                      QUICK_BUTTON_HEIGHT);
	_bDelete->setIcon(del);
	_bDelete->show();

	connect(_bUp, &QPushButton::pressed, this, &BlotGL::moveInstructionUp);
	connect(_bDown, &QPushButton::pressed, this, &BlotGL::moveInstructionDown);
	connect(_bDelete, &QPushButton::pressed, this, &BlotGL::deleteInstructions);
	connect(_bView, &QPushButton::pressed, this, &BlotGL::toggleVKey);
	
	_buttons.push_back(_bDown);
	_buttons.push_back(_bUp);
	_buttons.push_back(_bDelete);
	_buttons.push_back(_bMore);
	_buttons.push_back(_bView);
}

void BlotGL::rightClickMenu(const QPoint &p)
{
	std::cout << "here" << std::endl;
	QMenu *m = new QMenu();
	QPoint pos = _list->mapToGlobal(p);
	
	QAction *a = m->addAction("Set delay");
	connect(a, &QAction::triggered, this, &BlotGL::setDelay);

	m->exec(pos);
}

void BlotGL::setDelay()
{
	bool ok;
	double d;
	d = QInputDialog::getDouble(this, tr("Delay after previous instruction"),
	                            tr("Set value:"), 0, 0, 100, 1, &ok);

	if (ok)
	{
		QList<QTreeWidgetItem *> selected = _list->selectedItems();
		for (size_t i = 0; i < selected.size(); i++)
		{
			Instruction *inst = dynamic_cast<Instruction *>(selected[i]);
			if (inst)
			{
				inst->setDelay(d);
			}
		}
	}
}

Instruction *BlotGL::takeInstruction(Instruction *inst)
{
	int idx = indexOfInstruction(inst);
	if (idx >= 0)
	{
		return dynamic_cast<Instruction *>(_list->takeTopLevelItem(idx));
	}

	return NULL;
}

void BlotGL::deleteInstructions()
{
	if (_list->currentItem() == NULL)
	{
		return;
	}

	QList<QTreeWidgetItem *> list = _list->selectedItems();
	QList<Instruction *> conv;
	for (size_t i = 0; i < list.size(); i++)
	{
		conv.push_back(dynamic_cast<Instruction *>(list[i]));
	}
	
	Instruction *next = NULL;
	for (int i = 0; i < conv.size(); i++)
	{
		Set *p = conv[i]->instructionParent();
		Instruction *candidate = p->deleteInstruction(conv[i]);

		if (candidate == _presentation)
		{
			continue;
		}
		
		bool found = (std::find(conv.begin(), conv.end(), candidate) 
		              != conv.end());

		if (!found)
		{
			next = candidate;
		}
	}
	
	if (next != NULL)
	{
		selectInstruction(next, true);
		selectInEditMode();
		_list->setCurrentItem(next);
	}
}

void BlotGL::moveInstructionUp()
{
	moveInstruction(-1);
}

void BlotGL::moveInstructionDown()
{
	moveInstruction(1);
}


void BlotGL::moveInstToWipe()
{
	if (_list->currentItem() == NULL)
	{
		return;
	}

	int row = _list->indexOfTopLevelItem(_list->currentItem());
	int wipeRow = row;
	
	for (int i = row + 1; i < _list->topLevelItemCount(); i++)
	{
		Instruction *inst = instructionForItem(_list->topLevelItem(i));
		if (inst->getClassName() == "WipeSlate")
		{
			wipeRow = i;
			break;
		}
	}
	
	if (wipeRow > row)
	{
		moveInstToIndex(wipeRow - 1);
		/*
		QTreeWidgetItem *item = _list->takeTopLevelItem(row);
		_list->insertTopLevelItem(wipeRow - 1, item);
		*/
	}
}

void BlotGL::moveInstToBottom()
{
	moveInstToIndex(_list->topLevelItemCount());
}

std::vector<unsigned int> BlotGL::getSortedIndices()
{
	QList<QTreeWidgetItem *> list = _list->selectedItems();
	std::vector<unsigned int> indices;
	
	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem *item = list[i];
		unsigned int row = _list->indexOfTopLevelItem(item);
		indices.push_back(row);
	}
	
	std::sort(indices.begin(), indices.end(), std::less<unsigned int>());
	return indices;
}

void BlotGL::moveInstToIndex(int index)
{
	std::vector<unsigned int> indices = getSortedIndices();
	int diff = index - indices.back();
	
	std::cout << "moving " << diff << std::endl;
	
	moveInstruction(diff);
}

void BlotGL::moveInstruction(int diff)
{
	if (_list->currentItem() == NULL)
	{
		return;
	}
	
	int row = _list->indexOfTopLevelItem(_list->currentItem());

	std::vector<unsigned int> indices = getSortedIndices();
	if (indices.size() <= 0)
	{
		return;
	}

	int min = indices[0];
	int max = indices.back();
	
	int sign = (diff > 0) ? 1 : -1;

	if (abs(diff) <= 1)
	{
		row += diff;
	}

	diff = abs(diff);
	
	for (int i = 0; i < diff; i++)
	{
		int from, to;
		if (sign > 0)
		{
			from = max + 1;
			to = min;
		}
		else
		{
			from = min - 1;
			to = max;
		}
		
		if (from < 0 || to < 0 || from > _list->topLevelItemCount() 
		    || to > _list->topLevelItemCount())
		{
			break;
		}

		QTreeWidgetItem *item = _list->takeTopLevelItem(from);
		_list->insertTopLevelItem(to, item);
		
		min += sign;
		max += sign;
	}
	
	_list->clearSelection();

	_list->setCurrentItem(_list->topLevelItem(row));
}

void BlotGL::updateProjection(double side)
{
	_proj = make_mat4x4();
	_aspect.vals[0] = 1 / _aspectRatio;
}

void BlotGL::setAspectRatio(double ratio)
{
	_aspectRatio = ratio;
}

BlotGL::BlotGL(QWidget *p) : SlipGL(p), Set(this)
{
	_timer = new QTimer();
	_timer->setInterval(5);
	connect(_timer, &QTimer::timeout, this, &BlotGL::progressAnimations);

	_parent = p;
	_list = NULL;
	_editMode = true;
	_fullScreen = false;
	_altPressed = false;
	_currInstruct = NULL;
	_aspectRatio = 1;
	_aspect = make_mat4x4();
	
	if (p == NULL)
	{
		p = StartScreen::startScreenPtr;
	}

	makeList(p);
	
	connect(_list, &QTreeWidget::currentItemChanged, 
	        this, &BlotGL::selectInEditMode);

	_r = 1;
	_g = 1;
	_b = 1;
}

void BlotGL::selectInEditMode()
{
	if (_editMode)
	{
		selectInstruction();
	}
}

void BlotGL::insertInstruction(Instruction *inst, int idx)
{
	_list->insertTopLevelItem(idx, inst);
}

int BlotGL::instructionCount()
{
	return _list->topLevelItemCount();
}

int BlotGL::indexOfInstruction(Instruction *inst)
{
	int row = _list->indexOfTopLevelItem(inst);
	return row;
}

Instruction *BlotGL::instruction(int i)
{
	QTreeWidgetItem *item = _list->topLevelItem(i);
	Instruction *inst = dynamic_cast<Instruction *>(item);
	return inst;
}

Instruction *BlotGL::currentInstruction()
{
	QTreeWidgetItem *item = _list->currentItem();
	Instruction *inst = dynamic_cast<Instruction *>(item);
	return inst;
}

Set *BlotGL::instructionParent()
{
	return NULL;
}

void BlotGL::selectInstruction()
{
	if (instructionCount() == 0)
	{
		return;
	}

	clearAll();
	Instruction *curr = currentInstruction();
	Set *parent = curr->instructionParent();
	parent->displayToInstruction(curr);
}

void BlotGL::selectInstruction(Instruction *inst)
{
	_list->setCurrentItem(inst);
	selectInstruction();
}

void BlotGL::initializeGL()
{
	SlipGL::initializeGL();
	glDisable(GL_DEPTH_TEST);
}

void BlotGL::addObject(SlipObject *obj, bool top)
{
	if (obj == NULL)
	{
		return;
	}

	std::cout << "Single init" << std::endl;
	
	std::vector<SlipObject *>::iterator it;
	it = std::find(_objects.begin(), _objects.end(), obj);
	
	if (it == _objects.end())
	{
		if (!top)
		{
			_objects.push_back(obj);
		}
		else
		{
			_objects.insert(_objects.begin(), obj);
		}

		obj->initializeOpenGLFunctions();
		obj->initialisePrograms();
	}
}

void BlotGL::goBackOneSlide()
{
	Instruction *last = currentInstruction()->prevInstruction();
	
	if (last == NULL)
	{
		return;
	}
	
	Instruction *target = last;

	while (last && !last->waitForClick())
	{
		last = last->prevInstruction();

		if (last == NULL)
		{
			break;
		}
		
		target = last;
	}
	
	if (target->prevInstruction() != NULL)
	{
		target = target->prevInstruction();
	}

	_list->setCurrentItem(target);
	selectInstruction();
}

bool BlotGL::addToAnimated(Instruction *next)
{
	bool started = next->animateEffect();

	if (started)
	{
		_animating.push_back(next);
	}
	
	return started;
}

void BlotGL::triggerSimultaneousInstructions(Instruction *next, bool pass)
{
	bool begun_sequence = false;

	while (next != NULL)
	{
		next = next->nextInstruction();

		if (next->waitForClick() && !pass)
		{
			break;
		}
		
		std::cout << "Must do " << next->instText() << std::endl;

		if (!next->hasDelay())
		{
			bool started = addToAnimated(next);
			begun_sequence |= started;
		}
		
		update();
		_list->setCurrentItem(next);

		if (next->hasDelay())
		{
			Delay *delay = new Delay(this, next);
			addToAnimated(delay);
			begun_sequence = true;

			break;
		}

		pass = false;
	}
	
	if (begun_sequence)
	{
		_timer->start();
	}
}

void BlotGL::advancePresentation()
{
	if (_list == NULL)
	{
		return;
	}
	
	Instruction *next = currentInstruction();
	if (next == NULL && instructionCount() > 0)
	{
		_list->setCurrentItem(instruction(0));
		next = currentInstruction();
		selectInstruction();
	}

	if (next)
	{
		Instruction *old = next;
		next = next->displayableInstruction();
		_list->setCurrentItem(next);
		
		if (old != next)
		{
			selectInstruction();
		}
		
		triggerSimultaneousInstructions(next, true);
	}
}

void BlotGL::changeClick(bool click)
{
	Instruction *inst = currentInstruction();
	inst->setOnClick(click);
	
	StartScreen::startScreenPtr->setClick(inst);
}

void BlotGL::setFullScreen()
{

	QList<QScreen *> screens = qApp->screens();
	QSize resol = screens.last()->size();
	
	std::cout << "Making full screen" << std::endl;
	hide();
	_parent = SlipGL::parent();
	QWidget::setParent(NULL);
	resize(resol.width(), resol.height());
	setAspectRatio(_aspectRatio);
	setWindowState(Qt::WindowFullScreen);

	_fullScreen = true;
	_currPos++;

	std::cout << "Number of screens (will set last): " 
	<< screens.size() << std::endl;
	
	for (int i = 0; i < screens.size(); i++)
	{
		QScreen *scr = screens[i];
		std::cout << scr->geometry().x() << " " <<
		scr->geometry().y() << " " << scr->geometry().width() << " "
		<< scr->geometry().height() << std::endl;
	}
	
	windowHandle()->setScreen(screens.last());
	setGeometry(screens.last()->geometry());

	windowHandle()->showFullScreen();
	glDisable(GL_DEPTH_TEST);
	initialisePrograms();
}

void BlotGL::setSmallWindow()
{
	std::cout << "Making small window" << std::endl;

	StartScreen *w = static_cast<StartScreen *>(_parent);
	_fullScreen = false;

	if (w == NULL)
	{
		w = static_cast<StartScreen *>(SlipGL::parent());
	}
	
	if (w == NULL)
	{
		return;
	}
	
	if (windowHandle())
	{
		windowHandle()->showNormal();
	}
	
	QWidget::setParent(w);
	if (!isVisible())
	{
		show();
	}
	makeList(w);
	setEditMode(true);
	w->resizeWidgets();
}

void BlotGL::addImage(ImageProc *proc)
{
	BlotObject *obj = new BlotObject(proc);
	addObject(obj);
}

void BlotGL::keyReleaseEvent(QKeyEvent *event)
{
	_shiftPressed = false;
	_altPressed = false;
	_controlPressed = false;
}

void BlotGL::toggleVKey()
{
	QKeyEvent event(QEvent::KeyPress, (int)Qt::Key_V, Qt::NoModifier);
	keyPressEvent(&event);
}

void BlotGL::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Shift)
	{
		_shiftPressed = true;
	}
	if (event->key() == Qt::Key_Alt)
	{
		_altPressed = true;
	}
	if (event->key() == Qt::Key_Control)
	{
		_controlPressed = true;
	}
	if (event->key() == Qt::Key_V)
	{
		if (windowFlags() & Qt::Window)
		{
			setSmallWindow();
		}
		else
		{
			setEditMode(false);
			setFullScreen();
		}

		setFocus();
		setFocusPolicy(Qt::StrongFocus);
	}
	else if (event->key() == Qt::Key_Right || 
	         event->key() == Qt::Key_Down ||
	         event->key() == Qt::Key_PageDown ||
	         event->key() == Qt::Key_Enter ||
	         event->key() == Qt::Key_Space ||
	         event->key() == Qt::Key_N)
	{
		advancePresentation();
	}
	else if (event->key() == Qt::Key_Left ||
	         event->key() == Qt::Key_Up ||
	         event->key() == Qt::Key_PageUp ||
	         event->key() == Qt::Key_Backspace ||
	         event->key() == Qt::Key_P)
	{
		goBackOneSlide();
	}
}

void BlotGL::mousePressEvent(QMouseEvent *e)
{
	_startX = e->x();
	_startY = e->y();
	_lastX = _startX;
	_lastY = _startY;

}

Instruction *BlotGL::instructionForItem(QTreeWidgetItem *item)
{
	if (item == NULL)
	{
		return NULL;
	}
	
	Instruction *inst = dynamic_cast<Instruction *>(item);
	return inst;
}

bool isPossibleInstruction(Instruction *option, Instruction *old,
                           double x, double y)
{
	if (option == NULL)
	{
		return false;
	}

	if (!option->canMove())
	{
		return false;
	}

	if (option == old)
	{
		return false;
	}

	bool cover = option->isCovered(-y, x);

	return cover;
}

void BlotGL::selectInstruction(Instruction *inst, bool primary)
{
	if (inst == NULL || inst == _currInstruct)
	{
		return;
	}

	if (primary || _currInstruct == NULL)
	{
		if (_currInstruct != NULL)
		{
			_currInstruct->select(false);
		}

		_currInstruct = inst;
		selectInstruction();
	}
	else
	{
		if (std::find(_otherInstruct.begin(), _otherInstruct.end(),
		              inst) != _otherInstruct.end())
		{
			return;
		}
		
		_otherInstruct.push_back(inst);
	}

	inst->select(true);
}

void BlotGL::deselectAll()
{
	for (size_t i = 0; i < _otherInstruct.size(); i++)
	{
		_otherInstruct[i]->select(false);
	}
	
	_otherInstruct.clear();

	if (_currInstruct != NULL)
	{
		_currInstruct->select(false);
	}

	_currInstruct = NULL;
}

void BlotGL::replaceSelectedInstruction(double x, double y)
{
	deselectAll();
	Instruction *inst = findSelectedInstruction(x, y);
	
	if (inst != NULL)
	{
		selectInstruction(inst, true);
		inst->select(true);
	}
}

Instruction *BlotGL::findSelectedInstruction(double x, double y)
{
	Instruction *old = _currInstruct;
	
	/* Prioritise instruction selected in menu */
	Instruction *option = currentInstruction();

	if (isPossibleInstruction(option, old, x, y))
	{
		return option;
	}

	/* Else, go through everything in reverse order */
//	for (int i = _list->indexOfTopLevelItem(_list->currentItem()); i >= 0; i--)

	while (option != NULL)
	{
		option = option->prevInstruction();
		
		if (option == NULL)
		{
			return NULL;
		}
		
		if (option->getClassName() == "WipeSlate")
		{
			return NULL;
		}

		if (isPossibleInstruction(option, old, x, y))
		{
			return option;
		}
	}
	
	return NULL;
}

EditGroup BlotGL::editGroup()
{
	EditGroup group(_currInstruct, _otherInstruct);

	return group;
}

void BlotGL::mouseMoveEvent(QMouseEvent *e)
{
	if (_currInstruct == NULL)
	{
		return;
	}

	int newX = e->x();
	int newY = e->y();

	double frac_x = ((newX - _lastX) / (double)width() * 2);
	double frac_y = ((newY - _lastY) / (double)height() * 2);
	
	_lastX = newX;
	_lastY = newY;
	
	if (_altPressed)
	{
		double from_x = _lastX * 2 / (double)width() - 1;
		double from_y = _lastY * 2 / (double)width() - 1;
		EditGroup group = editGroup();
		group.rotateFractional(from_x, -from_y, frac_x, -frac_y);
	}
	else if (!_shiftPressed)
	{
//		_currInstruct->moveFractional(-frac_y, frac_x);
		EditGroup group = editGroup();
		group.moveFractional(-frac_y, frac_x);
	}
	else if (_shiftPressed)
	{
		_currInstruct->resizeFractional(-frac_y, frac_x, _controlPressed);
	}

	update();
}

void BlotGL::mouseReleaseEvent(QMouseEvent *e)
{
	if (!_editMode)
	{
		advancePresentation();
		return;
	}
	
	/* selection */
	if (e->x() == _startX && e->y() == _startY)
	{
		double x = (e->x() / (double)width()  * 2) - 1;
		double y = (e->y() / (double)height() * 2) - 1;
		
		std::cout << x << " "  << y << std::endl;

		/* If we need to select a primary instruction */
		/* If we are selecting additional instructions */
		if (_shiftPressed)
		{
			Instruction *inst = findSelectedInstruction(x, y);
			selectInstruction(inst, false);
		}
		else
		{
			replaceSelectedInstruction(x, y);
		}
	}
}

void BlotGL::clearAll()
{
	std::cout << "Clearing screen" << std::endl;
	for (size_t i = 0; i < _objects.size(); i++)
	{
		BlotObject *bo = dynamic_cast<BlotObject *>(_objects[i]);
		if (bo && bo->shouldWipe())
		{
			bo->setDisabled(true);
		}
	}

}

void BlotGL::addProperties()
{
	addDoubleProperty("aspect_ratio", &_aspectRatio);
	
	for (size_t i = 0; i < _objects.size(); i++)
	{
		BlotObject *bo = dynamic_cast<BlotObject *>(_objects[i]);
		
		if (bo)
		{
			Parser::addChild("blot_object", bo);
		}
	}

	if (_list == NULL)
	{
		return;
	}

	for (int i = 0; i < _list->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = _list->topLevelItem(i);
		Instruction *inst = instructionForItem(item);
		Parser::addChild("instruction", inst);
	}
}

void BlotGL::addObject(Parser *child, std::string name)
{
	if (name == "blot_object")
	{
		_objects.push_back(static_cast<BlotObject *>(child));
	}

	if (name == "instruction")
	{
		Instruction *inst = static_cast<Instruction *>(child);
		_list->addTopLevelItem(inst);
	}
}

void BlotGL::postParseTidy()
{
	for (int i = 0; i < _list->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = _list->topLevelItem(i);
		Instruction *inst = instructionForItem(item);
		inst->updateText();
	}

	setAspectRatio(_aspectRatio);
	advancePresentation();
}

bool BlotGL::imageInUse(ImageProc *image)
{
	for (int i = 0; i < _list->topLevelItemCount(); i++)
	{
		Instruction *inst = instructionForItem(_list->topLevelItem(i));
		BlotObject *obj = inst->object();

		if (obj && obj->getImage() && obj->getImage() == image)
		{
			return true;
		}
	}
	
	return false;
}

void BlotGL::removeImageReferences(ImageProc *image)
{
	/* remove instructions first */
	for (int i = 0; i < _list->topLevelItemCount(); i++)
	{
		Instruction *inst = instructionForItem(_list->topLevelItem(i));
		
		if (inst->object() && inst->object()->getImage() == image)
		{
			_list->takeTopLevelItem(i);
			i--;
		}
	}

	/* remove blot objects */

	for (size_t i = 0; i < _objects.size(); i++)
	{
		BlotObject *bo = dynamic_cast<BlotObject *>(_objects[i]);
		if (bo && bo->getImage() == image)
		{
			_objects.erase(_objects.begin() + i);
			i--;
		}
	}
	
	selectInEditMode();
}

void BlotGL::progressAnimations()
{
	for (size_t i = 0; i < _animating.size(); i++)
	{
		bool keep = _animating[i]->animateStep();
		
		if (!keep)
		{
			if (dynamic_cast<Delay *>(_animating[i]) != NULL)
			{
				delete _animating[i];
			}

			_animating.erase(_animating.begin() + i);
			i--;
		}
	}

	if (_animating.size() == 0)
	{
		_timer->stop();
	}
	
	update();
}

void BlotGL::addSet()
{
	Instruction *inst = currentInstruction();
	
	if (!inst) return;

	inst->instructionParent()->Set::addSet();
}

void BlotGL::addInstruction(Instruction *inst, bool atRow)
{
	Instruction *curr = currentInstruction();
	curr->instructionParent()->Set::addInstruction(inst, atRow);
}

void BlotGL::addMoveCurrentImage()
{
	Instruction *curr = instructionForItem(_list->currentItem());

	if (!curr || curr->object() == NULL)
	{
		return;
	}

	ImageMove *move = new ImageMove(this, curr);
	addInstruction(move);
}

void BlotGL::addWiggleCurrentImage()
{
	Instruction *curr = instructionForItem(_list->currentItem());

	if (!curr || curr->object() == NULL)
	{
		return;
	}

	ImageWiggle *wiggle = new ImageWiggle(this, curr);
	addInstruction(wiggle);
}

void BlotGL::addHideCurrentImage()
{
	Instruction *curr = instructionForItem(_list->currentItem());
	
	if (!curr || curr->object() == NULL)
	{
		return;
	}
	
	ImageHide *hide = new ImageHide(this, curr);
	
	if (hide->isValid())
	{
		addInstruction(hide);
	}
}

void BlotGL::selectAll()
{
	deselectAll();

	for (int i = 0; i < _list->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = _list->topLevelItem(i);
		Instruction *inst = instructionForItem(item);

		if (inst->object() == NULL)
		{
			continue;
		}
		
		if (inst->object()->isDisabled())
		{
			continue;
		}
		
		selectInstruction(inst, false);
	}
}

void BlotGL::copyToGL(BlotGL *another)
{
	for (int i = 0; i < _list->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = _list->topLevelItem(i);
		Instruction *inst = instructionForItem(item);
		
		if (inst->primaryLoad())
		{
			continue;
		}
		
		inst->setPresentation(another);
		inst->reloadParser();
		another->addInstruction(inst, false);
	}
}

