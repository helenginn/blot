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
#include "Instruction.h"
#include "Library.h"
#include "StartScreen.h"
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QIcon>
#include <QPushButton>
#include <QStyle>
#include <QScreen>
#include <QWindow>
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
	selectInstruction();
}

void BlotGL::moveInstructionUp()
{
	moveInstruction(-1);
}

void BlotGL::moveInstructionDown()
{
	moveInstruction(1);
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
	double current = width() / (double)height();
	_aspect.vals[0] = 1 / (ratio);
}

BlotGL::BlotGL(QWidget *p) : QOpenGLWidget(p)
{
	_prop = NULL;
	_parent = NULL;
	_list = NULL;
	_currPos = 0;
	_editMode = true;
	_controlPressed = false;
	_shiftPressed = false;
	_currInstruct = NULL;
	_aspectRatio = 1;
	_aspect = make_mat3x3();
	
	if (p == NULL)
	{
		p = StartScreen::startScreenPtr;
	}

	makeList(p);
	
	connect(_list, &QListWidget::currentRowChanged, 
	        this, &BlotGL::selectInstruction);

	advancePresentation();
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
	_objects.push_back(obj);
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

void BlotGL::advancePresentation(bool clicked)
{
	if (_list == NULL)
	{
		return;
	}

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
		
		inst->makeEffect();
		update();
		clicked = false;
		_list->setCurrentRow(_currPos);
		_currPos++;
	}
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
	_controlPressed = false;
}

void BlotGL::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Shift)
	{
		_shiftPressed = true;
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
			clearAll();
			setFullScreen();
		}

		setFocus();
		setFocusPolicy(Qt::StrongFocus);
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
	QVariant var = item->data(Qt::UserRole);
	Instruction *inst = qvariant_cast<Instruction *>(var);
	
	return inst;
}


void BlotGL::findSelectedInstruction(double x, double y)
{
	if (_currInstruct != NULL)
	{
		_currInstruct->select(false);
	}

	Instruction *old = _currInstruct;
	_currInstruct = NULL;

	for (int i = _list->count() - 1; i >= 0; i--)
	{
		Instruction *option = instructionForItem(_list->item(i));

		if (!option->canMove())
		{
			continue;
		}
		
		if (option == old)
		{
			continue;
		}

		bool cover = option->isCovered(-y, x);

		if (cover)
		{
			_currInstruct = option;
//			_list->setCurrentItem(_list->item(i));
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
	
	if (!_shiftPressed)
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
	
	selectInstruction();
}

mat3x3 BlotGL::getAspectMatrix()
{
	if (parent() == NULL)
	{
		return _aspect;
	}
	
	return make_mat3x3();
}
