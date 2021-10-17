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

#include "ImageWiggle.h"
#include "BlotGL.h"
#include "Properties.h"
#include "BlotObject.h"
#include "EditGroup.h"
#include "ImageProc.h"
#include "ImageMove.h"
#include "ImageHide.h"
#include "Instruction.h"
#include "Library.h"
#include "StartScreen.h"
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

	_list = new QTreeWidget(p);
	_list->setGeometry(0, MENU_HEIGHT, INSTRUCTION_WIDTH, 
	                   p->height() - MENU_HEIGHT - QUICK_BUTTON_HEIGHT);
	_list->setSelectionMode(QAbstractItemView::ContiguousSelection);
	_list->show();

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
	_bUp->show();
	
	x += QUICK_BUTTON_HEIGHT;

	_bDown = new QPushButton(p);
	_bDown->setGeometry(x, p->height() - QUICK_BUTTON_HEIGHT, 
	                      QUICK_BUTTON_HEIGHT,
	                      QUICK_BUTTON_HEIGHT);
	_bDown->setIcon(down);
	_bDown->show();
	
	x += QUICK_BUTTON_HEIGHT;

	_bMore = new QPushButton(p);
	_bMore->setGeometry(x, p->height() - QUICK_BUTTON_HEIGHT, 
	                      QUICK_BUTTON_HEIGHT,
	                      QUICK_BUTTON_HEIGHT);
	_bMore->setText("...");
	_bMore->show();

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
	connect(_bDelete, &QPushButton::pressed, this, &BlotGL::deleteInstruction);
	connect(_bMore, &QPushButton::pressed, this, &BlotGL::changeInstruction);
	connect(_bView, &QPushButton::pressed, this, &BlotGL::toggleVKey);
	
	_buttons.push_back(_bDown);
	_buttons.push_back(_bUp);
	_buttons.push_back(_bDelete);
	_buttons.push_back(_bMore);
	_buttons.push_back(_bView);
}

void BlotGL::changeInstruction()
{
	if (_prop != NULL)
	{
		delete _prop;
		_prop = NULL;
	}
	
	QTreeWidgetItem *item = _list->currentItem();

	if (item == NULL)
	{
		return;
	}
	
	QRect me = geometry();

	_prop = new Properties(instructionForItem(item));
	_prop->setGeometry(me.right() + 50, 100, 
	                  PROPERTIES_DEFAULT_WIDTH, PROPERTIES_DEFAULT_HEIGHT);
	_prop->show();

}

void BlotGL::deleteInstruction()
{
	if (_list->currentItem() == NULL)
	{
		return;
	}

	QList<QTreeWidgetItem *> list = _list->selectedItems();
	
	int min = _list->topLevelItemCount();
	for (int i = 0; i < list.size(); i++)
	{
		int idx = _list->indexOfTopLevelItem(list[i]);
		_list->takeTopLevelItem(idx);
		if (idx < min)
		{
			min = idx;
		}
	}

	QTreeWidgetItem *current = _list->topLevelItem(min);
	Instruction *inst = instructionForItem(current);
	selectInstruction(inst, true);
	selectInEditMode();
	_list->setCurrentItem(current);
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

BlotGL::BlotGL(QWidget *p) : SlipGL(p)
{
	_timer = new QTimer();
	_timer->setInterval(5);
	connect(_timer, &QTimer::timeout, this, &BlotGL::progressAnimations);

	_prop = NULL;
	_parent = p;
	_list = NULL;
	_currPos = 0;
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

	advancePresentation();
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

void BlotGL::selectInstruction()
{
	if (_list->topLevelItemCount() == 0)
	{
		return;
	}

	QTreeWidgetItem *item = _list->currentItem();
	int row = _list->indexOfTopLevelItem(item);
	_currPos = row;

	Instruction *myInst = instructionForItem(item);
	StartScreen::startScreenPtr->setClick(myInst);
	std::cout << "Here" << std::endl;
	
	int lastWipe = 0;

	for (int i = row - 1; i >= 0; i--)
	{
		Instruction *inst = instructionForItem(_list->topLevelItem(i));

		if (inst->getClassName() == "WipeSlate")
		{
			lastWipe = i;
			break;
		}
	}

	clearAll();

	for (int i = lastWipe; i <= row; i++)
	{
		Instruction *inst = instructionForItem(_list->topLevelItem(i));
		
		inst->instantEffect();
	}

	if (_currInstruct != NULL)
	{
		_currInstruct->select(false);
	}
	
	if (_prop && _prop->isVisible())
	{
		changeInstruction();
	}
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
	int row = _list->indexOfTopLevelItem(_list->currentItem()) - 1;
	
	if (!instructionForItem(_list->topLevelItem(row + 1))->waitForClick())
	{
		while (true)
		{
			Instruction *inst = instructionForItem(_list->topLevelItem(row - 1));
			row--;
			if (inst->waitForClick())
			{
				row--;
				break;
			}
		}
	}
	
	if (row >= 0 && row < _list->topLevelItemCount())
	{
		_list->setCurrentItem(_list->topLevelItem(row));
		selectInstruction();
		_currPos = row + 1;
	}
}

void BlotGL::advancePresentation(bool clicked)
{
	if (_list == NULL)
	{
		return;
	}
	
	if (_animating.size() && !clicked)
	{
		return;
	}
	
	bool begun_sequence = false;

	while (true)
	{
		if ((int)_list->topLevelItemCount() <= _currPos)
		{
			break;
		}
		
		Instruction *inst = instructionForItem(_list->topLevelItem(_currPos));
		
		if (inst->waitForClick() && !clicked)
		{
			break;
		}
		
		std::cout << "Must do " << inst->instText() << std::endl;
		
		if (_editMode)
		{
			inst->instantEffect();
		}
		else
		{
			bool started = inst->animateEffect();
			
			begun_sequence |= started;

			if (started)
			{
				_animating.push_back(inst);
			}
		}

		update();
		clicked = false;
		_list->setCurrentItem(_list->topLevelItem(_currPos));
		_currPos++;
	}
	
	if (begun_sequence)
	{
		_timer->start();
	}
}

void BlotGL::changeClick(bool click)
{
	Instruction *inst = instructionForItem(_list->currentItem());
	inst->setOnClick(click);
	
	StartScreen::startScreenPtr->setClick(inst);
}

void BlotGL::setFullScreen()
{

	QList<QScreen *> screens = qApp->screens();
	QSize resol = screens.last()->size();
	
	std::cout << "Making full screen" << std::endl;
	hide();
	_parent = parent();
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
		w = static_cast<StartScreen *>(parent());
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

void BlotGL::addInstruction(Instruction *inst, bool atRow)
{
	if (atRow)
	{
		int idx = _list->indexOfTopLevelItem(_list->currentItem()) + 1;
		_list->insertTopLevelItem(idx, inst);
		_list->clearSelection();
		_list->setCurrentItem(_list->topLevelItem(idx));
	}
	else
	{
		_list->addTopLevelItem(inst);
	}
	
	bool atTop = false;

	if (inst->primaryLoad())
	{
		atTop = true;
	}

	inst->updateText();
	addObject(inst->object(), atTop);
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
		advancePresentation(true);
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
	selectInstruction(inst, true);
}

Instruction *BlotGL::findSelectedInstruction(double x, double y)
{
	Instruction *old = _currInstruct;
	
	/* Prioritise instruction selected in menu */
	Instruction *option = instructionForItem(_list->currentItem());
	if (isPossibleInstruction(option, old, x, y))
	{
		return option;
	}

	/* Else, go through everything in reverse order */
	for (int i = _list->indexOfTopLevelItem(_list->currentItem()); i >= 0; i--)
	{
		Instruction *option = instructionForItem(_list->topLevelItem(i));

		if (isPossibleInstruction(option, old, x, y))
		{
			return option;
		}
	}

	int start = _list->indexOfTopLevelItem(_list->currentItem());
	for (int i = start; i < _list->topLevelItemCount(); i++)
	{
		Instruction *option = instructionForItem(_list->topLevelItem(i));

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
		advancePresentation(true);
		return;
	}
	
	/* selection */
	if (e->x() == _startX && e->y() == _startY)
	{
		double x = (e->x() / (double)width()  * 2) - 1;
		double y = (e->y() / (double)height() * 2) - 1;

		/* If we need to select a primary instruction */
		if (!_shiftPressed || _currInstruct == NULL)
		{
			replaceSelectedInstruction(x, y);
		}
		/* If we are selecting additional instructions */
		else if (_shiftPressed)
		{
			Instruction *inst = findSelectedInstruction(x, y);
			selectInstruction(inst, false);
		}
	}
}

void BlotGL::clearAll()
{
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
			addChild("blot_object", bo);
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
		addChild("instruction", inst);
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
