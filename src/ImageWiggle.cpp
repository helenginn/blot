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

#include "ImageWiggle.h"
#include "Instruction.h"
#include "src/maths.h"

ImageWiggle::ImageWiggle(BlotGL *gl, Instruction *inst) : 
ImageAnimated(gl, inst)
{
	_angle = 1;
	_stepTime = 0.01;
}

void ImageWiggle::addProperties()
{
	ImageAnimated::addProperties();
	
	addDoubleProperty("angle", &_angle);
}

void ImageWiggle::linkReference(BaseParser *child, std::string name)
{
	ImageAnimated::linkReference(child, name);
}

void ImageWiggle::instantEffect()
{

}

void ImageWiggle::prepareEffect()
{
	std::string text = _obj->getImage()->text();
	std::cout << "Wiggle effect for " << text << std::endl;
}

std::string ImageWiggle::instText()
{
	std::string start = "";
	start += (waitForClick() ? "" : "+ ");
	start += "Wiggle " + object()->getImage()->text();
	return start;
}

bool ImageWiggle::animateStep()
{
	double newTime = _time + _stepTime;
	bool keep_going = true;
	
	if (newTime > _endTime)
	{
		keep_going = false;
		newTime = _endTime;
	}

	setTime(newTime);
	
	/* portion between 0 and 1 */
	double portion = (_endTime - _time) / (_endTime - _startTime);
	double step = (_stepTime) / (_endTime - _startTime);

	double cycle = fmod(portion, 0.2);
	double dir = cycle < 0.1 ? -1 : 1;
	
	std::cout << _startTime << " " << _endTime << " " << _stepTime << 
	std::endl;
	std::cout << _angle << " " << cycle << " " << " " << portion << " " << dir << std::endl;
	
	double diffa = _angle * step * dir;

	_obj->rotateVertices(diffa);

	return keep_going;
}
