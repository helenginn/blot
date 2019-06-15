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

#include "BlotGL.h"
#include "Properties.h"
#include "BlotObject.h"
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

void BlotGL::initializeGL()
{
	initializeOpenGLFunctions();

	double val = 1.0;//rand() / (double)RAND_MAX;
	glClearColor(val, 1.0, 1.0, 1.0);

//	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	initialisePrograms();
}

void BlotGL::makeList(QWidget *p)
{
	if (_list)
	{
		return;
	}

	_list = new QListWidget(p);
	_list->setGeometry(0, MENU_HEIGHT, INSTRUCTION_WIDTH, 
	                   p->height() - MENU_HEIGHT - QUICK_BUTTON_HEIGHT);
	_list->show();

	QIcon del = qApp->style()->standardIcon(QStyle::SP_TrashIcon);
	QIcon up = qApp->style()->standardIcon(QStyle::SP_ArrowUp);
	QIcon down = qApp->style()->standardIcon(QStyle::SP_ArrowDown);
//	QIcon more = qApp->style()->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton);
	
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
//	_bMore->setIcon(more);
	_bMore->setText("...");
	_bMore->show();

	x += QUICK_BUTTON_HEIGHT;

	_bPlus = new QPushButton(p);
	_bPlus->setGeometry(x, p->height() - QUICK_BUTTON_HEIGHT, 
	                      QUICK_BUTTON_HEIGHT,
	                      QUICK_BUTTON_HEIGHT);
	_bPlus->setText("+");
	_bPlus->show();

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
	
	_buttons.push_back(_bDown);
	_buttons.push_back(_bUp);
	_buttons.push_back(_bDelete);
	_buttons.push_back(_bMore);
	_buttons.push_back(_bPlus);
}

void BlotGL::changeInstruction()
{
	if (_prop != NULL)
	{
		delete _prop;
		_prop = NULL;
	}
	
	QListWidgetItem *item = _list->currentItem();

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

	_list->takeItem(_list->currentRow());
	/* clears image if needed */
	selectInEditMode();
}

void BlotGL::moveInstructionUp()
{
	moveInstruction(-1);
}

void BlotGL::moveInstructionDown()
{
	moveInstruction(1);
}

void BlotGL::moveInstToBottom()
{
	if (_list->currentItem() == NULL)
	{
		return;
	}

	int row = _list->currentRow();
	QListWidgetItem *item = _list->takeItem(row);
	_list->addItem(item);
}

void BlotGL::moveInstruction(int diff)
{
	if (_list->currentItem() == NULL)
	{
		return;
	}

	int row = _list->currentRow();
	
	if (row + diff < 0 || row + diff >= _list->count())
	{
		return;
	}

	QListWidgetItem *item = _list->takeItem(row);
	row += diff;
	_list->insertItem(row, item);
	_list->setCurrentRow(row);
}

void BlotGL::setAspectRatio(double ratio)
{
	_aspectRatio = ratio;
	_aspect = make_mat3x3();
	_aspect.vals[0] = 1 / (ratio);
}

BlotGL::BlotGL(QWidget *p) : QOpenGLWidget(p)
{
	_timer = new QTimer();
	_timer->setInterval(20);
	connect(_timer, &QTimer::timeout, this, &BlotGL::progressAnimations);

	_prop = NULL;
	_parent = p;
	_list = NULL;
	_currPos = 0;
	_editMode = true;
	_controlPressed = false;
	_shiftPressed = false;
	_altPressed = false;
	_currInstruct = NULL;
	_aspectRatio = 1;
	_aspect = make_mat3x3();
	
	if (p == NULL)
	{
		p = StartScreen::startScreenPtr;
	}

	makeList(p);
	
	connect(_list, &QListWidget::currentRowChanged, 
	        this, &BlotGL::selectInEditMode);

	advancePresentation();
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
	if (_list->count() == 0)
	{
		return;
	}

	QListWidgetItem *item = _list->currentItem();
	int row = _list->row(item);
	_currPos = row;

	Instruction *myInst = instructionForItem(item);
	StartScreen::startScreenPtr->setClick(myInst);
	
	int lastWipe = 0;

	for (int i = row - 1; i >= 0; i--)
	{
		Instruction *inst = instructionForItem(_list->item(row));

		if (inst->getClassName() == "WipeSlate")
		{
			lastWipe = i;
			break;
		}
	}

	clearAll();

	for (int i = lastWipe; i <= row; i++)
	{
		Instruction *inst = instructionForItem(_list->item(i));
		
		inst->makeEffect();
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

void BlotGL::addObject(BlotObject *obj)
{
	if (obj == NULL)
	{
		return;
	}

	std::cout << "Single init" << std::endl;
	obj->initialisePrograms();
	
	std::vector<BlotObject *>::iterator it;
	it = std::find(_objects.begin(), _objects.end(), obj);
	
	if (it == _objects.end())
	{
		_objects.push_back(obj);
	}
}

void BlotGL::resizeGL(int w, int h)
{

}

void BlotGL::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (size_t i = 0; i < _objects.size(); i++)
	{
		_objects[i]->render(this);
	}
}

void BlotGL::initialisePrograms()
{
	std::cout << "Every init" << std::endl;
	for (size_t i = 0; i < _objects.size(); i++)
	{
		_objects[i]->initialisePrograms();
	}
}

void BlotGL::goBackOneSlide()
{
	int row = _list->currentRow() - 1;
	
	if (!instructionForItem(_list->item(row + 1))->waitForClick())
	{
		while (true)
		{
			Instruction *inst = instructionForItem(_list->item(row - 1));
			row--;
			if (inst->waitForClick())
			{
				row--;
				break;
			}
		}
	}
	
	if (row >= 0 && row < _list->count())
	{
		_list->setCurrentRow(row);
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
	
	if (_animating.size())
	{
		return;
	}
	
	bool begun_sequence = false;

	while (true)
	{
		if ((int)_list->count() <= _currPos)
		{
			break;
		}
		
		Instruction *inst = instructionForItem(_list->item(_currPos));
		
		if (inst->waitForClick() && !clicked)
		{
			break;
		}
		
		std::cout << "Must do " << inst->instText() << std::endl;
		
		if (_editMode)
		{
			inst->makeEffect();
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
		_list->setCurrentRow(_currPos);
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
	show();
	_currPos++;
	windowHandle()->setScreen(screens.last());
	showFullScreen();
}

void BlotGL::setSmallWindow()
{
	std::cout << "Making small window" << std::endl;
	hide();
	StartScreen *w = static_cast<StartScreen *>(_parent);

	if (w == NULL)
	{
		w = static_cast<StartScreen *>(parent());
	}
	
	if (w == NULL)
	{
		return;
	}
	
	QWidget::setParent(w);
	makeList(w);
	setEditMode(true);
	show();
	w->resizeWidgets();
}

void BlotGL::addImage(ImageProc *proc)
{
	BlotObject *obj = new BlotObject(proc);
	addObject(obj);
}

void BlotGL::addInstruction(Instruction *inst, bool atRow)
{
	QListWidgetItem *item = new QListWidgetItem();
	inst->setListItem(item);
	QVariant var = QVariant::fromValue<Instruction *>(inst);
	item->setData(Qt::UserRole, var);

	if (atRow)
	{
		_list->insertItem(_list->currentRow() + 1, item);
		_list->setCurrentRow(_list->currentRow() + 1);
	}
	else
	{
		_list->addItem(item);
	}

	inst->updateText();
	addObject(inst->object());
}

void BlotGL::keyReleaseEvent(QKeyEvent *event)
{
	_shiftPressed = false;
	_altPressed = false;
	_controlPressed = false;
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
	         event->key() == Qt::Key_PageDown ||
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

Instruction *BlotGL::instructionForItem(QListWidgetItem *item)
{
	if (item == NULL)
	{
		return NULL;
	}

	QVariant var = item->data(Qt::UserRole);
	Instruction *inst = qvariant_cast<Instruction *>(var);
	
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

void BlotGL::findSelectedInstruction(double x, double y)
{
	if (_currInstruct != NULL)
	{
		_currInstruct->select(false);
	}
	
	Instruction *old = _currInstruct;
	_currInstruct = NULL;
	
	Instruction *option = instructionForItem(_list->currentItem());
	if (isPossibleInstruction(option, old, x, y))
	{
		_currInstruct = option;
	}

	for (int i = _list->count() - 1; i >= 0 && _currInstruct == NULL; i--)
	{
		Instruction *option = instructionForItem(_list->item(i));

		if (isPossibleInstruction(option, old, x, y))
		{
			_currInstruct = option;
			break;
		}
	}
	

	if (_currInstruct == NULL)
	{
		std::cout << "No selection" << std::endl;
		return;
	}
	else
	{
		BlotObject *obj = _currInstruct->object();
		std::cout << "Selected " << obj->getImage()->text() << std::endl;
		_currInstruct->select(true);

	}
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
		_currInstruct->rotateFractional(from_x, -from_y, frac_x, -frac_y);
	}
	else if (!_shiftPressed)
	{
		_currInstruct->moveFractional(-frac_y, frac_x);
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

		findSelectedInstruction(x, y);
	}
}

void BlotGL::clearAll()
{
	for (size_t i = 0; i < _objects.size(); i++)
	{
		_objects[i]->setDisabled(true);
	}

}

void BlotGL::addProperties()
{
	addDoubleProperty("aspect_ratio", &_aspectRatio);
	
	for (size_t i = 0; i < _objects.size(); i++)
	{
		addChild("blot_object", _objects[i]);
	}

	if (_list == NULL)
	{
		return;
	}

	for (int i = 0; i < _list->count(); i++)
	{
		QListWidgetItem *item = _list->item(i);
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
		QListWidgetItem *item = new QListWidgetItem();
		Instruction *inst = static_cast<Instruction *>(child);
		QVariant var = QVariant::fromValue<Instruction *>(inst);

		item->setData(Qt::UserRole, var);
		item->setText("Temp");
		
		_list->addItem(item);
	}
}

void BlotGL::postParseTidy()
{
	for (int i = 0; i < _list->count(); i++)
	{
		QListWidgetItem *item = _list->item(i);
		Instruction *inst = instructionForItem(item);
		inst->setListItem(item);
		inst->updateText();
	}

	setAspectRatio(_aspectRatio);
}

bool BlotGL::imageInUse(ImageProc *image)
{
	for (size_t i = 0; i < _objects.size(); i++)
	{
		if (_objects[i]->getImage() == image)
		{
			return true;
		}
	}
	
	return false;
}

void BlotGL::removeImageReferences(ImageProc *image)
{
	/* remove instructions first */
	for (int i = 0; i < _list->count(); i++)
	{
		Instruction *inst = instructionForItem(_list->item(i));
		
		if (inst->object() && inst->object()->getImage() == image)
		{
			_list->takeItem(i);
			i--;
		}
	}

	/* remove blot objects */

	for (size_t i = 0; i < _objects.size(); i++)
	{
		if (_objects[i]->getImage() == image)
		{
			_objects.erase(_objects.begin() + i);
			i--;
		}
	}
	
	selectInEditMode();
}

mat3x3 BlotGL::getAspectMatrix()
{
	if (parent() == NULL)
	{
		return _aspect;
	}
	
	return make_mat3x3();
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
