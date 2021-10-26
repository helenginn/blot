// Blot
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

#include "Set.h"
#include "BlotGL.h"
#include "BlotObject.h"
#include "StartScreen.h"

Set::Set(BlotGL *pres, Set *parent) : QTreeWidgetItem(parent), Instruction(pres)
{
	_text = "New instruction set";

	setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable 
	         | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled);
	_currPos = 0;
}

void Set::setInstructions(QList<QTreeWidgetItem *> insts)
{
	QTreeWidgetItem::addChildren(insts);
	setExpanded(true);

	updateText();
}

void Set::addProperties()
{
	for (int i = 0; i < childCount(); i++)
	{
		QTreeWidgetItem *item = child(i);
		Instruction *inst = dynamic_cast<Instruction *>(item);
		BaseParser::addChild("instruction", inst);
	}
	
	addStringProperty("name", &_text);
	Instruction::addProperties();
}

void Set::addObject(Parser *child, std::string name)
{
	if (name == "instruction")
	{
		Instruction *inst = dynamic_cast<Instruction *>(child);
		QTreeWidgetItem::addChild(inst);
	}
}

void Set::setData(int column, int role, const QVariant &value)
{
	if (role == Qt::EditRole)
	{
		_text = value.toString().toStdString();
	}
	
	if (_text.length() == 0 && (int)value.type() == (int)QMetaType::QString)
	{
		_text = "New instruction set";
	}

	QTreeWidgetItem::setData(column, role, QString::fromStdString(_text));
}

void Set::updateText()
{
	setText(0, QString::fromStdString(_text));
}

void Set::instantEffect()
{
	for (size_t i = 0; i < instructionCount(); i++)
	{
//		instruction(i)->instantEffect();
	}
}

int Set::instructionCount()
{
	return childCount();
}

Instruction *Set::lastInstruction()
{
	if (instructionCount() == 0)
	{
		return NULL;
	}

	return instruction(instructionCount() - 1);
}

Instruction *Set::instruction(int i)
{
	QTreeWidgetItem *item = child(i);
	Instruction *inst = dynamic_cast<Instruction *>(item);
	return inst;
}

int Set::indexOfInstruction(Instruction *inst)
{
	int row = indexOfChild(inst);
	return row;
}

void Set::displayToInstruction(Instruction *inst)
{
	int row = indexOfInstruction(inst);
	int end = row;

	bool isSlate = (inst->getClassName() == "WipeSlate");
	
	if (row < 0)
	{
		row = instructionCount() - 1;
		end = row + 1;
		inst = instruction(row - 1);
	}

	StartScreen::startScreenPtr->setClick(inst);

	int lastWipe = row;

	if (!isSlate)
	{
		lastWipe = -1;

		for (int i = end - 1; i >= 0; i--)
		{
			Instruction *trial = instruction(i);

			if (trial->getClassName() == "WipeSlate")
			{
				lastWipe = i;
				break;
			}
			else if (trial->getClassName() == "InstructionSet")
			{
				Set *set = dynamic_cast<Set *>(trial);
				set->displayToInstruction(inst);
				
				lastWipe = i;
				break;
			}
		}
	}
	
	if (lastWipe < 0)
	{
		Set *parent = instructionParent();
		if (parent)
		{
			parent->displayToInstruction(this);
			lastWipe = 0;
		}
		else
		{
			lastWipe = 0;
		}
	}

	for (int i = lastWipe; i <= row; i++)
	{
		Instruction *inst = instruction(i);
		inst->instantEffect();
		std::cout << instText() << " displaying ";
		std::cout << inst->instText() << " (" << i << ")" << std::endl;
	}

	StartScreen::startScreenPtr->setClick(inst);
}

void Set::addInstruction(Instruction *inst, int idx)
{
	insertInstruction(inst, idx);
	
	_presentation->selectInstruction(inst);
	bool atTop = false;

	if (inst->primaryLoad())
	{
		atTop = true;
	}

	inst->updateText();
	_presentation->BlotGL::addObject(inst->object(), atTop);

}

void Set::addInstruction(Instruction *inst, bool atRow)
{
	int idx = 0;
	if (atRow)
	{
		Instruction *curr = _presentation->currentInstruction();
		int cidx = indexOfInstruction(curr);
		idx = cidx + 1;
	}

	addInstruction(inst, idx);
}

void Set::insertInstruction(Instruction *inst, int idx)
{
	insertChild(idx, inst);
}

Instruction *Set::takeInstruction(Instruction *inst)
{
	int idx = indexOfInstruction(inst);
	if (idx >= 0)
	{
		return dynamic_cast<Instruction *>(takeChild(idx));
	}
	return NULL;
}

Instruction *Set::deleteInstruction(Instruction *inst)
{
	int idx = indexOfInstruction(inst);
	takeInstruction(inst);
	
	if (idx == 0)
	{
		return this;
	}
	
	return instruction(idx);
}

Instruction *Set::displayableInstruction()
{
	if (instructionCount() == 0)
	{
		return this;
	}

	return instruction(0);
}

void Set::addSet(QList<QTreeWidgetItem *> list)
{
	if (list.size() == 0)
	{
		_presentation->list()->selectedItems();
	}

	if (list.size() == 0)
	{
		return;
	}
	
	int min = instructionCount() - 1;
	for (int i = 0; i < list.size(); i++)
	{
		Instruction *inst = dynamic_cast<Instruction *>(list[i]);
		
		int idx = indexOfInstruction(inst);
		if (idx < min)
		{
			min = idx;
		}

		inst->instructionParent()->takeInstruction(inst);
	}

	Set *set = new Set(_presentation, NULL);
	addInstruction(set, min);
	set->setInstructions(list);
	_presentation->list()->setCurrentItem(set);
}
