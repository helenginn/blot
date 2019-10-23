// Blot
// Copyright (C) 2017-2019 Helen Ginn
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

#include "ImageFade.h"

ImageFade::ImageFade(BlotGL *pres) : ImageAppear(pres)
{
	_valid = true;
	_endTime = 3;
	_stepTime = 0.05;
}

std::string ImageFade::instText()
{
	std::string start = "";
	start += (waitForClick() ? "" : "+ ");
	start += "Fade " + object()->getImage()->text();
	return start;
}

bool ImageFade::animateStep()
{
	bool keep_going = incrementTime();
	object()->setTime(rand() / (double)RAND_MAX);

	return keep_going;
}
