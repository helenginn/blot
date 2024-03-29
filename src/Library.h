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

#ifndef __Blot__Library__
#define __Blot__Library__

#ifdef __APPLE__
#define MENU_HEIGHT 0
#else
#define MENU_HEIGHT 25
#endif

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include "Parser.h"

class ImageProc;
class QListWidgetItem;
class BlotGL;
class StartScreen;

class Library : public QMainWindow, public Parser
{
	Q_OBJECT
public:
	Library(StartScreen *scr = NULL);
	virtual ~Library();
	
	void initialise();
	
	virtual std::string getClassName()
	{
		return "Library";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "MainLibrary";
	}
	
	void setFilename(std::string file)
	{
		_filename = file;
	}
	
	void setPresentation(BlotGL *gl)
	{
		_pres = gl;
	}
	
	BlotGL *presentation()
	{
		return _pres;
	}
	
	virtual void addProperties();
	virtual void addObject(Parser *child, std::string name);
	
public slots:
	void save();
	void saveAs();
	void mergeWith();
	void paste();
	void elaborate();
	void updateTitle();
	void addToPresentation();
	void fadeToPresentation();
	void deleteFromLibrary();
	void updatePaste();
	void changeBackground();
	void changeProcessing();
	void copyToClipboard();

	virtual void mousePressEvent(QMouseEvent *event);
private:
	ImageProc *imageProcForItem(QListWidgetItem *item);
	void copyToLibrary(Library *another);
	void getImageFromClipboard(QImage *im);
	void elaborateItem(QListWidgetItem *item);
	void clearElaboration();
	QListWidget *_list;	
	QLabel *_imageLabel;
	QGraphicsView *_pointView;
	QPushButton *_addToPres;
	QPushButton *_addFade;
	QPushButton *_bDelete;
	QPushButton *_bCopy;
	QPushButton *_bUpdate;
	QPushButton *_bChange;
	QPushButton *_bProcess;
	QLineEdit *_edit;
	QGraphicsScene *_graphics;
	std::string _filename;
	BlotGL *_pres;
	bool _seeding;
	std::vector<vec3> _points;
};

#endif
