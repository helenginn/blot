// vagabond
//
// Created by Helen Ginn
// Copyright (c) 2018 Helen Ginn. All rights reserved.

#ifndef __blot__charmanip__
#define __blot__charmanip__

#include <sstream>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>

inline std::string f_to_str(double val, int precision)
{
	std::ostringstream ss;
	if (precision > 0)
	{
		ss << std::fixed << std::setprecision(precision);
	}
	else if (precision < 0)
	{
		ss << std::fixed;
	}

	ss << val;
	std::string temp = ss.str();

	return temp;
}

inline std::string i_to_str(int val)
{
	std::ostringstream ss;
	ss << val;
	std::string temp = ss.str();

	return temp;
}


inline std::string indent(int num)
{
	std::ostringstream stream;

	for (int i = 0; i < num; i++)
	{
		stream << "  ";
	}
	return stream.str();
}

inline char *strchrwhite(char *block)
{
	while (true)
	{
		if (*block == ' ' || *block == '\n' 
		    || *block == '\t' || *block == '\0')
		{
			return block;
		}
		
		block++;
	}
	
	return NULL;
}

inline void incrementIndent(char **block)
{
	while ((*block)[0] == ' ' || (*block)[0] == '\t' || (*block)[0] == '\n' || (*block)[0] == '\0')
	{
		(*block)++;
	}
}

inline char *keywordValue(char *block, char **keyword, char **value) 
{
	char *space = strchrwhite(block);

	if (space == NULL)
	{
		std::cout << "Space is just null" << std::endl;
		return NULL;
	}

	*space = '\0';
	*keyword = block;
	block = space + 1;
	incrementIndent(&block);

	// Don't panic, we probably just have an 'object'.
	if (block[0] != '=')
	{
		//        std::cout << "keyword: " << *keyword << " - block char " << *block << std::endl;
		return block;
	}

	block++;
	incrementIndent(&block);

	space = strchrwhite(block);
	*space = '\0';

	*value = block;
	block = space + 1;
	incrementIndent(&block);

	return block;
}

inline std::string get_file_contents(std::string filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);

	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize((unsigned long)in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}


	std::string errString = "Could not get file contents for file " + std::string(filename);
	std::cout << errString << std::endl;

	throw(errno);
}



#endif
