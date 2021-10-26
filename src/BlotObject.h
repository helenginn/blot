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

#ifndef __Blot__BlotObject__
#define __Blot__BlotObject__

#include <QtGui/qopengl.h>
#include <QtGui/qopenglfunctions.h>
#include "ImageProc.h"
#include <h3dsrc/SlipObject.h>
#include "Parser.h"

class ImageProc;
class BlotGL;

class BlotObject : public SlipObject, public Parser
{
public:
	BlotObject(ImageProc *proc = NULL);
	BlotObject(BlotObject &other);

	virtual ~BlotObject() {};
	
	virtual std::string getClassName()
	{
		return "BlotObject";
	}
	
	virtual std::string getParserIdentifier();
	
	void midpoint(double *x, double *y);
	
	bool hasImage()
	{
		return (_image != NULL);
	}
	
	ImageProc *getImage()
	{
		return _image;
	}
	
	void setTime(float time);
	
	void ignoreAspectRatio(bool ignore = true)
	{
		_ignoreAspectRatio = ignore;
	}
	
	bool shouldWipe()
	{
		return !_ignoreAspectRatio;
	}
	
	void addProperties();
	void postParseTidy();

	bool isCovered(double x, double y);
	void setVertices(float t, float b, float l, float r);
	void getVertices(double *t, double *b, double *l, double *r);
	void addToVertices(float x, float y);
	void rotateVertices(double angle);
	
	void select(bool sel, double red, double green, double blue);
	void wipeEffect();
	void preprocessImage();
	
	void setZCoord(float z);
	void prepareTextures();
protected:
	virtual void render(SlipGL *gl);
	virtual void extraUniforms();
	virtual void linkReference(BaseParser *child, std::string name);

private:
	void makeDummy();

	std::string _random;
	GLuint _uTime;
	ImageProc *_image;
	
	float *_transform;
	float *_aspectFloat;
	float _time;
	bool _ignoreAspectRatio;
};

#endif
