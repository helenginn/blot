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
#include <QTreeWidget>
#include <h3dsrc/SlipGL.h>
#include <vector>
#include "Parser.h"
#include "mat3x3.h"
#include "Set.h"

class BlotObject;
class EditGroup;
class ImageProc;
class Instruction;
class Properties;
class QPushButton;

class BlotGL : public SlipGL, public Set
{
	Q_OBJECT
	
public:
	BlotGL(QWidget *parent = NULL);
	
	void makeList(QWidget *p);
	void addObject(SlipObject *obj, bool top = false);
	void addImage(ImageProc *proc);
	void advancePresentation();
	void triggerSimultaneousInstructions(Instruction *first, bool pass);
	bool addToAnimated(Instruction *next);
	Instruction *instructionForItem(QTreeWidgetItem *item);
	
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
	
	virtual QTreeWidget *list()
	{
		return _list;
	}
	
	void setAspectRatio(double ratio);
	
	double aspectRatio()
	{
		return _aspectRatio;
	}
	
	QPushButton *button(int i)
	{
		return _buttons[i];
	}
	
	size_t buttonCount()
	{
		return _buttons.size();
	}

	virtual Instruction *instruction(int i);
	virtual int instructionCount();
	virtual int indexOfInstruction(Instruction *inst);
	virtual void addInstruction(Instruction *inst, bool atRow = true);
	virtual Instruction *takeInstruction(Instruction *inst);
	
	void clearAll();
	void setFullScreen();
	void setSmallWindow();
	
	bool imageInUse(ImageProc *image);
	void addHideCurrentImage();
	void addMoveCurrentImage();
	void addWiggleCurrentImage();
	void removeImageReferences(ImageProc *image);
	void updateDisplay();
	
	void copyToGL(BlotGL *another);
	void selectAll();
	void moveInstToBottom();
	void moveInstToWipe();
	void moveInstToIndex(int index);
	void changeClick(bool click);
	virtual void addProperties();
	
	virtual void updateProjection(double side = 0.5);
	virtual void insertInstruction(Instruction *inst, int idx);
	void selectInstruction(Instruction *inst);
	Instruction *currentInstruction();
	
	void makeSet();
	virtual void addSet(QList<QTreeWidgetItem *> list =
	                    QList<QTreeWidgetItem *>());
	virtual Set *instructionParent(); 
	void deselectAll();
	void selectInstruction(Instruction *inst, bool primary);
public slots:
	void selectInEditMode();
	void moveInstructionUp();
	void moveInstructionDown();
	void deleteInstructions();
	void progressAnimations();
	void toggleVKey();
	
protected:
	virtual void initializeGL();

	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);

	virtual void addObject(Parser *child, std::string name);
	virtual void postParseTidy();

private:
	EditGroup editGroup();
	void selectInstruction();
	void goBackOneSlide();
	void moveInstruction(int diff);
	std::vector<unsigned int> getSortedIndices();
	Instruction *findSelectedInstruction(double x, double y);
	void replaceSelectedInstruction(double x, double y);
	
	bool isFullScreen()
	{
		return _fullScreen;
	}
	
	QTreeWidget *_list;
	QPushButton *_bDelete;
	QPushButton *_bUp;
	QPushButton *_bDown;
	QPushButton *_bView;
	QPushButton *_bMore;
	std::vector<QPushButton *> _buttons;

	Instruction *_currInstruct;
	std::vector<Instruction *> _otherInstruct;

	std::vector<Instruction *> _animating;
	QObject *_parent;
	bool _editMode;
	bool _fullScreen;
	bool _altPressed;
	int _startX;
	int _startY;
	mat4x4 _aspect;
	double _aspectRatio;
	QTimer *_timer;
};


#endif
