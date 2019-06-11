// Vagabond

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

#include "ParserTypes.h"
#include "Parser.h"

Parser *Parser::processBlock(char *block)
{
	char *start = &block[0];
	char header[] = "blot data structure";
	int headLength = strlen(header);
	start[headLength] = '\0';

	if (strcmp(start, header) != 0)
	{
		std::cout << "Aborting: does not look like "
		"a blot data structure." << std::endl;
		return NULL;
	}

	start = &start[headLength + 1];

	char *endline = strchr(start, '\n');
	*endline = '\0'; endline++;
	std::cout << "Blot data structure file is version " << 
	start << std::endl;

	start = endline;
	char *space = strchr(start, ' ');
	*space = '\0';   

	if (strcmp(start, "object") == 0)
	{
		start = space + 1;

		BaseParser *base = BaseParser::processBlock(start);

		return static_cast<Parser *>(base);
	}
	
	return NULL;
}

