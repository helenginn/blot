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

#ifndef __Blot__ImageMove__
#define __Blot__ImageMove__

#include "ImageAnimated.h"

class BlotGL;
class BlotObject;

class ImageMove : public ImageAnimated
{
public:
	ImageMove(BlotGL *pres = NULL, Instruction *inst = NULL,
	          double fraction = 1);

	bool isValid()
	{
		return _valid;
	}
	
	bool canMove()
	{
		return true;
	}
	
	virtual std::string getClassName()
	{
		return "ImageMove";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "ImageMove_" + _random;
	}

	virtual void select(bool sel);
	virtual void moveFractional(double fx, double fy);
	virtual void position(double *x, double *y);
	virtual void rotationalTranslate(float f, float mx, float my);
	virtual void rotateFractional(float x0, float y0, float fx, float fy);
	virtual void resizeFractional(double fx, double fy, bool aspect);
	virtual std::string instText();
	virtual void instantEffect();
	virtual void prepareEffect();
	virtual bool animateStep();
protected:
	virtual void addProperties();
	virtual void linkReference(BaseParser *child, std::string name);
private:
	double _oldx, _oldy;
	double _newx, _newy;
	double _resize;
	double _angle;
	Instruction *_master;

};

#endif
