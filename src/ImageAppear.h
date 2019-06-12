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

#ifndef __Blot__ImageAppear__
#define __Blot__ImageAppear__

#include "Instruction.h"
#include "BlotObject.h"

class BlotGL;

class ImageAppear : public Instruction
{
public:
	ImageAppear(BlotGL *pres);
	virtual ~ImageAppear() {};
	
	virtual bool canMove()
	{
		return true;
	}

	void setNewImage(ImageProc *proc);
	virtual void select(bool sel);
	virtual void moveFractional(double fx, double fy);;

	void setBlotObject(BlotObject *obj)
	{
		_obj = obj;
	}
	
	void setPosition(float t, float b, float l, float r)
	{
		_left = l;
		_top = t;
		_bottom = b;
		_right = r;
	}

	virtual void makeEffect();
	
	virtual BlotObject *object()
	{
		return _obj;
	}
private:
	BlotObject *_obj;
	
	float _left;
	float _right;
	float _bottom;
	float _top;

	int _advance;
};


#endif
