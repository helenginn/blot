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
#include <QListWidget>
#include <vector>
#include "Parser.h"
#include "mat3x3.h"

class BlotObject;
class ImageProc;
class Instruction;
class Properties;
class QPushButton;

class BlotGL : public QOpenGLWidget, QOpenGLFunctions, public Parser
{
	Q_OBJECT
	
public:
	BlotGL(QWidget *parent = NULL);
	
	void makeList(QWidget *p);
	void addObject(BlotObject *obj, bool top = false);
	void addImage(ImageProc *proc);
	void addInstruction(Instruction *inst, bool atRow = true);
	void advancePresentation(bool clicked = false);
	Instruction *instructionForItem(QListWidgetItem *item);
	
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
	
	virtual QListWidget *list()
	{
		return _list;
	}
	
	void setAspectRatio(double ratio);
	
	double aspectRatio()
	{
		return _aspectRatio;
	}
	
	mat3x3 getAspectMatrix();
	
	QPushButton *button(int i)
	{
		return _buttons[i];
	}
	
	size_t buttonCount()
	{
		return _buttons.size();
	}
	
	void clearAll();
	void setFullScreen();
	void setSmallWindow();
	
	bool imageInUse(ImageProc *image);
	void addHideCurrentImage();
	void addMoveCurrentImage();
	void removeImageReferences(ImageProc *image);
	void updateDisplay();
	
	void moveInstToBottom();
	void moveInstToWipe();
	void changeClick(bool click);
	virtual void addProperties();
public slots:
	void selectInEditMode();
	void moveInstructionUp();
	void moveInstructionDown();
	void deleteInstruction();
	void changeInstruction();
	void progressAnimations();
	void toggleVKey();
	
protected:
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL(int w, int h);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);

	virtual void addObject(Parser *child, std::string name);
	virtual void postParseTidy();
private:
	void selectInstruction();
	void goBackOneSlide();
	void moveInstruction(int diff);
	void initialisePrograms();
	void findSelectedInstruction(double x, double y);
	
	bool isFullScreen()
	{
		return _fullScreen;
	}
	
	QListWidget *_list;
	QPushButton *_bDelete;
	QPushButton *_bUp;
	QPushButton *_bDown;
	QPushButton *_bView;
	QPushButton *_bMore;
	std::vector<QPushButton *> _buttons;
	Instruction *_currInstruct;
	std::vector<Instruction *> _animating;
	Properties *_prop;
	std::vector<BlotObject *> _objects;
	QObject *_parent;
	bool _editMode;
	bool _fullScreen;
	bool _shiftPressed;
	bool _controlPressed;
	bool _altPressed;
	int _currPos;
	int _startX;
	int _startY;
	int _lastX;
	int _lastY;
	mat3x3 _aspect;
	double _aspectRatio;
	QTimer *_timer;
};


#endif
