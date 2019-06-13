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

#ifndef __Blot__ImageHide__
#define __Blot__ImageHide__

#include "Instruction.h"

class BlotGL;
class BlotObject;

class ImageHide : public Instruction
{
public:
	ImageHide(BlotGL *pres = NULL, Instruction *inst = NULL);

	bool isValid()
	{
		return _valid;
	}
	
	void setBlotObject(BlotObject *obj)
	{
		_obj = obj;
	}

	virtual BlotObject *object()
	{
		return _obj;
	}

	virtual std::string getClassName()
	{
		return "ImageHide";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "ImageHide_" + _random;
	}

	virtual std::string instText();

	virtual void makeEffect();
	virtual bool animateEffect();
	virtual bool animateStep();
protected:
	virtual void addProperties();
	virtual void linkReference(BaseParser *child, std::string name);
	void setTime(double time);
private:
	BlotObject *_obj;
	bool _valid;
	bool _fade;
};

#endif
