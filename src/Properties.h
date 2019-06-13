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

#ifndef __Blot__Properties__
#define __Blot__Properties__

#include <QMainWindow>

#define PROPERTIES_DEFAULT_WIDTH 350
#define PROPERTIES_DEFAULT_HEIGHT 600

#define PROPERTY_LABEL_WIDTH 250
#define PROPERTY_LABEL_HEIGHT 30

#define CHECK_BOX_DIMENSION 30 

class Instruction;
class QCheckBox;

class Properties : public QMainWindow
{
	Q_OBJECT
public:
	Properties(Instruction *inst = NULL);
	virtual ~Properties() {};
	
public slots:
	void changeClickRequirement();
	
private:
	Instruction *_inst;
	QCheckBox *_onClickCheck;
	std::vector<QObject *> _toDelete;
};

#endif
