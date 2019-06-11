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

#include "Library.h"
#include "ImageProc.h"

#define DEFAULT_LIBRARY_HEIGHT 800
#define DEFAULT_LIBRARY_WIDTH 1000

Library::Library()
{
	_tmp = NULL;
	this->resize(DEFAULT_LIBRARY_WIDTH, DEFAULT_LIBRARY_HEIGHT);
	this->setWindowTitle("Blot Library");

	QMenu *edit = menuBar()->addMenu(tr("&Edit"));
	QAction *action = edit->addAction(tr("Paste image"));
	connect(action, &QAction::triggered, this, &Library::paste);
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
	std::cout << image.width() << " " << image.height() << std::endl;
	
	_tmp = new ImageProc(&image);
	_tmp->process();
}

Library::~Library()
{
	if (_tmp != NULL)
	{
		delete _tmp; _tmp = NULL;
	}
}
