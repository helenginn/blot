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

#include "Instruction.h"
#include "Set.h"
#include "BlotGL.h"
#include "charmanip.h"

Instruction::Instruction(BlotGL *pres)
{
	_onClick = true;
	_presentation = pres;
	_random = i_to_str(rand());
	_delay = -1;

	setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | 
	         Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

Instruction::Instruction(Instruction &other)
{
	_onClick = other._onClick;
	_presentation = other._presentation;
	_random = i_to_str(rand());
	_delay = other._delay;

	setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | 
	         Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

void Instruction::addProperties()
{
	addDoubleProperty("delay", &_delay);
	addBoolProperty("on_click", &_onClick);
	addStringProperty("_random", &_random);
	addReference("presentation", _presentation);
}

void Instruction::linkReference(BaseParser *child, std::string name)
{
	if (name == "presentation")
	{
		_presentation = static_cast<BlotGL *>(child);
	}
}

void Instruction::updateText()
{
	setText(0, qText());
	QString d = "";
	
	if (hasDelay())
	{
		d = QString::number(delay());
	}
	
	setText(1, d);

	_presentation->list()->repaint();
}

bool Instruction::animateEffect()
{
	instantEffect();
	return false;
}

void Instruction::postParseTidy()
{
	updateText();
}

Set *Instruction::instructionParent()
{
	if (isTopLevel())
	{
		return _presentation;
	}
	else
	{
		return dynamic_cast<Set *>(QTreeWidgetItem::parent());	
	}
}

Instruction *Instruction::displayableInstruction()
{
	return this;
}

Instruction *Instruction::prevInstruction()
{
	return adjacentInstruction(-1);
}

Instruction *Instruction::nextInstruction()
{
	return adjacentInstruction(+1);
}

Instruction *Instruction::adjacentInstruction(int dir)
{
	Set *parent = instructionParent();
	if (parent == NULL)
	{
		return NULL;
	}
	
	int idx = parent->indexOfInstruction(this);
	idx += (dir > 0) ? 1 : -1;

	if (idx < 0 || parent->instructionCount() <= idx)
	{
		return parent->adjacentInstruction(dir);
	}
	else
	{
		Instruction *inst = parent->instruction(idx);
		Set *set = dynamic_cast<Set *>(inst);

		if (set == NULL || set->instructionCount() == 0)
		{
			return inst;
		}
		
		if (dir > 0)
		{
			return set->instruction(0);
		}
		else
		{
			Instruction *candidate = set->lastInstruction();
			if (candidate == NULL)
			{
				return set;
			}

			return candidate;
		}
	}
}

void Instruction::setData(int column, int role, const QVariant &value)
{
	if (role == Qt::EditRole && column == 0)
	{
		return;
	}

	if (role == Qt::EditRole)
	{
		_delay = value.toDouble();
	}
	
	QTreeWidgetItem::setData(column, role, value);
}
