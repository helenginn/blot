// Blot
// Copyright (C) 2017-2018 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as vpublished by
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

#ifndef __Blot__Instruction__
#define __Blot__Instruction__

#include "Parser.h"
#include <QTreeWidgetItem>

class Set;
class BlotGL;
class BlotObject;

class Instruction : public Parser, virtual public QTreeWidgetItem
{
public:
	Instruction(BlotGL *pres);
	virtual ~Instruction() {};
	
	virtual std::string getClassName()
	{
		return "Instruction";
	}
	
	virtual std::string getParserIdentifier()
	{
		return "Instruction" + _random;
	}
	
	virtual bool canMove()
	{
		return false;
	}
	
	virtual bool isCovered(double x, double y) { return false; }
	virtual void moveFractional(double fx, double fy) {};
	virtual void resizeFractional(double fx, double fy, bool aspect) {};
	virtual void rotateFractional(float x0, float y0, float fx, float fy) {};
	virtual void rotationalTranslate(float f, float mx, float my) {};
	
	virtual void position(double *x, double *y) {};
	
	void setOnClick(bool click)
	{
		_onClick = click;
		updateText();
	}
	
	virtual QString qText()
	{
		return QString::fromStdString(instText());
	}
	
	virtual std::string instText()
	{
		return "Instruct";
	}
	
	bool waitForClick()
	{
		return _onClick;
	}
	
	bool isTopLevel()
	{
		return QTreeWidgetItem::parent() == NULL;
	}
	
	virtual Set *instructionParent();
	
	virtual bool primaryLoad()
	{
		return false;
	}
	
	virtual void updateText();

	/* can return NULL! */
	virtual BlotObject *object()
	{
		return NULL;
	}
	
	virtual void instantEffect() = 0;
	virtual bool animateEffect(); 
	virtual bool animateStep() 
	{ 
		return false; 
	};
	
	bool hasDelay()
	{
		return (_delay > 0);
	}
	
	void setDelay(double d)
	{
		_delay = d;
		updateText();
	}
	
	double delay()
	{
		if (hasDelay()) return _delay;
		return 0;
	}
	
	virtual void postParseTidy();
	virtual void select(bool sel) {};
	virtual void addProperties();
	virtual void linkReference(BaseParser *child, std::string name);
	
	Instruction *nextInstruction();
	Instruction *prevInstruction();
	virtual Instruction *displayableInstruction();
	void setData(int column, int role, const QVariant &value);

	Instruction *adjacentInstruction(int dir);

	void setPresentation(BlotGL *newGL)
	{
		_presentation = newGL;
	}
protected:
	BlotGL *_presentation;
	std::string _random;

private:
	bool _onClick;
	double _delay;

};

Q_DECLARE_METATYPE(Instruction *)

#endif
