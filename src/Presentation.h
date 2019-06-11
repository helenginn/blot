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

#ifndef __Blot__Presentation__
#define __Blot__Presentation__

#include <QtWidgets/qmainwindow.h>
#include "BlotGL.h"
#include "Parser.h"

class ImageProc;
class Instruction;

class Presentation : public QMainWindow, public Parser
{
	Q_OBJECT

public:
	Presentation();
	void addImage(ImageProc *proc);
	void addInstruction(Instruction *inst);
	void advancePresentation(bool clicked = false);
	
	void setEditMode(bool edit)
	{
		_editMode = edit;
	}
	
	virtual std::string getClassName()
	{
		return "Presentation";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "MainPresentation";
	}
	
	virtual void addProperties();
protected:
	void resizeEvent(QResizeEvent *);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	
private:
	bool _editMode;
	std::vector<Instruction *> _instructions;
	int _currPos;
	BlotGL *_display;
	QTimer *_timer;
};

#endif
