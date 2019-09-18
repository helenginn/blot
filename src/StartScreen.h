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

#ifndef __Blot__StartScreen__
#define __Blot__StartScreen__

#define INSTRUCTION_WIDTH 200
#define DEFAULT_WIDTH 1000
#define DEFAULT_HEIGHT 600
#define QUICK_BUTTON_HEIGHT 40

#include <QMainWindow>

class BlotGL;
class Library;
class Instruction;
class QAction;

class StartScreen : public QMainWindow
{
	Q_OBJECT
	
public:
	StartScreen(QWidget *parent = 0, int argc = 0, char *argv[] = NULL);
	~StartScreen();
	
	static StartScreen *startScreenPtr;

	void resizeWidgets()
	{
		resizeEvent(NULL);
	}

	void setClick(Instruction *inst);
protected:
	virtual void resizeEvent(QResizeEvent *event);

private slots:
	void newLibrary();
	void openLibrary();
	void addWipe();
	void addHide();
	void addMove();
	void aspect4t3();
	void aspect16t9();
	void changeClick();
	void moveInstToBottom();
	void moveInstToWipe();
	
private:
	QAction *_waitClick;
	BlotGL *_pres;
	Library *_lib;
	char **_argv;
	int _argc;

};

#endif
