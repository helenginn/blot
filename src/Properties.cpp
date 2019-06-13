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

#include "Properties.h"
#include "Instruction.h"
#include <QLabel>
#include <QCheckBox>


Properties::Properties(Instruction *inst) : QMainWindow(NULL)
{
	setWindowTitle("Instruction properties");

	if (inst == NULL)
	{
		return;
	}

	_inst = inst;

	const int label_x = 80;
	const int tick_x = 40;
	
	int y = 20;
	
	QLabel *label = new QLabel(this);
	label->setGeometry(label_x, y, PROPERTY_LABEL_WIDTH, 
	                   PROPERTY_LABEL_HEIGHT);
	label->setText(inst->qText());
	label->show();
	
	y += PROPERTY_LABEL_HEIGHT;
	
	QCheckBox *check = new QCheckBox(this);
	check->setGeometry(tick_x, y, CHECK_BOX_DIMENSION,
	                   CHECK_BOX_DIMENSION);
	check->setChecked(inst->waitForClick());

	connect(check, &QCheckBox::stateChanged, 
	        this, &Properties::changeClickRequirement);

	check->show();
	_toDelete.push_back(check);
	_onClickCheck = check;
	
	label = new QLabel(this);
	label->setText("Wait for click first");
	label->setGeometry(label_x, y, PROPERTY_LABEL_WIDTH,
	                   PROPERTY_LABEL_HEIGHT);
	_toDelete.push_back(label);
	label->show();
	
	y += PROPERTY_LABEL_HEIGHT;
}

void Properties::changeClickRequirement()
{
	bool click = _onClickCheck->isChecked();
	_inst->setOnClick(click); 
}
