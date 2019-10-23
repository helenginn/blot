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

#include "BlotObject.h"
#include "BlotGL.h"
#include "ImageMove.h"

ImageMove::ImageMove(BlotGL *pres, Instruction *inst) 
: ImageAnimated(pres, inst)
{
	if (inst == NULL || inst->object() == NULL)
	{
		_valid = false;
		return;
	}

	_valid = true;
	_obj = inst->object();
	_fade = true;
	_endTime = 3;
	_angle = 0;
	_obj->midpoint(&_newx, &_newy);
	_obj->midpoint(&_oldx, &_oldy);
	_newx += 0.05;
	_newy += 0.05;
}

bool ImageMove::isCovered(double x, double y)
{
	return _obj->isCovered(x, y);
}

bool ImageMove::animateEffect()
{
	if (!_fade)
	{
		makeEffect();
		setTime(1);
		return false;
	}

	setTime(_startTime);
	return true;
}

bool ImageMove::animateStep()
{
	double newTime = _time + _stepTime;
	bool keep_going = true;
	
	if (newTime > _endTime)
	{
		keep_going = false;
		newTime = _endTime;
	}

	setTime(newTime);
	
	double portion = _stepTime / (_endTime - _startTime);
	
	double diffx = (_newx - _oldx) * portion;
	double diffy = (_newy - _oldy) * portion;
	double diffa = _angle * portion;

	_obj->addToVertices(diffx, diffy);
	_obj->rotateVertices(diffa);

	return keep_going;
}

void ImageMove::select(bool sel)
{
	if (!_obj)
	{
		return;
	}
	
	_obj->select(sel, 0.5, 0.0, 0.0);
	_presentation->update();
}

void ImageMove::moveFractional(double fx, double fy)
{
	_newx += fy;
	_newy += fx;

	_obj->addToVertices(fy, fx);
}

void ImageMove::rotateFractional(float x0, float y0, float fx, float fy)
{
	_angle += fy;
	_obj->rotateVertices(fy);
}

void ImageMove::makeEffect()
{
	std::string text = _obj->getImage()->text();
	std::cout << "Move effect for " << text << std::endl;
	_obj->addToVertices(_newx - _oldx, _newy - _oldy);
	_obj->rotateVertices(_angle);
	_presentation->update();
}

void ImageMove::addProperties()
{
	ImageAnimated::addProperties();
	
	addBoolProperty("fade", &_fade);
	addDoubleProperty("angle", &_angle);
	addDoubleProperty("oldx", &_oldx);
	addDoubleProperty("oldy", &_oldy);
	addDoubleProperty("newx", &_newx);
	addDoubleProperty("newy", &_newy);
}

void ImageMove::linkReference(BaseParser *child, std::string name)
{
	ImageAnimated::linkReference(child, name);
}

std::string ImageMove::instText()
{
	std::string start = "";
	start += (waitForClick() ? "" : "+ ");
	start += "Move " + object()->getImage()->text();
	return start;
}
