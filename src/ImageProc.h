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

#ifndef __Blot__ImageProc__
#define __Blot__ImageProc__

#include <cstdlib>
#include <QImage>
#include <vector>
#include <QtGui/qopengl.h>
#include "Parser.h"

typedef struct
{
	int data;
	int level;
} ImageVal;

class BlotObject;

class ImageProc : public Parser
{
public:
	ImageProc(QImage *image = NULL);
	~ImageProc();
	void process();
	void preprocess(bool scratch = true);
	void bindToTexture(BlotObject *sender);

	virtual std::string getClassName()
	{
		return "ImageProc";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "image_" + _randomID;
	}
	
	virtual void addProperties();
	
	QImage *getImage()
	{
		return _image;
	}
	
	void setImage(QImage &im);

	void setText(std::string text)
	{
		_text = text;
	}

	QString qText()
	{
		return QString::fromStdString(_text);
	}
	
	std::string text()
	{
		return _text;
	}
	
	bool hasPreprocessing()
	{
		return (_processed.size() > 0);
	}
	
	void deletePreprocessing()
	{
		_processed.clear();
	}
	
	void prepareProcessArray();
	void prepareSeeds();
	void chooseSeeds();
	
	void setSeeds(std::vector<vec3> points)
	{
		_points = points;
	}

	void getLastDims(double *left, double *top, double *angle)
	{
		*left = _lastLeft;
		*top = _lastTop;
		*angle = _angle;
	}
	
	void setLastDims(double left, double top, double angle)
	{
		_lastLeft = left;
		_lastTop = top;
		_angle = angle;
	}
private:
	void collapseToActiveCoordinate();
	void postParseTidy();
	void base64();

	std::string _text;
	std::string _randomID;
	std::string _base64;
	double _lastLeft;
	double _lastTop;
	double _angle;
	QImage *_image;
	std::vector<vec3> _points;
	std::vector<unsigned char> _processed;
	std::string _proc64;
	std::vector<ImageVal> _values;

};

Q_DECLARE_METATYPE(ImageProc *)


#endif
