#pragma once

/*******************************************************************************************************************
	Tools.h, Tools.cpp
	Created by Karsten Vermeulen

	Modified by Kim Kane
	Last updated: 06/01/2018

	*Still under construction.
	Handles all of our typical miscellaneous functions used, e.g. converting a string to an integer.

*******************************************************************************************************************/
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

//*************************************
//string case functions
//*************************************
void ToUpper(std::string& str);
void ToLower(std::string& str);

//*************************************
//string manipulation functions
//*************************************
void ListString(const std::string& str);
void RemoveCharacter(std::string& str, char character);
void CutString(std::string& str, int startPos, int length);
void AddString(std::string& finalString, const char* str);
void AddString(std::string& finalString, std::string& str);
void ParseString(std::string& str, std::vector<std::string>& subStrings, char token);
void ReplaceCharacter(std::string& str, const char current, const char replaceWith);

//*************************************
//string/type conversion functions
//*************************************
int StringToInteger(const std::string& str);
float StringToFloat(const std::string& str);
bool StringToBool(std::string const& s);

template <typename T> std::string NumberToString(T number) {
	std::stringstream str;
	str << number;
	return str.str();
}

//*************************************
//output message to console window
//*************************************
template <typename T> std::ostream& Debug(T str) {
	return std::cout << str << std::endl;;
}

/*!
	@brief
		Converts an enum value to its underlying type.

	@param value
		The enum value to convert.

	@return
		The underlying type of the enum e.g. float
	*******************************************************************************************************************/
template <typename Enumeration>
auto EnumToType(Enumeration const value) -> typename std::underlying_type<Enumeration>::type
{
	return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

template <typename Key, typename Value>
inline Key&	GetKey(std::pair<Key, Value>& that) { return that.first; }

template <typename Key, typename Value>
inline const Value&	GetValue(const std::pair<Key, Value>& that) { return that.second; }

template <typename Value, typename Iterator>
inline Value& GetValue(Iterator& that) { return that->second; }

template <typename Key, typename Iterator>
inline Key& GetKey(Iterator& that) { return that->first; }

template <class Derived, class Base>
Derived* Downcast(Base* base)
{
	Derived* derived = nullptr;

	if (base) {

		try { derived = dynamic_cast<Derived*>(base); }
		catch (const std::bad_cast& e) { std::cout << e.what() << '\n'; }
	}

	return derived;
}
