// BLot
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

#ifndef __Blot__StartScreen__
#define __Blot__StartScreen__

#define INSTRUCTION_WIDTH 200
#define DEFAULT_WIDTH 1000
#define DEFAULT_HEIGHT 600


#include <QMainWindow>

class BlotGL;
class Library;

class StartScreen : public QMainWindow
{
	Q_OBJECT
	
public:
	StartScreen(QWidget *parent = 0, int argc = 0, char *argv[] = NULL);
	~StartScreen();
	
	void drawEditMode();
protected:
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void keyPressEvent(QKeyEvent *event);

private slots:
	void newLibrary();
	void openLibrary();
	
private:
	BlotGL *_pres;
	Library *_lib;
	char **_argv;
	int _argc;

};

#endif
