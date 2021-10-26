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

#include "BlotObject.h"
#include "BlotGL.h"
#include "charmanip.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <QImage>
#include "shaders/vImage.h"
#include "shaders/fImage.h"
#include "shaders/fpImage.h"
#include "shaders/fWipe.h"

void BlotObject::addToVertices(float x, float y)
{
	for (size_t i = 0; i < _vertices.size(); i++)
	{
		_vertices[i].pos[0] += x;
		_vertices[i].pos[1] += y;
	}
}

bool BlotObject::isCovered(double x, double y)
{
	if (isDisabled())
	{
		return false;
	}
	
	int all_s[] = {0, 1, 3, 2};
	int all_e[] = {1, 3, 2, 0};

	int crosses = 0;

	for (int i = 0; i < 4; i++)
	{
		int s = all_s[i];
		int e = all_e[i];

		float sx = _vertices[s].pos[1];
		float sy = _vertices[s].pos[0];
		float ex = _vertices[e].pos[1];
		float ey = _vertices[e].pos[0];

		/* ys are outside of the range of this line cross */
		if ((sy < ey && (y < sy || y > ey)) ||
		    (sy >= ey && (y > sy || y < ey)))
		{
			continue;
		}

		float px = sx + (y - sy) * (ex - sx) / (ey - sy);

		/* cross point is to the right of the x value */
		if (px < x)
		{
			continue;
		}

		crosses++;
	}

	return (crosses % 2 == 1);
}

void BlotObject::setZCoord(float z)
{
	for (size_t i = 0; i < _vertices.size(); i++)
	{
		_vertices[i].pos[2] = z;
	}
}

void BlotObject::getVertices(double *t, double *b, double *l, double *r)
{
	if (_vertices.size() == 0)
	{
		return;
	}
	
	*b = _vertices[0].pos[0];
	*l = _vertices[0].pos[1];
	*r = _vertices[1].pos[1]; 
	*t = _vertices[2].pos[0];
}

void BlotObject::setVertices(float t, float b, float l, float r)
{
	if (_vertices.size() == 0)
	{
		makeDummy();
	}
	
	_vertices[0].pos[0] = b;
	_vertices[0].pos[1] = l;
	
	_vertices[1].pos[0] = b;
	_vertices[1].pos[1] = r;
	
	_vertices[2].pos[0] = t;
	_vertices[2].pos[1] = l;
	
	_vertices[3].pos[0] = t;
	_vertices[3].pos[1] = r;
}

void BlotObject::rotateVertices(double angle)
{
	double sina = sin(angle);
	double cosa = cos(angle);

	double mx, my;
	midpoint(&mx, &my);
	
	for (size_t i = 0; i < _vertices.size(); i++)
	{
		float dx, dy;
		dx = _vertices[i].pos[0] - mx;
		dy = _vertices[i].pos[1] - my;
		
		float nx = dx * cosa - dy * sina;
		float ny = dx * sina + dy * cosa;
		
		_vertices[i].pos[0] = mx + nx;
		_vertices[i].pos[1] = my + ny;
	}
}

void BlotObject::makeDummy()
{
	Helen3D::Vertex v; 
	memset(&v, 0, sizeof(Helen3D::Vertex));
	v.pos[0] = -0.5; v.pos[1] = -0.5;
	v.tex[0] = 0; v.tex[1] = 1;
	_vertices.push_back(v);
	v.pos[0] = -0.5; v.pos[1] = +0.5;
	v.tex[0] = 0; v.tex[1] = 0;
	 _vertices.push_back(v);
	v.pos[0] = +0.5; v.pos[1] = -0.5;
	v.tex[0] = 1; v.tex[1] = 1;
	_vertices.push_back(v);
	v.pos[0] = +0.5; v.pos[1] = +0.5;
	v.tex[0] = 1; v.tex[1] = 0;
	_vertices.push_back(v);
	
	_indices.push_back(0);
	_indices.push_back(1);
	_indices.push_back(2);
	_indices.push_back(2);
	_indices.push_back(1);
	_indices.push_back(3);
}

BlotObject::BlotObject(BlotObject &other) : SlipObject()
{
	_transform = other._transform;
	_aspectFloat = other._aspectFloat;
	_time = other._time;
	_random = i_to_str(rand());
	_image = other._image;
	_ignoreAspectRatio = other._ignoreAspectRatio;
	_vString = other._vString;
	_fString = other._fString;

	makeDummy();
	setDisabled(true);
	setNeedsExtra(true);
	_textured = true;
	_handleOwnTextures = true;
}

BlotObject::BlotObject(ImageProc *proc) : SlipObject()
{
	_transform = NULL;
	_aspectFloat = NULL;
	_time = 1;
	_random = i_to_str(rand());
	_image = proc;
	makeDummy();
	setDisabled(true);
	_ignoreAspectRatio = false;
	setNeedsExtra(true);
	_textured = true;
	_handleOwnTextures = true;
	_vString = vImage;
	_fString = fImage;
}

void BlotObject::prepareTextures()
{
	if (!hasImage())
	{
		return;
	}

	int num = 1;
	if (getImage()->hasPreprocessing())
	{
		num = 2;
	}
	

	if (_textures.size() < num)
	{
		_textures.resize(num);
		glGenTextures(num, &_textures[0]);
		checkErrors("textures");
		bindOneTexture(getImage()->getImage(), true);
		getImage()->bindToTexture(this);
	}
}

void BlotObject::render(SlipGL *gl)
{
	initializeOpenGLFunctions();
	prepareTextures();

	SlipObject::render(gl);
}

void BlotObject::extraUniforms()
{	
	const char *time_name = "time";
	_uTime = glGetUniformLocation(_usingProgram, time_name);
	glUniform1fv(_uTime, 1, &_time);
	
	if (!shouldWipe())
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	if (_textures.size())
	{
		{
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, texture(0));
			GLuint uTex = glGetUniformLocation(_usingProgram, "pic_tex");
			glUniform1i(uTex, 0);
		}

		if (getImage()->hasPreprocessing())
		{
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, texture(1));
			GLuint uTex = glGetUniformLocation(_usingProgram, "proc_tex");
			glUniform1i(uTex, 1);
		}
	}
}

void BlotObject::select(bool sel, double red, double green, double blue)
{
	std::cout << (sel ? "Selecting " : "Deselecting ");
	std::cout << getImage()->text() << std::endl;
	
	if (_vertices.size() == 0)
	{
		makeDummy();
	}

	for (size_t i = 0; i < _vertices.size(); i++)
	{
		_vertices[i].color[0] = red * sel;
		_vertices[i].color[1] = green * sel;
		_vertices[i].color[2] = blue * sel;
		_vertices[i].color[3] = 0.0;
	}
}

std::string BlotObject::getParserIdentifier()
{
	return "BO_" + _image->getParserIdentifier() + "_" + _random;
}

void BlotObject::addProperties()
{
	addBoolProperty("ignore_aspect", &_ignoreAspectRatio);
	addStringProperty("random", &_random);
	addReference("image", _image);
}

void BlotObject::linkReference(BaseParser *child, std::string name)
{
	if (name == "image")
	{
		_image = static_cast<ImageProc *>(child);
	}
}

void BlotObject::wipeEffect()
{
	changeProgram(vImage, fImage);
}

void BlotObject::postParseTidy()
{

}

void BlotObject::midpoint(double *x, double *y)
{
	*x = 0; *y = 0;

	for (size_t i = 0; i < _vertices.size(); i++)
	{
		*x += _vertices[i].pos[0];
		*y += _vertices[i].pos[1];
	}
	
	*x /= (double)_vertices.size();
	*y /= (double)_vertices.size();
}

void BlotObject::setTime(float time)
{
	_time = time;
}

void BlotObject::preprocessImage()
{
	getImage()->preprocess();
}
