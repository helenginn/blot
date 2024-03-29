// Blot
// Copyright (C) 2017-2019 Helen Ginn
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

#ifndef __Blot__ImageFade__
#define __Blot__ImageFade__

#include "ImageAppear.h"
#include "BlotObject.h"

class ImageFade : public ImageAppear
{
public:
	ImageFade(BlotGL *pres = NULL);

	bool isValid()
	{
		return _valid;
	}
	
	virtual std::string getClassName()
	{
		return "ImageFade";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "ImageFade_" + _random;
	}

	virtual void setBlotObject(BlotObject *obj);

	virtual std::string instText();
protected:
	virtual void postParseTidy();
private:

};


#endif
