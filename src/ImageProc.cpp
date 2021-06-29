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
#include <float.h>

ImageProc::ImageProc(QImage *image)
{
	_image = NULL;

	if (image != NULL)
	{
		QImage rgba = image->convertToFormat(QImage::Format_RGBA8888);
		_image = new QImage(rgba);
	}

	_randomID = i_to_str(rand());
	_lastLeft = FLT_MAX;
	_lastTop = FLT_MAX;
}

ImageProc::~ImageProc()
{
	delete _image;
	_image = NULL;
}

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

void ImageProc::setImage(QImage &im)
{
	QImage rgba = im.convertToFormat(QImage::Format_RGBA8888);
	_image = new QImage(rgba);
	process();
	deletePreprocessing();
}

void ImageProc::process()
{
	collapseToActiveCoordinate();
}

int roundup(int val, int num = 4)
{
	if ((val % num) == 0)
	{
		return val;
	}
	
	val += (num - (val % num));
	return val;
}

bool within_bounds(int x, int y, int w, int h)
{
	return (x >= 0 && y >= 0 && x < w && y < h);
}

void ImageProc::prepareSeeds()
{
	/* go through and find all alpha values > 0 or on border of image */
	
	int image_width = roundup(_image->width());

	for (int y = 0; y < _image->height(); y++)
	{
		for (int x = 0; x < _image->width(); x++)
		{
			QColor colour = _image->pixelColor(x, y);
			int alpha = colour.alpha();

			/* if we are right on the border, or not transparent enough,
			 * we don't set as a seed */
			if ((x > 0 && y > 0 && x < _image->width() - 1
			    && y < _image->height() - 1) || alpha > 5)
			{
				continue;
			}
			
			int index = y * image_width + x;
			_processed[index]++;
		}
	}
}

void ImageProc::chooseSeeds()
{
	if (_points.size() == 0)
	{
		prepareSeeds();
		return;
	}
	
	int image_width = roundup(_image->width());

	for (size_t i = 0; i < _points.size(); i++)
	{
		vec3 point = _points[i];

		for (int sy = -5; sy < 5; sy++)
		{
			for (int sx = -5; sx < 5; sx++)
			{
				int px = point.x + sx;
				int py = point.y + sy;

				if (!within_bounds(px, py, 
				                  _image->width(), _image->height()))
				{
					continue;
				}

				int index = px + py * image_width;
				_processed[index]++;
			}
		}
	}
	
	std::cout << "Added " << _points.size() << " points." << std::endl;
}

void ImageProc::prepareProcessArray()
{
	if (hasPreprocessing())
	{
		memset(&_processed[0], '\0', 
		       _processed.size() * sizeof(unsigned char));
		return;
	}

	int image_width = roundup(_image->width());
	int image_height = roundup(_image->height());

	_processed.resize(image_width * image_height);
}

void ImageProc::preprocess(bool scratch)
{
	if (scratch && hasPreprocessing())
	{
		return;
	}

	prepareProcessArray();
	chooseSeeds();
	
	int image_width = roundup(_image->width());

	/* first build up a list of points to check */
	std::vector<vec3> prevPoints;
	std::vector<int> tmp = std::vector<int>(_processed.size(), 0);

	for (int y = 0; y < _image->height(); y++)
	{
		for (int x = 0; x < _image->width(); x++)
		{
			int index = y * image_width + x;
			tmp[index] = _processed[index];
			
			if (_processed[index] > 0)
			{
				vec3 vec = make_vec3(x, y, 0);
				prevPoints.push_back(vec);
			}
		}
	}
	
	/* Now we iteratively go through and build up the processed indices
	 * of all pixels where alpha > 1 */

	int max = 0;

	while (true)
	{
		std::vector<vec3> newPoints;
		std::vector<int> old = tmp;
		bool changed = false;
		
		max++;
		
		for (size_t i = 0; i < prevPoints.size(); i++)
		{
			int x = prevPoints[i].x;
			int y = prevPoints[i].y;

			for (int sy = -1; sy < 2; sy++)
			{
				for (int sx = -1; sx < 2; sx++)
				{
					if (!within_bounds(sx + x, sy + y, 
					                   _image->width(), _image->height()))
					{
						continue;
					}

					int check = (y + sy) * image_width + (x + sx);

					if (tmp[check] > 0)
					{
						continue;
					}

					if (_image->constBits()[check * 4 + 3] <= 100)
					{
						continue;
					}

					tmp[check] = max;
					vec3 vec = make_vec3(x + sx, y + sy, 0);
					newPoints.push_back(vec);
					changed = true;
				}
			}
		}

		if (!changed)
		{
			break;
		}
		
		prevPoints = newPoints;
	}
	
	max++;
	
	for (size_t i = 0; i < _processed.size(); i++)
	{
		if (_points.size() == 0)
		{
			_processed[i] = (max - tmp[i]) * 255 / max;
		}
		else
		{
			_processed[i] = tmp[i] * (double)255 / (double)max;
		}
	}
	
	std::cout << "Preprocessed in " << max << " steps." << std::endl;
}

void ImageProc::bindToTexture(BlotObject *sender)
{
	if (hasPreprocessing())
	{
		sender->glBindTexture(GL_TEXTURE_2D, sender->texture(1));
		sender->glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _image->width(), 
		                     _image->height(), 0, GL_RED, 
		                     GL_UNSIGNED_BYTE, &_processed[0]);
		sender->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		sender->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		sender->glGenerateMipmap(GL_TEXTURE_2D);
	}

}

void ImageProc::addProperties()
{
	if (_image != NULL && !_image->isNull())
	{
		QByteArray byteArray;
		QBuffer buffer(&byteArray);
		_image->save(&buffer, "PNG");
		_base64 = QString::fromLatin1(byteArray.toBase64().data()).toStdString();
	}
	
	if (_processed.size() > 0)
	{
		void *ptr = &_processed[0];
		QByteArray byteArray((const char *)ptr, 
		                     _processed.size() * sizeof(unsigned char));
		QBuffer buffer(&byteArray);
		_proc64 = QString::fromLatin1(byteArray.toBase64().data()).toStdString();
	}

	addStringProperty("id", &_randomID);
	addStringProperty("title", &_text);
	addStringProperty("base64", &_base64);
	addStringProperty("proc64", &_proc64);
	addVec3ArrayProperty("seeds", &_points);
}

void ImageProc::postParseTidy()
{
	QString str = QString::fromStdString(_base64);
	QByteArray b64 = str.toLatin1();
	
	if (_image != NULL)
	{
		delete _image;
	}
	
	_image = new QImage();
	_image->loadFromData(QByteArray::fromBase64(b64), "PNG");

	if (_proc64.length() == 0)
	{
		return;
	}

	str = QString::fromStdString(_proc64);
	b64 = str.toLatin1();
	QByteArray p64 = QByteArray::fromBase64(b64);
	int size = p64.size();
	_processed.resize(size);

	memcpy(&_processed[0], &*p64.begin(), size);
}
