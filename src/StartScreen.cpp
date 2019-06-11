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

#define DEFAULT_WIDTH 600
#define DEFAULT_HEIGHT 400

#include <QMouseEvent>
#include <QWindow>
#include <QMenuBar>
#include <QApplication>
#include <QFileDialog>
#include "charmanip.h"
#include "StartScreen.h"
#include "Library.h"
#include "Presentation.h"

StartScreen::StartScreen(QWidget *parent,
                         int argc, char *argv[]) : QMainWindow(parent)
{
	this->resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

	this->setWindowTitle("Blot");

	_argc = argc;
	_argv = argv;

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

	Library *l = static_cast<Library *>(p);
	l->setFilename(fileNames[0].toStdString());
	l->show();
}

void StartScreen::newLibrary()
{
	Library *lib = new Library();
	lib->show();
}

void StartScreen::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_V)
	{
		QList<QScreen *> screens = qApp->screens();
		_pres = new Presentation();

		_pres->show();
		_pres->windowHandle()->setScreen(screens.last());
		_pres->showFullScreen();
	}
}

void StartScreen::mousePressEvent(QMouseEvent *e)
{
	
}

StartScreen::~StartScreen()
{
}

