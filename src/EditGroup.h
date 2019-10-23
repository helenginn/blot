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

#ifndef __blot__EditGroup__
#define __blot__EditGroup__

#include "Instruction.h"

class EditGroup
{
public:
	EditGroup(Instruction *primary, std::vector<Instruction *> other);

	void moveFractional(double fx, double fy);
	void rotateFractional(float x0, float y0, float fx, float fy);
private:
	void findMidpoints();
	bool movableObject(int i);

	std::vector<Instruction *> _instructions;

	double _lrMiddle;
	double _tbMiddle;
	
	bool _onlyMove;

};

#endif
