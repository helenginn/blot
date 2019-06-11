// Blot
// Copyright (C) 2019- Helen Ginn
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

#define DEFAULT_WIDTH 600
#define DEFAULT_HEIGHT 400

#include <QMouseEvent>
#include <QWindow>
#include "StartScreen.h"
#include <QApplication>
#include "Presentation.h"

StartScreen::StartScreen(QWidget *parent,
                         int argc, char *argv[]) : QMainWindow(parent)
{
	this->resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

	this->setWindowTitle("Blot");

	_argc = argc;
	_argv = argv;
	
}

void StartScreen::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_V)
	{
		QList<QScreen *> screens = qApp->screens();
		_pres = new Presentation();

		_pres->show();
		_pres->windowHandle()->setScreen(screens.last());
		_pres->showFullScreen();
/*		
		if (_tmp)
		{
			_pres->addImage(_tmp);
		}
		*/
	}
}

void StartScreen::mousePressEvent(QMouseEvent *e)
{
	
}

StartScreen::~StartScreen()
{
}

