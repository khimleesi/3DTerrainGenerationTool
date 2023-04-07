#pragma once

/*******************************************************************************************************************
	Log.h, Log.cpp
	Created by Kim Kane
	Last updated: 04/01/2018
	
	A simple logging system that displays a message to the console window in a number of different colours.
	The colour intesity can be changed by passing true or false as an argument to the Colour function.
	
	More log types can be added to the LogType enum if necessary.
	This must also be updated within the switch statement in the Debug function.
	
	Supports variables of other data types (int, float, etc.) - when not passing a variable use COG_LOG_EMPTY.
	A macro can also be used for simplicity: #define LOG(message) std::cout << message << std::endl;

	Also supports OpenGL error messages (see defined macros below).

*******************************************************************************************************************/
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>

#define COG_LOG_EMPTY -1

/*******************************************************************************************************************
	Macros have been defined for differentiating between Debug and Release modes.
	The Debug and OpenGLDebug functions will only be called during Debug mode.

	You could also use ASSERT(function) if(!(function)) __debugbreak(); to generate a breakpoint.
*******************************************************************************************************************/
#if defined(COG_DEBUG)
	#define COG_LOG(message, variable, type) Debug(message, variable, type)
	#define COG_GLCALL(function) function; OpenGLDebug(#function, __FILE__, __LINE__)
#elif defined(COG_RELEASE)
	#define COG_LOG(message, variable, type)
	#define COG_GLCALL(function) function
#endif

/*******************************************************************************************************************
	Enums that define the log type (error message, etc.) and the log colour (colour of text in console)
*******************************************************************************************************************/
enum LogType	{ LOG_MESSAGE, LOG_WARN, LOG_ERROR, LOG_SUCCESS, LOG_MEMORY, LOG_RESOURCE, LOG_BREAK };
enum LogColour	{ COLOR_GREY, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED, COLOR_PINK, COLOR_YELLOW, COLOR_WHITE };

void Colour(LogColour colour, bool intensity = true);
void OpenGLDebug(const char* function, const char* fileName, int line);

/*******************************************************************************************************************
	Debug function that outputs text to the console window. Replaces std::cout << for faster debugging
*******************************************************************************************************************/
template <typename T> inline void Debug(const std::string& message, T variable, LogType type)
{
	//--- Create a local string to hold final debug message
	std::string outputString; 

	//--- Create a local sstream that holds the T variable data (int, float, char, etc.)
	std::stringstream tempVariable;

	//--- Store this data in our sstream so we can return a string copy of it using .str()
	tempVariable << variable; 
	
	//--- If variable is COG_LOG_EMPTY, just return the message. Otherwise, concatenate the strings
	if (variable != (T)COG_LOG_EMPTY)	{ outputString = message + tempVariable.str(); } 
	else								{ outputString = message; }

	//--- Switch to check the passed in LogType and output the passed in message and/or variable (complete with pretty colours), to make debugging easy and fast
	switch (type)  {

		case LOG_MESSAGE:	{ Colour(COLOR_GREY); std::cout << "[MESSAGE] " << outputString << std::endl; break; }
		case LOG_WARN:		{ Colour(COLOR_YELLOW); std::cout << "[WARNING] " << outputString << std::endl; break; }
		case LOG_ERROR:		{ Colour(COLOR_RED); std::cout << "[ERROR] " << outputString << std::endl; break; }
		case LOG_SUCCESS:	{ Colour(COLOR_GREEN); std::cout << "[SUCCESS] " << outputString << std::endl; break; }
		case LOG_MEMORY:	{ Colour(COLOR_PINK); std::cout << "[MEMORY] " << outputString << std::endl; break; }
		case LOG_RESOURCE:	{ Colour(COLOR_CYAN); std::cout << "[RESOURCE] " << outputString << std::endl; break; }
		case LOG_BREAK:		{ Colour(COLOR_WHITE); std::cout << "------------------------" << outputString << std::endl; break; }
	}

	Colour(COLOR_GREY);
}

namespace cog {
	/*!
			@brief
				A function that prints user-defined data to the console window in a specific color.

			@param message
				Initial message to output prior to user-defined data.

			@param color
				The color of the text.

			@param args
				The user-defined data to output.
			*******************************************************************************************************************/
	template<typename... Args>
	static void Print(const char* message, const unsigned short& color, Args&&... args)
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << "{ PRETTY }";
		SetConsoleTextAttribute(handle, color);
		std::cout << message; ((std::cout << std::forward<Args>(args) << ' '), ...); std::cout << std::endl;
		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);
	}
}

/*! @cond */
#define COG_LOG_LEVEL_ERROR 0
#define COG_LOG_LEVEL_WARNING 1
#define COG_LOG_LEVEL_SUCCESS 2
#define COG_LOG_LEVEL_MEMORY 3
#define COG_LOG_LEVEL_MESSAGE 4
#define COG_LOG_LEVEL_ALL ( COG_LOG_LEVEL_ERROR | COG_LOG_LEVEL_WARNING | COG_LOG_LEVEL_SUCCESS | COG_LOG_LEVEL_MEMORY | COG_LOG_LEVEL_MESSAGE )
/*! @endcond */

/*! @def COG_LOG_LEVEL
	@brief Defines the current log level to output to the console. */
#ifndef COG_LOG_LEVEL
#define COG_LOG_LEVEL COG_LOG_LEVEL_ERROR
#endif

	/*! @cond */
#define COG_ENABLE_LOGGING(x) ((COG_LOG_LEVEL == x) || (COG_LOG_LEVEL == COG_LOG_LEVEL_ALL))
/*! @endcond */

/*! @def COG_ERROR
	@brief Outputs an error message to the console window in the colour red. */
#if defined(COG_DEBUG) && COG_ENABLE_LOGGING(COG_LOG_LEVEL_ERROR)
#define COG_ERROR(...) cog::Print(" { ERROR } ", FOREGROUND_RED | FOREGROUND_INTENSITY, __VA_ARGS__)
#else
#define COG_ERROR(...)
#endif

	/*! @def COG_WARNING
		@brief Outputs a warning message to the console window in the colour yellow. */
#if defined(COG_DEBUG) && COG_ENABLE_LOGGING(COG_LOG_LEVEL_WARNING)
#define COG_WARNING(...) cog::Print(" { WARNING } ", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, __VA_ARGS__)
#else
#define COG_WARNING(...)
#endif

		/*! @def COG_SUCCESS
			@brief Outputs a message of success to the console window in the colour green. */
#if defined(COG_DEBUG) && COG_ENABLE_LOGGING(COG_LOG_LEVEL_SUCCESS)
#define COG_SUCCESS(...) cog::Print(" { SUCCESS } ", FOREGROUND_GREEN | FOREGROUND_INTENSITY, __VA_ARGS__)
#else
#define COG_SUCCESS(...)
#endif

			/*! @def COG_MEMORY
				@brief Outputs a memory allocation message to the console window in the colour cyan. */
#if defined(COG_DEBUG) && COG_ENABLE_LOGGING(COG_LOG_LEVEL_MEMORY)
#define COG_MEMORY(...) cog::Print(" { MEMORY } ", FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, __VA_ARGS__)
#else
#define COG_MEMORY(...)
#endif

				/*! @def COG_MESSAGE
					@brief Outputs a message to the console window in the colour grey. */
#if defined(COG_DEBUG) && COG_ENABLE_LOGGING(COG_LOG_LEVEL_MESSAGE)
#define COG_MESSAGE(...) cog::Print(" { MESSAGE } ", FOREGROUND_INTENSITY, __VA_ARGS__)
#else
#define COG_MESSAGE(...)
#endif

					/*! @def COG_ASSERT
						@brief Creates an assertion based on condition of `x`. If condition equals false, this will cause a debug break. */
#if defined(COG_DEBUG)
#define COG_ASSERT(x) if (!(x)) { COG_ERROR("Assertion failed: [", #x, "] in file:", __FILE__, "line:", __LINE__); __debugbreak(); }
#elif defined(COG_RELEASE)
#define COG_ASSERT(x)
#endif