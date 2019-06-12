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
#include "BlotObject.h"
#include "ImageProc.h"
#include "Instruction.h"
#include "Library.h"
#include "StartScreen.h"
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
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

BlotGL::BlotGL(QWidget *p) : QOpenGLWidget(p, 0)
{
	_currPos = 0;
	_editMode = true;
	_currInstruct = NULL;
	advancePresentation();
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
		_objects[i]->render();
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
	while (true)
	{
		if ((int)_instructions.size() <= _currPos)
		{
			break;
		}
		
		if (_instructions[_currPos]->waitForClick() && !clicked)
		{
			break;
		}
		
		Instruction *inst = _instructions[_currPos];
		inst->makeEffect();
		update();
		clicked = false;
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
	show();
	windowHandle()->setScreen(screens.last());
	showFullScreen();
}

void BlotGL::setSmallWindow()
{
	std::cout << "Making small window" << std::endl;
	hide();
	StartScreen *w = static_cast<StartScreen *>(_parent);
	if (w)
	{
		w->drawEditMode();
	}
}

void BlotGL::addImage(ImageProc *proc)
{
	BlotObject *obj = new BlotObject(proc);
	addObject(obj);
}

void BlotGL::addInstruction(Instruction *inst)
{
	_instructions.push_back(inst);
	addObject(inst->object());
}

void BlotGL::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_V)
	{
		setSmallWindow();
	}
}

void BlotGL::mousePressEvent(QMouseEvent *e)
{
	_startX = e->x();
	_startY = e->y();
	_lastX = _startX;
	_lastY = _startY;

}

void BlotGL::findSelectedInstruction(double x, double y)
{
	if (_currInstruct != NULL)
	{
		_currInstruct->select(false);
	}

	Instruction *old = _currInstruct;
	_currInstruct = NULL;

	for (int i = _instructions.size() - 1; i >= 0; i--)
	{
		if (!_instructions[i]->canMove())
		{
			continue;
		}
		
		if (_instructions[i] == old)
		{
			continue;
		}

		BlotObject *option = _instructions[i]->object();
		bool cover = option->isCovered(x, y);

		if (cover)
		{
			_currInstruct = _instructions[i];
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
	_currInstruct->moveFractional(-frac_y, frac_x);
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

}
