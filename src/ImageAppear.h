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

#include "ImageAnimated.h"
#include "BlotObject.h"

class BlotGL;

class ImageAppear : public ImageAnimated
{
public:
	ImageAppear(BlotGL *pres = NULL);
	ImageAppear(ImageAppear &other);
	virtual ~ImageAppear() {};
	
	virtual bool canMove()
	{
		return true;
	}

	void setNewImage(ImageProc *proc);
	virtual void select(bool sel);
	virtual void moveFractional(double fx, double fy);
	virtual void resizeFractional(double fx, double fy, bool aspect);
	virtual void rotateFractional(float x0, float y0, float fx, float fy);
	virtual void rotationalTranslate(float f, float mx, float my);
	virtual void position(double *x, double *y);
	
	void setPosition(float t, float b, float l, float r)
	{
		_left = l;
		_top = t;
		_bottom = b;
		_right = r;
	}
	
	void getPosition(float *t, float *b, float *l, float *r)
	{
		*t = _top;
		*l = _left;
		*r = _right;
		*b = _bottom;
	}

	virtual bool animateStep();
	virtual void instantEffect();
	virtual void prepareEffect();
	
	virtual std::string getClassName()
	{
		return "ImageAppear";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "ImageAppear_" + _random;
	}

	virtual std::string instText();

	virtual void addProperties();
	virtual void linkReference(BaseParser *child, std::string name);
protected:

private:
	double _left;
	double _right;
	double _bottom;
	double _top;
	double _angle;
};


#endif
