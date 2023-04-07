#include <algorithm>
#include "Tools.h"

bool StringToBool(std::string const& s) {
	return s != "0";
}
//------------------------------------------------------------------------------------------------------
//function that converts characters in passed string object to uppercase letters  
//------------------------------------------------------------------------------------------------------
void ToUpper(std::string& str) {

	std::transform(str.begin(), str.end(), str.begin(), toupper);
}
//------------------------------------------------------------------------------------------------------
//function that converts characters in passed string object to lowercase letters
//------------------------------------------------------------------------------------------------------
void ToLower(std::string& str) {

	std::transform(str.begin(), str.end(), str.begin(), tolower);
}
//------------------------------------------------------------------------------------------------------
//function that displays size of passed string and loops through it to display each character 
//------------------------------------------------------------------------------------------------------
void ListString(const std::string& str) {

	std::cout << "Size of sting : " << str.length() << " characters" << std::endl;
	std::cout << "-------------------------------------------------" << std::endl;

	for (size_t i = 0; i < str.size(); i++) {

		std::cout << "Character #" << i << " : " << str[i] << std::endl;
	}
}
//------------------------------------------------------------------------------------------------------
//function that finds passed character in string object and removes it
//------------------------------------------------------------------------------------------------------
void RemoveCharacter(std::string& str, char character) {

	//try to find first instance of passed character and store its position
	auto it = std::find(str.begin(), str.end(), character);

	//if character could not be found display an error message
	if (it == str.end()) {

		std::cout << "Character not found." << std::endl;
	}

	//otherwise remove the character from the string and continue
	//to find and erase more occurrences of the character 
	//as long as the end of the string has not been reached 
	else {

		do {

			str.erase(it);
			it = std::find(str.begin(), str.end(), character);

		} while (it != str.end());
	}
}
//------------------------------------------------------------------------------------------------------
//function that cuts out a portion of a string based on values passed
//------------------------------------------------------------------------------------------------------
void CutString(std::string& str, int startPos, int length) {

	str = str.substr(startPos, length);
}
//------------------------------------------------------------------------------------------------------
//function that concatenates a passed substring to a string literal 
//------------------------------------------------------------------------------------------------------
void AddString(std::string& finalString, const char* str) {

	finalString += str;
}
//------------------------------------------------------------------------------------------------------
//function that concatenates two passed substrings 
//------------------------------------------------------------------------------------------------------
void AddString(std::string& finalString, std::string& str) {

	finalString.append(str);
}
//------------------------------------------------------------------------------------------------------
//function that splits a string object into segments based on token passed
//------------------------------------------------------------------------------------------------------
void ParseString(std::string& str, std::vector<std::string>& subStrings, char token) {

	unsigned int tokenPosition = 0;

	//first check if string is EMPTY and display error message if it is
	if (str.empty()) {

		std::cout << "The passed string is EMPTY." << std::endl;
	}

	//otherwise find the token position, store the substring  
	//and erase that portion of the original string object as 
	//long as the end of the string has not been reached 
	else {

		do {
			tokenPosition = str.find(token);
			subStrings.push_back(str.substr(0, tokenPosition));
			str.erase(0, tokenPosition + 1);

		} while (tokenPosition != std::string::npos);
	}
}
//------------------------------------------------------------------------------------------------------
//function that converts passed string object to an integer value
//------------------------------------------------------------------------------------------------------
int StringToInteger(const std::string& str) {

	return atoi(str.c_str());
}
//------------------------------------------------------------------------------------------------------
//function that converts passed string object to a float value
//------------------------------------------------------------------------------------------------------
float StringToFloat(const std::string& str) {

	return (float)atof(str.c_str());
}

//temp function being used in pipeline manager -DELETE!
std::string AddString(const std::string& a, const std::string& b)
{
	return a + b; // works because they are both strings.
}



void ReplaceCharacter(std::string& str, const char current, const char replaceWith) {
	std::replace(str.begin(), str.end(), current, replaceWith);
}