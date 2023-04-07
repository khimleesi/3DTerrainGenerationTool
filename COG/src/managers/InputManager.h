#pragma once

/*******************************************************************************************************************
	InputManager.h, InputManager.cpp
	Created by Kim Kane
	Last updated: 25/03/2018

	Singleton class that handles all our in-game input.

	[Features]
	Supports mouse input, keyboard input and PS4 controller input.
	Buttons are already mapped and read from a file upon initialization - see "mappedButtons.config".
	Locking mechanisms to allow for repeat key presses or single key presses per frame.

	[Upcoming]
	Multi-player support (multiple controllers).

	[Side Notes]
	None at present.

*******************************************************************************************************************/

#include <pretty_sdl/sdl.h>
#include <pretty_glm/glm.hpp>
#include <string>
#include <map>

#include "managers/FileManager.h"
#include "utilities/Singleton.h"
#include "physics/AABounds2D.h"
#include "utilities/EventHandler.h"

namespace input_constants {

	enum TotalButtons : Uint8 { TOTAL_MOUSE_BUTTONS = 4, TOTAL_CONTROLLER_BUTTONS = 14 };

	const std::string SUPPORTED_GUID	= "4c05c405000000000000504944564944";
	const int BUFFER_SIZE				= 33;
	const Sint16 DEAD_ZONE				= 8000;
	const float SPEED					= 1.1f;
	const float ROTATE_SPEED			= 1.0f;
}

class InputManager {

	struct srMappedButtons {

		friend class cereal::access;

		std::map<std::string, Uint8> mouse;
		std::map<std::string, Uint8> controller;

	private:
		/*! @brief Function that contains serialization data for loading and saving. */
		template <class Archive>
		void Serialize(Archive& archive)
		{
			archive(COG_NVP(mouse),
					COG_NVP(controller));
		}
	};

public:
	friend class Singleton<InputManager>;

public:
	bool Initialize();
	void Update();
	void ShutDown();

public:
	bool IsWindowClosed() const;
	void SetWindowClosed(bool hasClosed);

public:
	bool IsKeyPressed(Uint8 key, bool repeat = true);
	bool IsMouseButtonPressed(Uint8 button, bool repeat = true);
	bool IsControllerButtonPressed(const std::string& button, bool repeat = false);
	bool IsMouseColliding(const AABounds2D& other);

public:
	EventHandler* GetEvents() { return &m_events; }

public:
	const glm::vec2& GetMousePosition() const;
	const glm::vec2& GetMouseMotion() const;
	const glm::vec2& GetMouseWheel() const;

public:
	const glm::vec2& GetControllerLeftAxis() const;
	const glm::vec2& GetControllerRightAxis() const;
	const glm::vec2& GetDirectionalPad() const;

public:
	void SetMousePosition(int x, int y);
	void SetMouseCursorType(const std::string& cursorType = "cursor.arrow");
	void SetMouseCursorState(const std::string& cursorEnabled = "cursor.on", const std::string& cursorVisible = "cursor.show");

private:
	InputManager();
	InputManager(const InputManager&)				= delete;
	InputManager& operator=(const InputManager&)	= delete;

private:
	bool LoadMappedButtons(const std::string& fileLocation, const std::string& objectName);
	void PrintObjectData();

private:
	void ResetMouse();
	void InitializeMouseButtons();
	void GenerateMouseMotionEvent(SDL_Event const&);
	void GenerateMouseButtonEvent(SDL_Event const&);
	void GenerateMouseWheelEvent(SDL_Event const&);

private:
	void LoadController();
	void InitializeControllerButtons();
	void CheckAxisMotion(SDL_Event const&, const std::string& button, float& axis, float speed);
	void GenerateControllerButtonEvent(SDL_Event const&);
	void GenerateControllerDirectionalPadEvent(SDL_Event const&);
	void GenerateControllerAxisEvent(SDL_Event const&);
	
private:
	EventHandler	m_events;
	bool			m_windowClosed;

private:
	Uint8*			m_keyStates;

private:
	SDL_Cursor*		m_cursor;
	glm::vec2		m_mousePosition;
	glm::vec2		m_mouseMotion;
	glm::vec2		m_mouseWheel;
	Uint8			m_mouseButtonState[input_constants::TOTAL_MOUSE_BUTTONS];

private:
	SDL_Joystick*	m_controller;
	SDL_JoystickID	m_controllerID;
	glm::vec2		m_controllerLeftAxisMotion;
	glm::vec2		m_controllerRightAxisMotion;
	glm::vec2		m_controllerDirectionalPad;
	Uint8			m_controllerButtonState[input_constants::TOTAL_CONTROLLER_BUTTONS];

private:
	srMappedButtons m_buttons;
};

typedef Singleton<InputManager> Input;