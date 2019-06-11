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
#include <QBuffer>
#include "BlotObject.h"
#include "charmanip.h"
#include <iostream>

ImageProc::ImageProc(QImage *image)
{
	QImage rgba = image->convertToFormat(QImage::Format_RGBA8888);
	_image = new QImage(rgba);
	_randomID = i_to_str(rand());
}

ImageProc::ImageProc()
{
	_image = NULL;
	_randomID = i_to_str(rand());
}

/*
ImageProc::ImageProc(const ImageProc &other)
{
	_image = other._image;
	_values = other._values;
}
*/

void ImageProc::collapseToActiveCoordinate()
{
	size_t tot_pix = _image->height() * _image->width();
	_values.resize(tot_pix);
	size_t count = 0;

	for (int y = 0; y < _image->height(); y++)
	{
		for (int x = 0; x < _image->width(); x++)
		{
			QColor colour = _image->pixel(x, y);

			int coord = 0;
			if (_image->hasAlphaChannel())
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

void ImageProc::bindToTexture(BlotObject *sender)
{
	std::cout << "Binding" << std::endl;
	glBindTexture(GL_TEXTURE_2D, sender->texture(0));
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _image->width(), _image->height(), 
	             0, GL_RGBA, GL_UNSIGNED_BYTE, _image->bits());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	sender->glGenerateMipmap(GL_TEXTURE_2D);

	/*
	glBindTexture(GL_TEXTURE_2D, sender->texture(1));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _image.width(), _image.height(), 
	             0, GL_RGBA, GL_UNSIGNED_BYTE, _image.bits());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	sender->glGenerateMipmap(GL_TEXTURE_2D);
	*/
}

void ImageProc::addProperties()
{
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	_image->save(&buffer, "PNG");
	_base64 = QString::fromLatin1(byteArray.toBase64().data()).toStdString();

	addStringProperty("id", &_randomID);
	addStringProperty("title", &_text);
	addStringProperty("base64", &_base64);
}

void ImageProc::postParseTidy()
{

}
