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
	_left = -0.5;
	_right = 0.5;
	_top = 0.5;
	_bottom = -0.5;
}

void ImageAppear::makeEffect()
{
	std::string text = _obj->getImage()->text();
	std::cout << "Make effect for " << text << std::endl;
	_obj->setDisabled(false);

	_presentation->dodgyRefresh();
	
	_presentation->update();
}

void ImageAppear::setNewImage(ImageProc *proc)
{
	std::cout << "Setting new image" << std::endl;
	_obj = new BlotObject(proc);
}

