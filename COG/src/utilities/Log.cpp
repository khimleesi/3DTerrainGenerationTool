#include <pretty_opengl/glew.h>

#include "Log.h"

/*******************************************************************************************************************
	Handles the text colour and colour intensity of the console window text using a Win32 handle
*******************************************************************************************************************/
void Colour(LogColour colour, bool intensity)
{
	//--- Checks the intensity (bool) of the colour (true is bright, false is dark)
	WORD colourIntesity = (intensity) ? FOREGROUND_INTENSITY : 0;

	//--- Set the text attribute for the console using the handle and pass in the colour and the intensity we want
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour | colourIntesity);
}


/*******************************************************************************************************************
	Loop through all of the OpenGL errors and output them to the debug window
*******************************************************************************************************************/
void OpenGLDebug(const char* function, const char* fileName, int line) {

	static bool processErrors = true;

	while (processErrors) {

		GLenum errorMessage = glGetError();

		if (errorMessage == GL_NO_ERROR) { break; }

		switch (errorMessage) {

			case GL_INVALID_ENUM:					{ COG_LOG("[OpenGL] Invalid enumeration: ", function, LOG_ERROR); break; }
			case GL_INVALID_VALUE:					{ COG_LOG("[OpenGL] Invalid value: ", function, LOG_ERROR); break; }
			case GL_INVALID_OPERATION:				{ COG_LOG("[OpenGL] Invalid operation: ", function, LOG_ERROR); break; }
			case GL_INVALID_FRAMEBUFFER_OPERATION:	{ COG_LOG("[OpenGL] Invalid framebuffer operation: ", function, LOG_ERROR); break; }
			case GL_OUT_OF_MEMORY:					{ COG_LOG("[OpenGL] Out of memory: ", function, LOG_ERROR); break; }
			case GL_STACK_UNDERFLOW:				{ COG_LOG("[OpenGL] Stack underflow: ", function, LOG_ERROR); break; }
			case GL_STACK_OVERFLOW:					{ COG_LOG("[OpenGL] Stack overflow: ", function, LOG_ERROR); break; }
		}

		COG_LOG("[OpenGL] Line: ", line, LOG_ERROR); COG_LOG("[OpenGL] File: ", fileName, LOG_ERROR); processErrors = false;
	}
}