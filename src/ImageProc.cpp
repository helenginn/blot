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

#include "ImageProc.h"

ImageProc::ImageProc(QImage *image)
{
	_image = *image;
}

void ImageProc::collapseToActiveCoordinate()
{
	size_t tot_pix = _image.height() * _image.width();
	_values.resize(tot_pix);
	size_t count = 0;

	for (int y = 0; y < _image.height(); y++)
	{
		for (int x = 0; x < _image.width(); x++)
		{
			QColor colour = _image.pixel(x, y);

			int coord = 0;
			if (_image.hasAlphaChannel())
			{
				coord = colour.alpha();
			}
			else
			{
				coord = colour.black();
			}
			
			_values[count].data = coord;
			_values[count].level = 0;
			
			count++;
		}
	}
}

void ImageProc::process()
{
	collapseToActiveCoordinate();

}
