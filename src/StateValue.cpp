// vagabond
//
// Created by Helen Ginn
// Copyright (c) 2018 Helen Ginn. All rights reserved.

#include "StateValue.h"
#include "Parser.h"

void StateValue::applyToParser(BaseParser *parser)
{
	bool found = false;

	for (size_t i = 0; i < parser->_stringProperties.size(); i++)
	{
		if (parser->_stringProperties[i].ptrName == _ptrName)
		{
			*parser->_stringProperties[i].stringPtr = _string;

			found = true;
		}
	}

	for (size_t i = 0; i < parser->_doubleProperties.size(); i++)
	{
		if (parser->_doubleProperties[i].ptrName == _ptrName)
		{
			*parser->_doubleProperties[i].doublePtr = _double;

			found = true;
		}
	}

	for (size_t i = 0; i < parser->_mat3x3Properties.size(); i++)
	{
		if (parser->_mat3x3Properties[i].ptrName == _ptrName)
		{
			*parser->_mat3x3Properties[i].mat3x3Ptr = _mat3x3;

			found = true;
		}
	}

	for (size_t i = 0; i < parser->_vec3Properties.size(); i++)
	{
		if (parser->_vec3Properties[i].ptrName == _ptrName)
		{
			*parser->_vec3Properties[i].vec3Ptr = _vec3;

			found = true;
		}
	}

	for (size_t i = 0; i < parser->_vec3ArrayProperties.size(); i++)
	{
		if (parser->_vec3ArrayProperties[i].ptrName == _ptrName)
		{
			*parser->_vec3ArrayProperties[i].vec3ArrayPtr = _vec3Array;

			found = true;
		}
	}

	for (size_t i = 0; i < parser->_mat3x3ArrayProperties.size(); i++)
	{
		if (parser->_mat3x3ArrayProperties[i].ptrName == _ptrName)
		{
			*parser->_mat3x3ArrayProperties[i].mat3x3ArrayPtr = _mat3x3Array;

			found = true;
		}
	}

	for (size_t i = 0; i < parser->_boolProperties.size(); i++)
	{
		if (parser->_boolProperties[i].ptrName == _ptrName)
		{
			*parser->_boolProperties[i].boolPtr = _bool;

			found = true;
		}
	}

	for (size_t i = 0; i < parser->_intProperties.size(); i++)
	{
		if (parser->_intProperties[i].ptrName == _ptrName)
		{
			*parser->_intProperties[i].intPtr = _int;

			found = true;
		}
	}

	if (!found)
	{
		return;
	}
}

void StateValue::repairCustom()
{
	for (size_t i = 0; i < _custom.length(); i++)
	{
		if (_custom[i] == '\0')
		{
			_custom[i] = ' ';
		}	
	}	
}
