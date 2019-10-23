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

ImageHide::ImageHide(BlotGL *pres, Instruction *inst) : ImageAnimated(pres)
{
	if (inst == NULL || inst->object() == NULL)
	{
		_valid = false;
		return;
	}

	_valid = true;
	_startTime = _endTime;
	_endTime = -0.1;
	_stepTime *= -1;
	_obj = inst->object();
	_fade = true;
}

bool ImageHide::animateStep()
{
	bool keep_going = incrementTime();
	_obj->setTime(_time);
	return keep_going;
}

void ImageHide::instantEffect()
{
	prepareEffect();
	_obj->setTime(_endTime);

	_presentation->update();
}

void ImageHide::prepareEffect()
{
	std::string text = _obj->getImage()->text();
	std::cout << "Hide effect for " << text << std::endl;

}

void ImageHide::addProperties()
{
	ImageAnimated::addProperties();
	
	addReference("blot_object", _obj);
	addBoolProperty("fade", &_fade);
}

void ImageHide::linkReference(BaseParser *child, std::string name)
{
	ImageAnimated::linkReference(child, name);
}

std::string ImageHide::instText()
{
	std::string start = "";
	start += (waitForClick() ? "" : "+ ");
	start += "Hide " + object()->getImage()->text();
	return start;
}
