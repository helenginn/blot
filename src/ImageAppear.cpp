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
#include "maths.h"
#include <float.h>

ImageAppear::ImageAppear(BlotGL *pres) : ImageAnimated(pres)
{
	_left = -0.3;
	_right = 0.3;
	_top = 0.3;
	_bottom = -0.3;
	_angle = deg2rad(0);
	_endTime = 1.0;
}

void ImageAppear::instantEffect()
{
	prepareEffect();
	setTime(_endTime);
	_obj->setDisabled(false);
	_obj->setTime(_endTime);
}

void ImageAppear::prepareEffect()
{
	setTime(_startTime);
	_obj->setTime(_time);
	std::string text = _obj->getImage()->text();
	std::cout << "Show effect for " << text << std::endl;
	_obj->setVertices(_top, _bottom, _left, _right);
	_obj->setDisabled(false);
	_obj->rotateVertices(_angle);
	_presentation->update();
}

bool ImageAppear::animateStep()
{
	bool keep_going = incrementTime();
	_obj->setTime(_time);
	return keep_going;
}

void ImageAppear::addProperties()
{
	ImageAnimated::addProperties();
	
	addDoubleProperty("left", &_left);
	addDoubleProperty("right", &_right);
	addDoubleProperty("top", &_top);
	addDoubleProperty("bottom", &_bottom);
	addDoubleProperty("angle", &_angle);
}

void ImageAppear::position(double *x, double *y)
{
	*x = (_right + _left) / 2;
	*y = (_bottom + _top) / 2;
}

void ImageAppear::rotationalTranslate(float f, float mx, float my)
{
	double x, y;
	position(&x, &y);
	
	float dx = x - mx;
	float dy = y - my;
	
	float ndx = cos(f) * dx - sin(f) * dy;
	float ndy = sin(f) * dx + cos(f) * dy;
	ndx -= dx;
	ndy -= dy;

	moveFractional(-ndx, ndy);
}

void ImageAppear::rotateFractional(float x0, float y0, float fx, float fy)
{
	float angle = fx;
	
	if (angle != angle)
	{
		return;
	}
	
	double old_sin = sin(_angle) * sin(_angle);
	double old_cos = cos(_angle) * cos(_angle);
	_angle += angle;
	double new_sin = sin(_angle) * sin(_angle);
	double new_cos = cos(_angle) * cos(_angle);

	double aspect = _presentation->aspectRatio();
	
	double diff = _right - _left;
	double mid = (_right + _left) / 2;
	diff *= 1 + (aspect - 1) * (new_cos - old_cos);
	_left = mid - diff / 2;
	_right = mid + diff / 2;
	
	diff = _bottom - _top;
	mid = (_bottom + _top) / 2;
	diff *= 1 + (aspect - 1) * (new_sin - old_sin);
	_top = mid - diff / 2;
	_bottom = mid + diff / 2;

	_obj->setVertices(_top, _bottom, _left, _right);
	_obj->rotateVertices(_angle);
}

void ImageAppear::linkReference(BaseParser *child, std::string name)
{
	ImageAnimated::linkReference(child, name);
}

void ImageAppear::setNewImage(ImageProc *proc)
{
	BlotObject *obj = new BlotObject(proc);
	setBlotObject(obj);
	
	double l, t, a;
	proc->getLastDims(&l, &t, &a);
	
	if (l < FLT_MAX && t < FLT_MAX)
	{
		_left = l;
		_right = -l;
		_top = t;
		_bottom = -t;
		_angle = a;
		return;
	}
	
	double aspect = _presentation->aspectRatio();
	
	QImage *im = proc->getImage();
	int width = im->width();
	int height = im->height();

	double ratio = height / (double)width;
	_left = _bottom * ratio * aspect;
	_right = _top * ratio * aspect;
}

void ImageAppear::moveFractional(double fx, double fy)
{
	_left += fx;
	_right += fx;
	_top += fy;
	_bottom += fy;

	_obj->setVertices(_top, _bottom, _left, _right);
	_obj->rotateVertices(_angle);
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
	_obj->rotateVertices(_angle);
	_obj->getImage()->setLastDims((_left - _right) / 2, 
	                              (_top - _bottom) / 2, _angle);
}

void ImageAppear::select(bool sel)
{
	if (!_obj)
	{
		return;
	}
	
	_obj->select(sel, 0.0, 0.0, 0.5);
	_presentation->update();
}

std::string ImageAppear::instText()
{
	std::string start = "";
	start += (waitForClick() ? "" : "+ ");
	start += "Show " + object()->getImage()->text();
	return start;
}

