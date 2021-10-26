// blot
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

#ifndef __blot__Tree__
#define __blot__Tree__

#include <QTreeWidget>
#include <QAbstractItemView>

class BlotGL;

class Tree : public QTreeWidget
{
Q_OBJECT
public:
	Tree(QWidget *p, BlotGL *pres);

protected:
	virtual Qt::DropActions supportedDropActions() const;
	virtual Qt::ItemFlags getTreeItemFlags() const;

public slots:
	void rightClickMenu(const QPoint &p);
	void setDelay();
	void makeMoves();
	void makeHides();
	void duplicate();

private:
	BlotGL *_pres;

};

#endif
