// Blot
// Copyright (C) 2019 Helen Ginn
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

#ifndef __blot__ImageWiggle__
#define __blot__ImageWiggle__

#include "ImageAnimated.h"
#include "BlotObject.h"

class ImageWiggle : public ImageAnimated
{
public:
	ImageWiggle(BlotGL *gl = NULL, Instruction *inst = NULL);
	
	virtual bool isCovered(double, double)
	{
		return false;
	}
	
	virtual std::string getClassName()
	{
		return "ImageWiggle";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "ImageWiggle_" + _random;
	}

	virtual std::string instText();
	virtual bool animateStep();
	virtual void instantEffect();
	virtual void prepareEffect();
protected:
	virtual void addProperties();
	virtual void linkReference(BaseParser *child, std::string name);
private:
	double _angle;
};

#endif
