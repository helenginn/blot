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

#include <QClipboard>
#include <QMimeData>
#include <QMenuBar>
#include <QApplication>
#include <iostream>
#include <fstream>

#include "Library.h"
#include "ImageProc.h"

Library *Library::_lib = NULL;

#ifdef __APPLE__
#define MENU_HEIGHT 0
#else
#define MENU_HEIGHT 25
#endif

#define DEFAULT_LIBRARY_HEIGHT 800
#define DEFAULT_LIBRARY_WIDTH 1000
#define IMAGE_TITLE_HEIGHT 30

#define ELABORATION_MIDPOINT ((DEFAULT_LIBRARY_WIDTH + LIST_WIDTH) / 2)
#define ELABORATION_WIDTH (DEFAULT_LIBRARY_WIDTH - LIST_WIDTH)

#define LIST_WIDTH 200

Library::Library()
{
	_imageLabel = NULL;
	_edit = NULL;

	this->resize(DEFAULT_LIBRARY_WIDTH, DEFAULT_LIBRARY_HEIGHT);
	this->setWindowTitle("Blot Library");
	_list = new QListWidget(this);
	_list->setGeometry(0, MENU_HEIGHT, LIST_WIDTH, DEFAULT_LIBRARY_HEIGHT);
	_list->show();

	QMenu *edit = menuBar()->addMenu(tr("&Edit"));
	QAction *action = edit->addAction(tr("Paste image"));
	action->setShortcut(QKeySequence::Paste);
	connect(action, &QAction::triggered, this, &Library::paste);
	connect(_list, &QListWidget::itemSelectionChanged, 
	        this, &Library::elaborate);
}

void Library::paste()
{
	const QClipboard *clip = QApplication::clipboard();
	const QMimeData *mimeData = clip->mimeData();

	if (!mimeData->hasImage())
	{
		std::cout << "No image" << std::endl;
		return;
	} 

	QImage image = clip->image();
	
	ImageProc *tmp = new ImageProc(&image);
	tmp->process();
	QListWidgetItem *item = new QListWidgetItem();
	QVariant var = QVariant::fromValue<ImageProc *>(tmp);
	item->setData(Qt::UserRole, var);
	item->setText("New image");

	_list->addItem(item);
	elaborateItem(item);

	if (_edit)
	{
		_edit->selectAll();
		_edit->setFocus();
		_edit->setFocusPolicy(Qt::StrongFocus);
	}
}

void Library::setCurrentLibrary(Library *lib)
{
	_lib = lib;
}

void Library::elaborate()
{
	elaborateItem(_list->currentItem());
}

void Library::clearElaboration()
{
	if (_imageLabel != NULL)
	{
		_imageLabel->hide();
		delete _imageLabel; _imageLabel = NULL;
	}

	if (_edit != NULL)
	{
		_edit->hide();
		delete _edit; _edit = NULL;
	}
}

void Library::elaborateItem(QListWidgetItem *item)
{
	if (item == NULL)
	{
		return;
	}

	_list->setCurrentItem(item);
	ImageProc *proc = imageProcForItem(item);
	QImage *im = proc->getImage();
	
	float width = im->width();
	float height = 300;
	if (im->height() > height)
	{
		width = im->width() * 300 / im->height();
	}
	else
	{
		height = im->height();
	}

	if (_imageLabel == NULL)
	{
		_imageLabel = new QLabel(this);
	}

	_imageLabel->setPixmap(QPixmap::fromImage(im->scaled(width, height)));
	_imageLabel->setGeometry(ELABORATION_MIDPOINT - width / 2, 
	                    MENU_HEIGHT + IMAGE_TITLE_HEIGHT,
	                    width, height);
	_imageLabel->show();
	
	if (_edit == NULL)
	{
		_edit = new QLineEdit(this);
	}

	_edit->setGeometry(LIST_WIDTH + 10, MENU_HEIGHT + 10,
	                   ELABORATION_WIDTH, IMAGE_TITLE_HEIGHT);
	_edit->setText(item->text());
	_edit->show();
	
	connect(_edit, &QLineEdit::editingFinished, this, &Library::updateTitle);
}

ImageProc *Library::imageProcForItem(QListWidgetItem *item)
{
	QVariant var = item->data(Qt::UserRole);
	ImageProc *proc = qvariant_cast<ImageProc *>(var);
	
	return proc;
}

void Library::updateTitle()
{
	_list->currentItem()->setText(_edit->text());
	_list->setFocus();

	std::ofstream file;
	file.open("library.blot");
	writeToFile(file, 0);
	file.close();
}

void Library::addProperties()
{
	for (int i = 0; i < _list->count(); i++)
	{
		QListWidgetItem *item = _list->item(i);
		ImageProc *proc = imageProcForItem(item);
		Parser *p = static_cast<Parser *>(proc);
		
		addChild("image_proc", p);
	}
}

Library::~Library()
{
	clearElaboration();

}
