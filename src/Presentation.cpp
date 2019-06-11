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

#include "Presentation.h"
#include "Instruction.h"
#include "ImageProc.h"
#include "BlotObject.h"
#include <QTimer>
#include <QKeyEvent>

Presentation::Presentation()
{
	_display = new BlotGL(this);

	_currPos = 0;
	_timer = new QTimer();
	_timer->setInterval(30);
	connect(_timer, SIGNAL(timeout()), _display, SLOT(update()));
	_timer->start();
	
	resizeEvent(NULL);
	advancePresentation();
}

void Presentation::advancePresentation(bool clicked)
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
		_currPos++;
	}
}

void Presentation::addImage(ImageProc *proc)
{
	BlotObject *obj = new BlotObject(proc);
	_display->addObject(obj);
}

void Presentation::addInstruction(Instruction *inst)
{
	_instructions.push_back(inst);
	_display->addObject(inst->object());
}

void Presentation::resizeEvent(QResizeEvent *)
{
	_display->setGeometry(0, 0, width(), height());
}

void Presentation::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_V)
	{
		hide();
		deleteLater();
	}
}

void Presentation::mousePressEvent(QMouseEvent *event)
{
	if (true || !_editMode)
	{
		advancePresentation(true);
	}
}

void Presentation::addProperties()
{

}
