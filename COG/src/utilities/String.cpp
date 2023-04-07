/*!
	@file String.cpp
	@brief Contains helper functions when working with strings.
	@author PrettyBinary
	@date 24/07/2018

*******************************************************************************************************************/

#include <algorithm>
#include <cctype>
#include "String.h"
#include "memory/Allocator.h"

	/*!
	@brief
		A function that converts a float to a c-style string.

	@param input
		The float to convert.

	@param places
		The number of decimal places to use.

	@return
		A pointer to the buffer data.
	*******************************************************************************************************************/
	char* String::Float(const float& input, unsigned int places)
	{
		memset(s_Buffer.get(), NULL, (size_t)KiB);
		sprintf(s_Buffer.get(), "%.*f", places, input);
			
		return s_Buffer.get();
	}


	/*!
	@brief
		A function that checks if a string contains any characters.

	@param string
		The string to check.

	@param chars
		The characters to look for.

	@return
		True if the string contains the chars, false otherwise.
	*******************************************************************************************************************/
	bool String::Contains(const std::string& string, const std::string& chars)
	{
		return string.find(chars) != std::string::npos;
	}


	/*!
	@brief
		A function that removes all special characters from a string.

	@param string
		The string to amend.

	@return
		The string without any special characters.
	*******************************************************************************************************************/
	std::string String::RemoveSpecialCharacters(std::string string)
	{
		string.erase(std::remove_if((string.begin()), string.end(), [](unsigned char c) {
			return (std::isspace(c) || !std::isalnum(c));
		}), string.end());

		return string;
	}
		
		
	/*!
	@brief
		A function that converts a void* to its hexadecimal value.

	@param input
		The void* to convert.

	@return
		A pointer to the buffer data.
	*******************************************************************************************************************/
	char* String::Hex(const void* input)
	{
		memset(s_Buffer.get(), NULL, (size_t)KiB);
		sprintf(s_Buffer.get(), "0x%p", input);
			
		return s_Buffer.get();
	}

	String::Buffer String::s_Buffer = std::make_unique<char[]>((size_t)KiB);