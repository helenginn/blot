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

#ifndef __Blot__ChangeBackground__
#define __Blot__ChangeBackground__

#include "Instruction.h"

class BlotGL;
class BlotObject;

class ChangeBackground : public Instruction
{
public:
	ChangeBackground(BlotGL *pres = NULL, ImageProc *proc = NULL);

	virtual std::string getClassName()
	{
		return "ChangeBackground";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "ChangeBackground" + _random;
	}

	virtual BlotObject *object()
	{
		return _obj;
	}

	virtual bool primaryLoad()
	{
		return true;
	}

	virtual std::string instText();
	virtual void instantEffect();

	virtual void addProperties();
	virtual void linkReference(BaseParser *child, std::string name);
protected:

private:
	BlotObject *_obj;

};


#endif
