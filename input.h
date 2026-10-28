#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <limits>

namespace Input
{
	inline void clearBuffer()
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	inline bool hasUnextractedInput()
	{
		return !std::cin.eof() && std::cin.peek() != '\n';
	}

	inline void clearFailedExtraction()
	{
		if (!std::cin)
		{
			if (std::cin.eof())
			{
				std::exit(0);
			}

			std::cin.clear();
			clearBuffer();
		}
	}
}

#endif