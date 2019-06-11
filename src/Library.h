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

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include "Parser.h"

class ImageProc;
class QListWidgetItem;

class Library : public QMainWindow, public Parser
{
	Q_OBJECT
public:
	Library();
	virtual ~Library();
	
	static void setCurrentLibrary(Library *lib);
	
	virtual std::string getClassName()
	{
		return "Library";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "MainLibrary";
	}
	
	virtual void addProperties();
	
public slots:
	void paste();
	void elaborate();
	void updateTitle();

private:
	ImageProc *imageProcForItem(QListWidgetItem *item);
	void elaborateItem(QListWidgetItem *item);
	void clearElaboration();
	QListWidget *_list;	
	QLabel *_imageLabel;
	QLineEdit *_edit;

	static Library *_lib;
};

#endif
