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

#include "ImageAnimated.h"
#include "BlotObject.h"

ImageAnimated::ImageAnimated(BlotGL *gl, Instruction *inst) : Instruction(gl)
{
	_valid = true;
	_startTime = 0;
	_endTime = 3;
	_time = _endTime;
	_fade = true;
	_stepTime = 0.02;

	if (inst == NULL || inst->object() == NULL)
	{
		_valid = false;
	}
	else
	{
		_obj = inst->object();
	}
}

void ImageAnimated::linkReference(BaseParser *child, std::string name)
{
	if (name == "blot_object")
	{
		_obj = static_cast<BlotObject *>(child);
	}
	
	Instruction::linkReference(child, name);
}

void ImageAnimated::addProperties()
{
	Instruction::addProperties();
	
	addBoolProperty("fade", &_fade);
	addDoubleProperty("end_time", &_endTime);
	addReference("blot_object", _obj);
}

bool ImageAnimated::animateEffect()
{
	if (!_fade)
	{
		instantEffect();
		setTime(1);
		return false;
	}

	prepareEffect();
	setTime(_startTime);
	return true;
}

bool ImageAnimated::isCovered(double x, double y)
{
	return _obj->isCovered(x, y);
}

bool ImageAnimated::incrementTime()
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

void ImageAnimated::setTime(double time)
{
	_time = time;
}
