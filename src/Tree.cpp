// header
// Copyright (C) 2019 Helen Ginn
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

#include "Tree.h"
#include "BlotGL.h"
#include "ImageMove.h"
#include "ImageHide.h"
#include "ImageAppear.h"
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>

Tree::Tree(QWidget *p, BlotGL *pres) : QTreeWidget(p)
{
	_pres = pres;

	setDragEnabled(true);
	setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
	setSelectionMode(QAbstractItemView::ContiguousSelection);
	setColumnCount(3);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QTreeWidget::customContextMenuRequested,
	        this, &Tree::rightClickMenu);

	
	QStringList labels;
	labels.push_back("Instruction");
	labels.push_back("delay");
	labels.push_back("speed");
	setHeaderLabels(labels);
	
	header()->resizeSection(0, 250);
}

Qt::DropActions Tree::supportedDropActions() const
{
	return Qt::MoveAction;
}

Qt::ItemFlags Tree::getTreeItemFlags() const
{
    return (Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
}

void Tree::rightClickMenu(const QPoint &p)
{
	std::cout << "here" << std::endl;
	QMenu *m = new QMenu();
	QPoint pos = mapToGlobal(p);
	
	QAction *a = m->addAction("Set delay");
	connect(a, &QAction::triggered, this, &Tree::setDelay);
	a = m->addAction("Move");
	connect(a, &QAction::triggered, this, &Tree::makeMoves);
	a = m->addAction("Hide");
	connect(a, &QAction::triggered, this, &Tree::makeHides);
	a = m->addAction("Duplicate");
	connect(a, &QAction::triggered, this, &Tree::duplicate);
	a = m->addAction("Make set");
	connect(a, &QAction::triggered, _pres, &BlotGL::makeSet);
	a = m->addAction("Select");
	connect(a, &QAction::triggered, this, &Tree::select);

	m->exec(pos);
}

void Tree::select()
{
	QList<QTreeWidgetItem *> selected = selectedItems();
	_pres->deselectAll();

	for (size_t i = 0; i < selected.size(); i++)
	{
		Instruction *inst = dynamic_cast<Instruction *>(selected[i]);
		_pres->selectInstruction(inst, false);
	}

}

void Tree::setDelay()
{
	bool ok;
	double d;
	d = QInputDialog::getDouble(this, tr("Delay after previous instruction"),
	                            tr("Set value:"), 0, 0, 100, 1, &ok);

	if (ok)
	{
		QList<QTreeWidgetItem *> selected = selectedItems();
		for (size_t i = 0; i < selected.size(); i++)
		{
			Instruction *inst = dynamic_cast<Instruction *>(selected[i]);
			if (inst)
			{
				inst->setDelay(d);
			}
		}
	}
}

void Tree::duplicate()
{
	QList<QTreeWidgetItem *> selected = selectedItems();
	QList<QTreeWidgetItem *> replacement;
	int count = 0;

	for (size_t i = 0; i < selected.size(); i++)
	{
		ImageAppear *inst = dynamic_cast<ImageAppear *>(selected[i]);
		
		if (inst)
		{
			ImageAppear *dupl = new ImageAppear(*inst);
			_pres->addInstruction(dupl);
			replacement.push_back(dupl);
		}
	}
	
	if (replacement.size() >= 5)
	{
		_pres->addSet(replacement);
	}
	
	_pres->deselectAll();
}

void Tree::makeHides()
{
	QList<QTreeWidgetItem *> selected = selectedItems();
	QList<QTreeWidgetItem *> replacement;

	for (size_t i = 0; i < selected.size(); i++)
	{
		Instruction *inst = dynamic_cast<Instruction *>(selected[i]);
		ImageHide *hide = new ImageHide(_pres, inst);
		replacement.push_back(hide);
		_pres->addInstruction(hide);
	}

	if (replacement.size() >= 5)
	{
		_pres->addSet(replacement);
	}
}

void Tree::makeMoves()
{
	bool ok;
	double d;
	d = QInputDialog::getDouble(this, tr("Percentage of default motion"),
	                            tr("Set value:"), 0, 0, 1, 2, &ok);

	QList<QTreeWidgetItem *> selected = selectedItems();
	QList<QTreeWidgetItem *> replacement;

	for (size_t i = 0; i < selected.size(); i++)
	{
		Instruction *inst = dynamic_cast<Instruction *>(selected[i]);
		ImageMove *move = new ImageMove(_pres, inst, d);
		replacement.push_back(move);
		_pres->addInstruction(move);
	}
	
	if (replacement.size() >= 5)
	{
		_pres->addSet(replacement);
	}
}
