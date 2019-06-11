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
#include <iostream>
#include <QImage>

BlotObject::BlotObject()
{
	initializeOpenGLFunctions();
	_extra = true;
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

	return shader;
}

void BlotObject::initialisePrograms()
{
	GLint result;

	/* create program object and attach shaders */
	_program = glCreateProgram();
	
	addShaderFromString(_program, GL_FRAGMENT_SHADER, std::string());
	addShaderFromString(_program, GL_VERTEX_SHADER, std::string());

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
    if (result == GL_FALSE)
    {
        std::cout << "sceneInit(): Program linking failed." << std::endl;

        /* delete the program */
        glDeleteProgram(_program);
        _program = 0;
    }

    glGenBuffers(1, &_bufferID);
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo);
    
	bindTextures();
	
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
	glBindTexture(GL_TEXTURE_2D, _textures[0]);
	
	QImage *image = getImage();
	QImage rgba = image->convertToFormat(QImage::Format_RGBA8888);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width(), image->height(), 
	             0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.bits());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);
}

void BlotObject::rebindProgram()
{
    glBufferData(GL_ARRAY_BUFFER, vSize(), vPointer(), GL_STATIC_DRAW);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize(), iPointer(), GL_STATIC_DRAW);

	/* Vertices */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
	                      (void *)(0 * sizeof(float)));
	/* Normals */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      (void *)(3 * sizeof(float)));

	/* Colours */
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
	                      (void *)(6 * sizeof(float)));

	if (_extra)
	{
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		                      (void *)(10 * sizeof(float)));
	}

	if (_textures.size())
	{
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(14 * sizeof(float)));
	}

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

	if (_extra)
	{
		glEnableVertexAttribArray(3);
	}

	if (_textures.size())
	{
		glEnableVertexAttribArray(4); 
	}
}

