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

#include <QMouseEvent>
#include <QWindow>
#include <QMenuBar>
#include <QApplication>
#include <QFileDialog>
#include <QScreen>
#include "charmanip.h"
#include "StartScreen.h"
#include "Library.h"
#include "BlotGL.h"

StartScreen::StartScreen(QWidget *parent,
                         int argc, char *argv[]) : QMainWindow(parent)
{
	this->resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

	this->setWindowTitle("Blot");

	_argc = argc;
	_argv = argv;
	
	_lib = NULL;

	QMenu *edit = menuBar()->addMenu(tr("&File"));
	QAction *action = edit->addAction(tr("New library"));
	action->setShortcut(QKeySequence::New);
	connect(action, &QAction::triggered, this, &StartScreen::newLibrary);

	action = edit->addAction(tr("Open library"));
	action->setShortcut(QKeySequence::Open);
	connect(action, &QAction::triggered, this, &StartScreen::openLibrary);
} 

void StartScreen::openLibrary()
{
	QString types = "Blot files (*.blot)";
	QFileDialog *fileDialogue = new QFileDialog(this, "Choose library", 
	                                            types);
	
	fileDialogue->setNameFilter(types);
	fileDialogue->setFileMode(QFileDialog::AnyFile);
	fileDialogue->show();

	QStringList fileNames;
	if (fileDialogue->exec())
	{
		fileNames = fileDialogue->selectedFiles();
	}

	if (fileNames.size() <= 0)
	{
		return;
	}

	std::string contents = get_file_contents(fileNames[0].toStdString());
	const char *block = contents.c_str();
	char *dup = strdup(block);
	Parser *p = Parser::processBlock(dup);
	free(dup);
	
	if (p == NULL)
	{
		return;
	}

	if (_lib)
	{
		delete _lib;
		_lib = NULL;
	}

	_lib = static_cast<Library *>(p);
	_lib->setFilename(fileNames[0].toStdString());
	_lib->show();

	drawEditMode();
}

void StartScreen::drawEditMode()
{
	if (_lib == NULL)
	{
		return;
	}

	std::cout << "Draw edit mode" << std::endl;
	_pres = _lib->presentation();
	_pres->QOpenGLWidget::setParent(this);
	_pres->setGeometry(INSTRUCTION_WIDTH, MENU_HEIGHT, 
	                   DEFAULT_WIDTH - INSTRUCTION_WIDTH, DEFAULT_HEIGHT);
	_pres->show();
}

void StartScreen::newLibrary()
{
	if (_lib)
	{
		delete _lib;
		_lib = NULL;
	}

	_lib = new Library();
	_lib->show();
	drawEditMode();
}

void StartScreen::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_V && _lib != NULL)
	{
		_pres = _lib->presentation();
		_pres->setFullScreen();
	}
}

StartScreen::~StartScreen()
{
}

