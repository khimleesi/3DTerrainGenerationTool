#pragma once
/*!
	@file String.h
	@brief Contains helper functions when working with strings.
	@author PrettyBinary
	@date 24/07/2018

	@class Pretty::Utilities::String
	@brief Static class which contains string helper functions.

*******************************************************************************************************************/

#include <memory>
#include <string>

	class String {

	public:
		static char* Hex(const void* input);
		static char* Float(const float& input, unsigned int places = 3);
		static bool Contains(const std::string& string, const std::string& chars);
		static std::string RemoveSpecialCharacters(std::string string);

	private:
		typedef std::unique_ptr<char[]> Buffer;	/*!< Heap allocated char buffer. */

	private:
		static Buffer s_Buffer;	/*!< Buffer storage for when working with c style strings. */
	};
