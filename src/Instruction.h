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

#ifndef __Blot__Instruction__
#define __Blot__Instruction__

#include "BlotGL.h"

class Instruction
{
public:
	Instruction(BlotGL *pres);
	virtual ~Instruction() {};
	
	virtual bool canMove()
	{
		return false;
	}
	
	virtual void moveFractional(double fx, double fy) {};
	
	void setOnClick(bool click)
	{
		_onClick = click;
	}
	
	bool waitForClick()
	{
		return _onClick;
	}

	virtual BlotObject *object()
	{
		return NULL;
	}
	
	virtual void makeEffect() = 0;
	
	virtual void select(bool sel) {};
protected:
	BlotGL *_presentation;

private:
	bool _onClick;

};


#endif
