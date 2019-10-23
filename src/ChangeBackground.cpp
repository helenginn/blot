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

#include "ChangeBackground.h"
#include "BlotGL.h"
#include "BlotObject.h"
#include "ImageProc.h"

ChangeBackground::ChangeBackground(BlotGL *pres, ImageProc *proc)
: Instruction(pres)
{
	if (proc)
	{
		_obj = new BlotObject(proc);
		_obj->ignoreAspectRatio();
	}
}

void ChangeBackground::instantEffect()
{
	_obj->setDisabled(false);
	_obj->setVertices(1, -1, -1, 1);
	_obj->setZCoord(-0.99);
	std::cout << "Changing background" << std::endl;

	_presentation->update();
}

std::string ChangeBackground::instText()
{
	std::string start = "";
	start += (waitForClick() ? "" : "+ ");
	start += "Background to " + object()->getImage()->text();
	return start;
}

void ChangeBackground::addProperties()
{
	Instruction::addProperties();
	
	addReference("blot_object", _obj);
}

void ChangeBackground::linkReference(BaseParser *child, std::string name)
{
	if (name == "blot_object")
	{
		_obj = static_cast<BlotObject *>(child);
	}
	
	Instruction::linkReference(child, name);
}

