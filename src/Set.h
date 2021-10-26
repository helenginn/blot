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

#ifndef __blot__Set__
#define __blot__Set__

#include "Instruction.h"

class BlotGL;

class Set : public Instruction, virtual public QTreeWidgetItem
{
public:
	Set(BlotGL *pres = NULL, Set *parent = NULL);
	
	void setInstructions(QList<QTreeWidgetItem *> insts);

	virtual Instruction *instruction(int i);
	virtual int instructionCount();
	virtual int indexOfInstruction(Instruction *inst);
	virtual Instruction *takeInstruction(Instruction *inst);
	virtual void insertInstruction(Instruction *inst, int idx);
	virtual Instruction *displayableInstruction();
	Instruction *deleteInstruction(Instruction *inst);
	Instruction *lastInstruction();

	virtual std::string getClassName()
	{
		return "InstructionSet";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "InstructionSet_" + _random;
	}

	virtual void instantEffect();
	
	virtual std::string instText()
	{
		return _text;
	}

	virtual void addProperties();
	virtual void addObject(Parser *child, std::string name);
	virtual void updateText();

	virtual void setData(int column, int role, const QVariant &value);

	virtual void displayToInstruction(Instruction *inst);
	virtual void addInstruction(Instruction *inst, bool atRow = true);
	void addInstruction(Instruction *inst, int idx);
	virtual void addSet(QList<QTreeWidgetItem *> list = 
	                    QList<QTreeWidgetItem *>());
protected:
	int _currPos;
	Instruction *_currInstruct;
private:

	std::string _text;

};

#endif
