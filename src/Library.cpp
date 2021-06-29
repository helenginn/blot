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
#include <QMouseEvent>
#include <QMessageBox>
#include <QMenuBar>
#include <QApplication>
#include <QGraphicsItem>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <QFileDialog>

#include "Library.h"
#include "BlotGL.h"
#include "StartScreen.h"
#include "ImageProc.h"
#include "ImageAppear.h"
#include "ImageFade.h"
#include "charmanip.h"
#include "ChangeBackground.h"

#define DEFAULT_LIBRARY_HEIGHT 800
#define DEFAULT_LIBRARY_WIDTH 1000
#define IMAGE_TITLE_HEIGHT 30
#define IMAGE_HEIGHT 300
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 40

#define ELABORATION_MIDPOINT ((DEFAULT_LIBRARY_WIDTH + LIST_WIDTH) / 2)
#define ELABORATION_WIDTH (DEFAULT_LIBRARY_WIDTH - LIST_WIDTH)

#define LIST_WIDTH 200

void Library::initialise()
{
	_graphics = NULL;
	_imageLabel = NULL;
	_edit = NULL;
	_addToPres = NULL;
	_addFade = NULL;
	_bProcess = NULL;
	_seeding = false;

	this->resize(DEFAULT_LIBRARY_WIDTH, DEFAULT_LIBRARY_HEIGHT);
	this->setWindowTitle("Blot Library");
	_list = new QListWidget(this);
	_list->setGeometry(0, MENU_HEIGHT, LIST_WIDTH, 
	                   DEFAULT_LIBRARY_HEIGHT - QUICK_BUTTON_HEIGHT);
	_list->show();
	connect(_list, &QListWidget::itemSelectionChanged, 
	        this, &Library::elaborate);

	QMenu *file = menuBar()->addMenu(tr("&File"));
	QAction *action = file->addAction(tr("Save library"));
	action->setShortcut(QKeySequence::Save);
	connect(action, &QAction::triggered, this, &Library::save);

	action = file->addAction(tr("Save library as..."));
	action->setShortcut(QKeySequence::SaveAs);
	connect(action, &QAction::triggered, this, &Library::saveAs);

	action = file->addAction(tr("Merge with library..."));
	connect(action, &QAction::triggered, this, &Library::mergeWith);

	QMenu *edit = menuBar()->addMenu(tr("&Edit"));
	action = edit->addAction(tr("Copy image"));
	action->setShortcut(QKeySequence::Copy);
	connect(action, &QAction::triggered, this, &Library::copyToClipboard);

	action = edit->addAction(tr("Paste image"));
	action->setShortcut(QKeySequence::Paste);
	connect(action, &QAction::triggered, this, &Library::paste);
}

Library::Library(StartScreen *scr)
{
	initialise();
	_pres = new BlotGL(scr);
}

void Library::mergeWith()
{
	QString types = "Blot files (*.blot)";
	QFileDialog *fileDialogue = new QFileDialog(this, "Choose library", 
	                                            types);
	
	fileDialogue->setNameFilter(types);
	fileDialogue->setFileMode(QFileDialog::AnyFile);

	QStringList fileNames;
	if (fileDialogue->exec())
	{
		fileNames = fileDialogue->selectedFiles();
	}

	if (fileNames.size() <= 0)
	{
		std::cout << "No filenames" << std::endl;
		return;
	}

	StartScreen *current = StartScreen::startScreenPtr;
	StartScreen start;
	start.openLibraryFile(fileNames[0].toStdString());
	
	Library *extra = start.getLibrary();
	extra->copyToLibrary(this);
	extra->hide();
	
	StartScreen::startScreenPtr = current;
}

void Library::saveAs()
{
	QString types = "Blot files (*.blot)";
	QString filename = QFileDialog::getSaveFileName(this, "Save library as");
	_filename = filename.toStdString();
	
	if (_filename.length())
	{
		save();
	}
}

void Library::save()
{
	if (_filename.length() == 0)
	{
		saveAs();
		return;
	}
	
	std::string tmp = _filename + ".tmp";

	std::ofstream file;
	file.open(tmp);
	writeToFile(file, 0);
	file.close();
	
	rename(tmp.c_str(), _filename.c_str());
}

void Library::getImageFromClipboard(QImage *im)
{
	const QClipboard *clip = QApplication::clipboard();

	QImage image = clip->image();
	
	*im = image;
}

void Library::paste()
{
	QImage image;
	getImageFromClipboard(&image);
	
	if (image.isNull())
	{
		std::cout << "No image" << std::endl;
		return;
	} 
	
	ImageProc *tmp = new ImageProc(&image);
	tmp->process();
	tmp->setText("New image");
	QListWidgetItem *item = new QListWidgetItem();
	QVariant var = QVariant::fromValue<ImageProc *>(tmp);
	item->setData(Qt::UserRole, var);
	item->setText(tmp->qText());

	_list->addItem(item);
	elaborateItem(item);

	if (_edit)
	{
		_edit->selectAll();
		_edit->setFocus();
		_edit->setFocusPolicy(Qt::StrongFocus);
	}
}

void Library::copyToClipboard()
{
	QListWidgetItem *item = _list->currentItem();
	
	if (item == NULL)
	{
		return;
	}

	ImageProc *proc = imageProcForItem(item);
	QImage *im = proc->getImage();
	
	if (im->isNull())
	{
		return;
	}
	
	QApplication::clipboard()->setImage(*im);
}

void Library::updatePaste()
{
	QImage image;
	getImageFromClipboard(&image);
	
	if (image.isNull())
	{
		std::cout << "No image" << std::endl;
		return;
	} 
	
	QListWidgetItem *item = _list->currentItem();
	ImageProc *proc = imageProcForItem(item);
	proc->setImage(image);
	elaborate();
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

	if (_addToPres != NULL)
	{
		_addToPres->hide();
		delete _addToPres; _addToPres = NULL;
	}

	if (_edit != NULL)
	{
		_edit->hide();
		delete _edit; _edit = NULL;
	}
}

void Library::mousePressEvent(QMouseEvent *event)
{
	if (!_seeding)
	{
		return;
	}

	double x = event->x();
	double y = event->y();

	if (x < _imageLabel->geometry().left() || 
	    x > _imageLabel->geometry().right())
	{
		return;
	}

	if (y < _imageLabel->geometry().top() || 
	    y > _imageLabel->geometry().bottom())
	{
		return;
	}
	
	x -= _imageLabel->geometry().left();
	y -= _imageLabel->geometry().top();
	
	QBrush brush(QColor(255, 255, 255, 255));
	QPen pen(QColor(255, 255, 255, 255));
	
	_graphics->addEllipse(x - 5, y - 5, 10, 10, pen, brush);
	
	ImageProc *proc = imageProcForItem(_list->currentItem());
	x *= proc->getImage()->width() / _imageLabel->width();
	y *= proc->getImage()->height() / _imageLabel->height();
	vec3 centre = make_vec3(x, y, 0);
	_points.push_back(centre);

	_bProcess->setText("Click when done");
}

void Library::addToPresentation()
{
	ImageProc *proc = imageProcForItem(_list->currentItem());
	ImageAppear *appear = new ImageAppear(_pres);
	appear->setNewImage(proc);
	_pres->addInstruction(appear);
	appear->instantEffect();
}

void Library::fadeToPresentation()
{
	ImageProc *proc = imageProcForItem(_list->currentItem());
	ImageAppear *appear = new ImageFade(_pres);
	appear->setNewImage(proc);
	_pres->addInstruction(appear);
	appear->instantEffect();
}

void Library::changeBackground()
{
	ImageProc *proc = imageProcForItem(_list->currentItem());
	ChangeBackground *change = new ChangeBackground(_pres, proc);
	_pres->addInstruction(change);
	change->instantEffect();
}

void Library::changeProcessing()
{
	if (_seeding == false)
	{
		_bProcess->setText("Click image");
		_points.clear();
		_seeding = true;
		setFocus();
		
		QBrush trans(Qt::transparent);
		
		_pointView = new QGraphicsView(this);
		_pointView->setGeometry(_imageLabel->geometry());
		_graphics = new QGraphicsScene(_imageLabel);
		_graphics->setSceneRect(_imageLabel->rect());
		_pointView->setBackgroundBrush(trans);
		_pointView->setStyleSheet("background-color: transparent;");
		_pointView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		_pointView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		_pointView->setScene(_graphics);
		_pointView->show();

		return;
	}
	else
	{
		delete _graphics;
		_graphics = NULL;
		_bProcess->setText("Change fade");

		ImageProc *proc = imageProcForItem(_list->currentItem());
		proc->prepareProcessArray();
		proc->setSeeds(_points);
		proc->preprocess(false);

		_seeding = false;
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
	
	float mywidth = im->width();
	float height = 300;
	if (im->height() > height)
	{
		mywidth = im->width() * 300 / im->height();
	}
	else
	{
		height = im->height();
	}
	
	/* if width is too long still */
	if (mywidth > (width() - LIST_WIDTH))
	{
		double ratio = (width() - LIST_WIDTH) / (double)mywidth;
		height *= ratio;
		mywidth *= ratio;
	}

	if (_imageLabel == NULL)
	{
		_imageLabel = new QLabel(this);
	}

	_imageLabel->setPixmap(QPixmap::fromImage(im->scaled(mywidth, height)));
	_imageLabel->setGeometry(ELABORATION_MIDPOINT - mywidth / 2, 
	                    MENU_HEIGHT + IMAGE_TITLE_HEIGHT,
	                    mywidth, height);
	_imageLabel->show();
	
	if (_edit == NULL)
	{
		_edit = new QLineEdit(this);
		connect(_edit, &QLineEdit::editingFinished, 
		        this, &Library::updateTitle);
	}

	_edit->setGeometry(LIST_WIDTH + 10, MENU_HEIGHT + 10,
	                   ELABORATION_WIDTH, IMAGE_TITLE_HEIGHT);
	_edit->setText(proc->qText());
	_edit->show();
	
	if (_addToPres == NULL)
	{
		_addToPres  = new QPushButton(this);
		connect(_addToPres, &QPushButton::clicked, 
		        this, &Library::addToPresentation);

		_addFade  = new QPushButton(this);
		connect(_addFade, &QPushButton::clicked, 
		        this, &Library::fadeToPresentation);

		_bDelete = new QPushButton(this);
		connect(_bDelete, &QPushButton::clicked, 
		        this, &Library::deleteFromLibrary);

		_bUpdate = new QPushButton(this);
		connect(_bUpdate, &QPushButton::clicked, 
		        this, &Library::updatePaste);

		_bCopy = new QPushButton(this);
		connect(_bCopy, &QPushButton::clicked, 
		        this, &Library::copyToClipboard);

		_bChange = new QPushButton(this);
		connect(_bChange, &QPushButton::clicked, 
		        this, &Library::changeBackground);

		_bProcess = new QPushButton(this);
		connect(_bProcess, &QPushButton::clicked, 
		        this, &Library::changeProcessing);
	}
	
	int y = MENU_HEIGHT + IMAGE_TITLE_HEIGHT + IMAGE_HEIGHT;

	y += BUTTON_HEIGHT * 0.2;
	
	_addToPres->setGeometry(ELABORATION_MIDPOINT - BUTTON_WIDTH * 1.2,
	                        y, BUTTON_WIDTH, BUTTON_HEIGHT);
	_addToPres->setText("Add to presentation");
	_addToPres->show();
	
	_addFade->setGeometry(ELABORATION_MIDPOINT + BUTTON_WIDTH * 0.2,
	                        y, BUTTON_WIDTH, BUTTON_HEIGHT);
	_addFade->setText("Add special fade");
	_addFade->show();
	
	y += BUTTON_HEIGHT * 1.2;
	
	_bDelete->setGeometry(ELABORATION_MIDPOINT - BUTTON_WIDTH / 2,
	                        y, BUTTON_WIDTH, BUTTON_HEIGHT);
	_bDelete->setText("Delete");
	_bDelete->show();
	
	y += BUTTON_HEIGHT * 1.2;
	
	_bCopy->setGeometry(ELABORATION_MIDPOINT - BUTTON_WIDTH / 2,
	                        y, BUTTON_WIDTH, BUTTON_HEIGHT);
	_bCopy->setText("Copy to clipboard");
	_bCopy->show();
	
	y += BUTTON_HEIGHT * 1.2;
	
	_bUpdate->setGeometry(ELABORATION_MIDPOINT - BUTTON_WIDTH / 2,
	                        y, BUTTON_WIDTH, BUTTON_HEIGHT);
	_bUpdate->setText("Update from clipboard");
	_bUpdate->show();
	
	y += BUTTON_HEIGHT * 1.2;
	
	_bChange->setGeometry(ELABORATION_MIDPOINT - BUTTON_WIDTH / 2,
	                        y, BUTTON_WIDTH, BUTTON_HEIGHT);
	_bChange->setText("Change background");
	_bChange->show();
	
	y += BUTTON_HEIGHT * 1.2;
	
	_bProcess->setGeometry(ELABORATION_MIDPOINT - BUTTON_WIDTH / 2,
	                        y, BUTTON_WIDTH, BUTTON_HEIGHT);
	_bProcess->setText("Change fade");
	_bProcess->show();
}

void Library::deleteFromLibrary()
{
	QListWidgetItem *item = _list->currentItem();
	ImageProc *proc = imageProcForItem(item);
	if (_pres->imageInUse(proc))
	{
		QMessageBox msgBox;
		msgBox.setText("The presentation uses this image.");
		msgBox.setInformativeText("Do you want to delete all instances?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		int ret = msgBox.exec();	

		if (ret == QMessageBox::No)
		{
			return;
		}
	}
	
	_pres->removeImageReferences(proc);
	_list->takeItem(_list->currentRow());
}

ImageProc *Library::imageProcForItem(QListWidgetItem *item)
{
	QVariant var = item->data(Qt::UserRole);
	ImageProc *proc = qvariant_cast<ImageProc *>(var);
	
	return proc;
}

void Library::updateTitle()
{
	ImageProc *proc = imageProcForItem(_list->currentItem());
	proc->setText(_edit->text().toStdString());
	_list->currentItem()->setText(proc->qText());
	_list->setFocus();
}

void Library::addProperties()
{
	for (int i = 0; i < _list->count(); i++)
	{
		QListWidgetItem *item = _list->item(i);
		ImageProc *proc = imageProcForItem(item);
		addChild("image_proc", proc);
	}
	
	addChild("presentation", _pres);
}

void Library::copyToLibrary(Library *another)
{
	for (int i = 0; i < _list->count(); i++)
	{
		QListWidgetItem *item = _list->item(i);
		Parser *proc = imageProcForItem(item);
		another->addObject(proc, "image_proc");
	}
	
	_pres->copyToGL(another->presentation());
}

void Library::addObject(Parser *child, std::string name)
{
	if (name == "image_proc")
	{
		QListWidgetItem *item = new QListWidgetItem();
		ImageProc *proc = static_cast<ImageProc *>(child);
		QVariant var = QVariant::fromValue<ImageProc *>(proc);

		item->setData(Qt::UserRole, var);
		item->setText(proc->qText());
		
		_list->addItem(item);
	}
	
	if (name == "presentation")
	{
		_pres = static_cast<BlotGL *>(child);
	}
}

Library::~Library()
{
	clearElaboration();
	
	for (int i = 0; i < _list->count(); i++)
	{
		QListWidgetItem *item = _list->item(i);
		ImageProc *proc = imageProcForItem(item);
		delete proc;
	}
	
	delete _list;
}
