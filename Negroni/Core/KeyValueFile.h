#pragma once

#include "Base.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <unordered_map>

namespace Core
{
	class KeyValueFile
	{
	public:
		static std::unordered_map<str, str> Read(const str& filePath)
		{
			std::unordered_map<str, str> keyValueMap;

			std::ifstream	 infile(filePath);
			std::string	     line;

			if (!infile.is_open())
			{
				return keyValueMap;
			}

			while (std::getline(infile, line))
			{
				RTrim(line);
				LTrim(line);

				bool skipLine = line.empty() || line.at(0) == '#';
				if (skipLine)
					continue;

				std::istringstream buffer(line);
				std::vector<str> tokens{
					std::istream_iterator<str>{buffer},
					std::istream_iterator<str>{}
				};

				if (tokens.size() < 2)
					continue;

				keyValueMap[tokens[0]] = tokens[1];
			}

			return keyValueMap;
		}

	private:
		KeyValueFile();
	};
}
