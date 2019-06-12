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

#ifndef __Blot__BlotGL__
#define __Blot__BlotGL__

#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qopengl.h>
#include <QtGui/qopenglfunctions.h>
#include <vector>
#include "Parser.h"

class BlotObject;
class ImageProc;
class Instruction;

class BlotGL : public QOpenGLWidget, QOpenGLFunctions, public Parser
{
	Q_OBJECT
	
public:
	BlotGL(QWidget *parent = NULL);
	BlotGL(BlotGL &other);
	
	void addObject(BlotObject *obj);
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
	
	void dodgyRefresh();
	void setFullScreen();
	void setSmallWindow();
	
	void updateDisplay();
	virtual void addProperties();
public slots:
	
protected:
	void resizeEvent(QResizeEvent *);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);

private:
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL();
	void initialisePrograms();
	
	std::vector<BlotObject *> _objects;
	QObject *_parent;
	bool _editMode;
	std::vector<Instruction *> _instructions;
	int _currPos;
	QTimer *_timer;
};


#endif
