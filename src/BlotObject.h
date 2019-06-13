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
#include "Parser.h"

typedef struct
{
	GLfloat pos[3];
	GLfloat normal[3];
	GLfloat color[4];
	GLfloat extra[4];
	GLfloat tex[2];
} Vertex;

class ImageProc;
class BlotGL;

class BlotObject : public QOpenGLFunctions, public Parser
{
public:
	BlotObject(ImageProc *proc = NULL);
	virtual ~BlotObject() {};
	void initialisePrograms();
	void render(BlotGL *sender);
	
	virtual std::string getClassName()
	{
		return "BlotObject";
	}
	
	virtual std::string getParserIdentifier();
	
	Vertex *vPointer()
	{
		return &_vertices[0];
	}

	size_t vSize()
	{
		return sizeof(Vertex) * _vertices.size();
	}

	GLuint *iPointer()
	{
		return &_indices[0];
	}

	size_t iSize()
	{
		return sizeof(GLuint) * _indices.size();
	}
	
	GLuint texture(size_t i)
	{
		return _textures[i];
	}

	size_t indexCount()
	{
		return _indices.size();
	}
	
	bool isDisabled()
	{
		return _disabled;
	}
	
	void setDisabled(bool dis)
	{
		_disabled = dis;
	}
	
	bool hasImage()
	{
		return (_image != NULL);
	}
	
	ImageProc *getImage()
	{
		return _image;
	}
	
	void addProperties();
	void postParseTidy();

	void setVertices(float t, float b, float l, float r);
	
	void select(bool sel);
protected:
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;
	virtual void linkReference(BaseParser *child, std::string name);

private:
	GLuint addShaderFromString(GLuint program, GLenum type, std::string str);
	void checkErrors();
	void rebindProgram();
	void bindTextures();
	void makeDummy();

	std::string _random;
	GLuint _program;
	GLuint _bufferID;
	GLuint _vbo;
	GLuint _renderType;
	GLuint _uAspect;
	GLuint _uTime;
	ImageProc *_image;
	std::vector<GLuint> _textures;
	
	bool _extra;
	bool _disabled;
};

#endif
