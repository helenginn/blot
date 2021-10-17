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
#include "BlotGL.h"
#include "charmanip.h"

Instruction::Instruction(BlotGL *pres)
{
	_onClick = true;
	_presentation = pres;
	_random = i_to_str(rand());
}

void Instruction::addProperties()
{
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
	treeWidget()->repaint();

}

bool Instruction::animateEffect()
{
	instantEffect();
	return false;
}
