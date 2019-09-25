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

#include "EditGroup.h"
#include "BlotObject.h"

EditGroup::EditGroup(Instruction *primary, std::vector<Instruction *> other)
{
	_instructions = other;
	_instructions.push_back(primary);
	_onlyMove = true;
	
	for (size_t i = 0; i < _instructions.size(); i++)
	{
		if (_instructions[i]->getClassName() != "ImageMove")
		{
			_onlyMove = false;
			break;
		}
	}
}

void EditGroup::findMidpoints()
{
	double x, y;
	double sum_x = 0;
	double sum_y = 0;
	double count = 0;

	for (size_t i = 0; i < _instructions.size(); i++)
	{
		if (!movableObject(i))
		{
			continue;
		}
		
		_instructions[i]->position(&x, &y);
		
		sum_x += x;
		sum_y += y;
		count++;
	}

	sum_x /= count;
	sum_y /= count;

	_lrMiddle = sum_x;
	_tbMiddle = sum_y;
}

bool EditGroup::movableObject(int i)
{
	if (_instructions[i]->object() == NULL)
	{
		return false;
	}

	if (!_onlyMove && _instructions[i]->getClassName() == "ImageMove")
	{
		return false;
	}

	return true;
}

void EditGroup::moveFractional(double fx, double fy)
{
	for (size_t i = 0; i < _instructions.size(); i++)
	{
		if (!movableObject(i))
		{
			continue;
		}
		
		_instructions[i]->moveFractional(fx, fy);
	}
}

void EditGroup::rotateFractional(float x0, float y0, float fx, float fy)
{
	findMidpoints();

	for (size_t i = 0; i < _instructions.size(); i++)
	{
		if (!movableObject(i))
		{
			continue;
		}
		
		_instructions[i]->rotateFractional(x0, y0, fx, fy);
		_instructions[i]->rotationalTranslate(fx, _lrMiddle, _tbMiddle);
	}

}


