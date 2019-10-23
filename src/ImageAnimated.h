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

#ifndef __blot__ImageAnimated__
#define __blot__ImageAnimated__

#include "Instruction.h"

class ImageAnimated : public Instruction
{
public:
	ImageAnimated(BlotGL *gl = NULL, Instruction *inst = NULL);
	
	virtual BlotObject *object()
	{
		return _obj;
	}

	void setBlotObject(BlotObject *obj)
	{
		_obj = obj;
	}

	virtual bool isCovered(double x, double y);

	bool animateEffect();
	virtual void prepareEffect() = 0;
	virtual bool animateStep() = 0;
protected:
	virtual bool incrementTime();
	virtual void addProperties();
	virtual void linkReference(BaseParser *child, std::string name);
	void setTime(double time);

	double _time;
	double _stepTime;
	double _endTime;
	double _startTime;
	bool _fade;
	bool _valid;

	BlotObject *_obj;
private:
	
};


#endif
