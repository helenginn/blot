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

typedef struct
{
	GLfloat pos[3];
	GLfloat normal[3];
	GLfloat color[4];
	GLfloat extra[4];
	GLfloat tex[2];
} Vertex;

class BlotObject : public QOpenGLFunctions
{
public:
	BlotObject();
	virtual ~BlotObject() {};
	void initialisePrograms();
	
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
protected:
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;
	
	bool hasImage()
	{
		return (getImage() != NULL);
	}
	
	QImage *getImage()
	{
		return NULL;
	}

private:
	GLuint addShaderFromString(GLuint program, GLenum type, std::string str);
	void rebindProgram();
	void bindTextures();
	GLuint _program;
	GLuint _bufferID;
	GLuint _vbo;
	std::vector<GLuint> _textures;
	
	bool _extra;
};

#endif
