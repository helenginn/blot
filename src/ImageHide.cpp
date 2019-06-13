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

#include "ImageHide.h"
#include "BlotObject.h"

ImageHide::ImageHide(BlotGL *pres, Instruction *inst) : Instruction(pres)
{
	if (inst->object() == NULL)
	{
		_valid = false;
		return;
	}

	_startTime = 1;
	_endTime = 0;
	_stepTime *= -1;
	_obj = inst->object();
	_fade = true;
}

bool ImageHide::animateEffect()
{
	if (!_fade)
	{
		std::cout << "Instant " << " " << _obj->isDisabled() << std::endl;
		makeEffect();
		setTime(1);
		return false;
	}

	setTime(_startTime);
	std::cout << "Fade " << " " << _obj->isDisabled() << std::endl;
	return true;
}

bool ImageHide::animateStep()
{
	double newTime = _time + _stepTime;
	bool keep_going = true;
	
	if (newTime < _endTime)
	{
		keep_going = false;
		newTime = _endTime;
		makeEffect();
	}

	setTime(newTime);

	return keep_going;
}


void ImageHide::makeEffect()
{
	std::string text = _obj->getImage()->text();
	std::cout << "Hide effect for " << text << std::endl;
	_obj->setDisabled(true);

	_presentation->update();
}

void ImageHide::addProperties()
{
	Instruction::addProperties();
	
	addReference("blot_object", _obj);
	addBoolProperty("fade", &_fade);
}

void ImageHide::linkReference(BaseParser *child, std::string name)
{
	if (name == "blot_object")
	{
		_obj = static_cast<BlotObject *>(child);
	}
	
	Instruction::linkReference(child, name);
}

std::string ImageHide::instText()
{
	std::string start = "";
	start += (waitForClick() ? "" : "+ ");
	start += "Hide " + object()->getImage()->text();
	return start;
}

void ImageHide::setTime(double time)
{
	_time = time;
	_obj->setTime(time);
}
