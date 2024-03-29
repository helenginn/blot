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
#include "WipeSlate.h"
#include "Library.h"
#include "BlotGL.h"

StartScreen *StartScreen::startScreenPtr = NULL;

StartScreen::StartScreen(QWidget *parent,
                         int argc, char *argv[]) : QMainWindow(parent)
{
	this->resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

	this->setWindowTitle("Blot");
	
	startScreenPtr = this;

	_argc = argc;
	_argv = argv;
	
	_lib = NULL;
	_pres = NULL;

	QMenu *file = menuBar()->addMenu(tr("&File"));
	QAction *action = file->addAction(tr("New library"));
	action->setShortcut(QKeySequence::New);
	connect(action, &QAction::triggered, this, &StartScreen::newLibrary);

	action = file->addAction(tr("Open library"));
	action->setShortcut(QKeySequence::Open);
	connect(action, &QAction::triggered, this, &StartScreen::openLibrary);

	file->addSeparator();

	QMenu *edit = menuBar()->addMenu(tr("&Edit"));
	action = edit->addAction(tr("Select all"));
	action->setShortcut(QKeySequence::SelectAll);
	connect(action, &QAction::triggered, this, &StartScreen::selectAll);

	QMenu *aspects = file->addMenu(tr("Choose aspect ratio"));
	action = aspects->addAction(tr("4:3"));
	connect(action, &QAction::triggered, this, &StartScreen::aspect4t3);
	action = aspects->addAction(tr("16:9"));
	connect(action, &QAction::triggered, this, &StartScreen::aspect16t9);

	QMenu *insert = menuBar()->addMenu(tr("&Insert"));
	action = insert->addAction(tr("Wipe slate"));
	connect(action, &QAction::triggered, this, &StartScreen::addWipe);
	action = insert->addAction(tr("Hide image"));
	action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_H));
	connect(action, &QAction::triggered, this, &StartScreen::addHide);
	action = insert->addAction(tr("Move image"));
	action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_M));
	connect(action, &QAction::triggered, this, &StartScreen::addMove);
	action = insert->addAction(tr("Wiggle image"));
	connect(action, &QAction::triggered, this, &StartScreen::addWiggle);
	action = insert->addAction(tr("Make set"));
	connect(action, &QAction::triggered, this, &StartScreen::makeSet);

	QMenu *instruction = menuBar()->addMenu(tr("&Instruction"));
	_waitClick = instruction->addAction(tr("Wait for click"));
	_waitClick->setCheckable(true);
	_waitClick->setShortcut(QKeySequence(Qt::ALT + Qt::Key_C));
	connect(_waitClick, &QAction::triggered, 
	        this, &StartScreen::changeClick);

	action = instruction->addAction(tr("Move to bottom"));
	action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_B));
	connect(action, &QAction::triggered, 
	        this, &StartScreen::moveInstToBottom);

	action = instruction->addAction(tr("Move down to Wipe Slate"));
	action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_W));
	connect(action, &QAction::triggered, 
	        this, &StartScreen::moveInstToWipe);
} 

void StartScreen::moveInstToWipe()
{
	_pres->moveInstToWipe();
}

void StartScreen::moveInstToBottom()
{
	_pres->moveInstToBottom();
}

void StartScreen::changeClick()
{
	_pres->changeClick(_waitClick->isChecked());
}

void StartScreen::setClick(Instruction *inst)
{
	if (inst != NULL)
	{
		_waitClick->setChecked(inst->waitForClick());
	}
}

void StartScreen::aspect16t9()
{
	_pres->setAspectRatio(16./9.);
	resizeEvent(NULL);
}

void StartScreen::aspect4t3()
{
	_pres->setAspectRatio(4./3.);
	resizeEvent(NULL);
}

void StartScreen::resizeEvent(QResizeEvent *event)
{
	if (_pres)
	{
		std::cout << "Resizing widgets" << std::endl;
		double asp = _pres->aspectRatio();
		int plus_x = 0;
		int plus_y = 0;
		int pres_width = width() - INSTRUCTION_WIDTH;
		int pres_height = pres_width / asp;

		if (pres_height > height())
		{
			pres_height = height() - MENU_HEIGHT;
			pres_width = height() * asp;
			plus_x += (width() - INSTRUCTION_WIDTH - pres_width) / 2;
		}
		else
		{
			plus_y += (height() - pres_height - MENU_HEIGHT) / 2;
		}

		_pres->setGeometry(INSTRUCTION_WIDTH + plus_x, MENU_HEIGHT + plus_y, 
		                   pres_width, pres_height);
		
		if (_pres->list())
		{
			_pres->list()->setGeometry(0, MENU_HEIGHT, INSTRUCTION_WIDTH, 
			                           height() - MENU_HEIGHT 
			                           - QUICK_BUTTON_HEIGHT);

			for (size_t i = 0; i < _pres->buttonCount(); i++)
			{
				QRect geom = _pres->button(i)->geometry();
				geom.setTop(height() - QUICK_BUTTON_HEIGHT);
				geom.setHeight(QUICK_BUTTON_HEIGHT);
				_pres->button(i)->setGeometry(geom);
			}
		}
	}
	else
	{
		std::cout << "No presentation; not resizing" << std::endl;
	}
}

void StartScreen::openLibraryFile(std::string lib)
{
	std::string contents = get_file_contents(lib);
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
	_lib->setFilename(lib);
	_lib->show();
	_pres = _lib->presentation();
	_pres->QWidget::setParent(this);
	_pres->setGeometry(geometry());

	_pres->setSmallWindow();
	_pres->setFocus();
	_pres->setFocusPolicy(Qt::StrongFocus);
}

void StartScreen::openLibrary()
{
	std::cout << "We open a library" << std::endl;
	QString types = "Blot files (*.blot)";
	QFileDialog *fileDialogue = new QFileDialog(this, "Choose library", 
	                                            types);
	
	fileDialogue->setNameFilter(types);
	fileDialogue->setFileMode(QFileDialog::AnyFile);
//	fileDialogue->show();

	QStringList fileNames;
	if (fileDialogue->exec())
	{
		fileNames = fileDialogue->selectedFiles();
	}

	std::cout << "Finished dialogue box" << std::endl;

	if (fileNames.size() <= 0)
	{
		std::cout << "No filenames" << std::endl;
		return;
	}

	std::cout << "Parsing" << std::endl;

	openLibraryFile(fileNames[0].toStdString());
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
	_pres = _lib->presentation();
	_pres->QWidget::setParent(this);
	_pres->setSmallWindow();
	_pres->setFocus();
	_pres->setFocusPolicy(Qt::StrongFocus);
}

void StartScreen::addWipe()
{
	WipeSlate *wipe = new WipeSlate(_pres);
	_pres->addInstruction(wipe);
}

void StartScreen::makeSet()
{
	_pres->makeSet();
}

void StartScreen::addHide()
{
	_pres->addHideCurrentImage();
}

void StartScreen::addMove()
{
	_pres->addMoveCurrentImage();
}

void StartScreen::addWiggle()
{
	_pres->addWiggleCurrentImage();
}

StartScreen::~StartScreen()
{
}

void StartScreen::selectAll()
{
	_pres->selectAll();
}

