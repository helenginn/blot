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
#include <string>
#include <QImage>
#include "shaders/vImage.h"
#include "shaders/fImage.h"
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
	Vertex v; 
	memset(&v, 0, sizeof(Vertex));
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

BlotObject::BlotObject(ImageProc *proc)
{
	_time = 1;
	_random = i_to_str(rand());
	_renderType = GL_TRIANGLES;
	_image = proc;
	makeDummy();
	_disabled = true;
	_extra = true;
	_program = 0;
}

GLuint BlotObject::addShaderFromString(GLuint program, GLenum type, 
                                       std::string str)
{
	GLint length = str.length();

	const char *cstr = str.c_str();
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &cstr, &length);
	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		char *log;

		/* get the shader info log */
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		log = (char *)malloc(length);
		glGetShaderInfoLog(shader, length, &result, log);

		/* print an error message and the info log */
		std::cout << "Shader: unable to compile: " << std::endl;
		std::cout << str << std::endl;
		std::cout << log << std::endl;
		free(log);

		glDeleteShader(shader);
		return 0;
	}

	glAttachShader(_program, shader);
	return shader;
}

void BlotObject::deletePrograms()
{
	glDeleteProgram(_program);
	_program = 0;
}

void BlotObject::initialisePrograms(std::string *v, std::string *f)
{
	if (v == NULL)
	{
		v = &vImage;
	}

	if (f == NULL)
	{
		f = &fImage;
	}
	
	initializeOpenGLFunctions();
	bindTextures();

	GLint result;

	/* create program object and attach shaders */
	_program = glCreateProgram();

	addShaderFromString(_program, GL_VERTEX_SHADER, *v);
	addShaderFromString(_program, GL_FRAGMENT_SHADER, *f);

	glBindAttribLocation(_program, 0, "position");
	glBindAttribLocation(_program, 1, "normal");
	glBindAttribLocation(_program, 2, "color");

	if (!_extra)
	{
		glBindAttribLocation(_program, 3, "projection");
	}
	else
	{
		glBindAttribLocation(_program, 3, "extra");
	}

	if (_textures.size())
	{
		glBindAttribLocation(_program, 4, "tex");
	}

	/* link the program and make sure that there were no errors */
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &result);
	checkErrors();

	if (result == GL_FALSE)
	{
		std::cout << "sceneInit(): Program linking failed." << std::endl;

		/* delete the program */
		glDeleteProgram(_program);
		_program = 0;
	}

	glGenBuffers(1, &_bufferID);
	glGenBuffers(1, &_vbo);
	rebindProgram();
}

void BlotObject::bindTextures()
{
	if (!hasImage())
	{
		return;
	}

	_textures.resize(1);

	glGenTextures(1, &_textures[0]);

	getImage()->bindToTexture(this);
}

void BlotObject::rebindProgram()
{
	glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo);

	glBufferData(GL_ARRAY_BUFFER, vSize(), vPointer(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize(), iPointer(), GL_STATIC_DRAW);

	checkErrors();

	/* Vertices */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
	                      (void *)(0 * sizeof(float)));
	/* Normals */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      (void *)(3 * sizeof(float)));

	/* Colours */
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
	                      (void *)(6 * sizeof(float)));

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      (void *)(10 * sizeof(float)));

	checkErrors();

	if (_textures.size())
	{
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(14 * sizeof(float)));
	}

	checkErrors();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	if (_textures.size())
	{
		glEnableVertexAttribArray(4); 
	}
}

void BlotObject::checkErrors()
{
	GLenum err = glGetError();

	if (err != 0)
	{
		std::cout << "OUCH " << err << std::endl;
		
		switch (err)
		{
			case GL_INVALID_ENUM:
			std::cout << "Invalid enumeration" << std::endl;
			break;

			case GL_STACK_OVERFLOW:
			std::cout << "Stack overflow" << std::endl;
			break;

			case GL_STACK_UNDERFLOW:
			std::cout << "Stack underflow" << std::endl;
			break;

			case GL_OUT_OF_MEMORY:
			std::cout << "Out of memory" << std::endl;
			break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "Invalid framebuffer op" << std::endl;
			break;

			case GL_INVALID_VALUE:
			std::cout << "Invalid value" << std::endl;
			break;

			case GL_INVALID_OPERATION:
			std::cout << "Invalid operation" << std::endl;
			break;

		}
	}
}

void BlotObject::render(BlotGL *sender)
{
	if (_disabled)
	{
		return;
	}
	
	glUseProgram(_program);
	rebindProgram();
	
	mat3x3 aspect = sender->getAspectMatrix();
	float *toFloat = mat3x3_malloc_float3x3(aspect);

	const char *uniform_name = "aspect";
	_uAspect = glGetUniformLocation(_program, uniform_name);
	glUniformMatrix3fv(_uAspect, 1, GL_FALSE, toFloat);
	checkErrors();

	const char *time_name = "time";
	_uTime = glGetUniformLocation(_program, time_name);
	glUniform1fv(_uTime, 1, &_time);

	if (_textures.size())
	{
		glBindTexture(GL_TEXTURE_2D, _textures[0]);
	}
	
	glDrawElements(_renderType, indexCount(), GL_UNSIGNED_INT, 0);
	checkErrors();
	
	free(toFloat);

	glUseProgram(0);
}

void BlotObject::select(bool sel, double red, double green, double blue)
{
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

void BlotObject::changeProgram(std::string &v, std::string &f)
{
	deletePrograms();
	initialisePrograms(&v, &f);
}

std::string BlotObject::getParserIdentifier()
{
	return "BO_" + _image->getParserIdentifier() + "_" + _random;
}

void BlotObject::addProperties()
{
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


void BlotObject::setDisabled(bool dis)
{
	_disabled = dis;
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
