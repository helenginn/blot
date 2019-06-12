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

BlotGL::BlotGL(BlotGL &other)
{
	_objects = other._objects;
	_parent = other._parent;
	_editMode = other._editMode;
	_instructions = other._instructions;
	_currPos = other._currPos;
	_timer = other._timer;
}

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
	std::cout << "Resizing " << w << " " << h << std::endl;

}

void BlotGL::paintGL()
{
	std::cout << "Repainting, validity " << isValid() << std::endl;
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

void BlotGL::dodgyRefresh()
{
	return;
	if (parent() == NULL)
	{
		return;
	}

	QWidget::setParent(parentWidget(), Qt::Window);
	QWidget::setParent(parentWidget(), Qt::Widget);

	show();
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

	dodgyRefresh();
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

void BlotGL::mousePressEvent(QMouseEvent *event)
{
	if (true || !_editMode)
	{
		std::cout << "Pressing" << std::endl;
		advancePresentation(true);
	}
}

void BlotGL::addProperties()
{

}
