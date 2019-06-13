// Bond
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

#include "ImageAppear.h"

ImageAppear::ImageAppear(BlotGL *pres) : Instruction(pres)
{
	_advance = 10;
	_left = -0.3;
	_right = 0.3;
	_top = 0.3;
	_bottom = -0.3;
	_fade = true;
}

bool ImageAppear::animateEffect()
{
	if (!_fade)
	{
		makeEffect();
		setTime(1);
		return false;
	}

	setTime(_startTime);
	makeEffect();
	return true;
}

bool ImageAppear::animateStep()
{
	double newTime = _time + _stepTime;
	bool keep_going = true;
	
	if (newTime > _endTime)
	{
		keep_going = false;
		newTime = _endTime;
	}

	setTime(newTime);

	return keep_going;
}

void ImageAppear::setTime(double time)
{
	_time = time;
	_obj->setTime(time);
}

void ImageAppear::makeEffect()
{
	std::string text = _obj->getImage()->text();
	std::cout << "Make effect for " << text << std::endl;
	_obj->setDisabled(false);
	_obj->setVertices(_top, _bottom, _left, _right);

	_presentation->update();
}

void ImageAppear::addProperties()
{
	Instruction::addProperties();
	
	addReference("blot_object", _obj);
	addDoubleProperty("left", &_left);
	addDoubleProperty("right", &_right);
	addDoubleProperty("top", &_top);
	addDoubleProperty("bottom", &_bottom);
}

void ImageAppear::linkReference(BaseParser *child, std::string name)
{
	if (name == "blot_object")
	{
		_obj = static_cast<BlotObject *>(child);
	}
	
	Instruction::linkReference(child, name);
}

void ImageAppear::setNewImage(ImageProc *proc)
{
	_obj = new BlotObject(proc);
	
	QImage *im = proc->getImage();
	int width = im->width();
	int height = im->height();

	double ratio = height / (double)width;
	_left = _bottom * ratio;
	_right = _top * ratio;
}

void ImageAppear::moveFractional(double fx, double fy)
{
	_left += fx;
	_right += fx;
	_top += fy;
	_bottom += fy;

	_obj->setVertices(_top, _bottom, _left, _right);
}

void ImageAppear::resizeFractional(double fx, double fy, bool aspect)
{
	double ratio = (_bottom - _top) / (_right - _left);

	_left -= fx;
	_right += fx;
	_top += fy;
	_bottom -= fy;
	
	if (aspect)
	{
		double new_ratio = (_bottom - _top) / (_right - _left);
		double fix = ratio / new_ratio;
		double diff = (_bottom - _top) * (fix - 1) / 2;
		
		_bottom += diff;
		_top -= diff;
	}

	_obj->setVertices(_top, _bottom, _left, _right);
}

bool ImageAppear::isCovered(double x, double y)
{
	if (_obj->isDisabled())
	{
		return false;
	}

	if (y < _bottom || y > _top)
	{
		return false;
	}

	if (x < _left || x > _right)
	{
		return false;
	}

	return true;
}

void ImageAppear::select(bool sel)
{
	if (!_obj)
	{
		return;
	}
	
	_obj->select(sel);
	_presentation->update();
}

std::string ImageAppear::instText()
{
	std::string start = "";
	start += (waitForClick() ? "" : "+ ");
	start += "Show " + object()->getImage()->text();
	return start;
}

