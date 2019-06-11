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

#ifndef __Blot__BlotGL__
#define __Blot__BlotGL__

#include <QtWidgets/qopenglwidget.h>
#include <vector>

class BlotObject;

class BlotGL : public QOpenGLWidget
{
	Q_OBJECT
	
public:
	BlotGL(QWidget *parent = NULL);
	
	void addObject(BlotObject *obj);
public slots:
	void update();
	
protected:

private:
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL();
	void initialisePrograms();
	
	std::vector<BlotObject *> _objects;
};


#endif
