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
	Library();
	Library(StartScreen *scr);
	virtual ~Library();
	
	static void setCurrentLibrary(Library *lib);
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
	void paste();
	void elaborate();
	void updateTitle();
	void addToPresentation();
	void deleteFromLibrary();

private:
	ImageProc *imageProcForItem(QListWidgetItem *item);
	void elaborateItem(QListWidgetItem *item);
	void clearElaboration();
	QListWidget *_list;	
	QLabel *_imageLabel;
	QPushButton *_addToPres;
	QPushButton *_bDelete;
	QLineEdit *_edit;
	std::string _filename;
	BlotGL *_pres;

	static Library *_lib;
};

#endif
