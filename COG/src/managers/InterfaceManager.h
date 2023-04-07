#pragma once

#include <pretty_sdl/sdl.h>
#include <pretty_sdl/SDL_syswm.h>
#include <map>
#include <pretty_opengl/glew.h>
#include "imgui/imgui.h"
#include "utilities/Singleton.h"
#include "managers/AudioManager.h"
#include "managers/ScreenManager.h"

class InterfaceManager : private NoCopyOrAssignment {

	friend class Singleton<InterfaceManager>;

public:
	static const char* GetClipboardText(void*);
	static void SetClipboardText(void*, const char* text);

	IMGUI_API bool Initialize(bool classic, const char* glsl_version);
	IMGUI_API void NewFrame();
	IMGUI_API void InvalidateDeviceObjects();
	IMGUI_API bool CreateDeviceObjects();
	IMGUI_API void Shutdown();
	IMGUI_API bool Render();	
	void CreateFontsTexture();

public:
	void Popup(const std::string& title, const std::string& message);
	void CreateButton(const std::string& tag, const std::string& description, float width, float height);
	bool IsButtonClicked(const std::string& tag);
	void BeginWindow(const std::string& title);
	void CreateDragVec3(const std::string& label, const std::string& tooltip, glm::vec3& data);

public:
	template <typename T>
	void CreateInputField(const std::string& label, const std::string& tooltip, T data[], size_t size)
	{
		ImGui::Text(label.c_str());
		std::string hiddenLabel = "##" + label;
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip(tooltip.c_str()); }
		ImGui::SameLine();
		ImGui::InputText(hiddenLabel.c_str(), data, size, ImGuiInputTextFlags_CharsNoBlank);
	}

private:
	static Uint64       s_Time;
	static bool         s_MousePressed[3];
	static SDL_Cursor*  s_MouseCursors[ImGuiMouseCursor_COUNT];
	static char*        s_ClipboardTextData;
	static char         s_GlslVersion[32];
	static GLuint       s_FontTexture;
	static int          s_ShaderHandle, s_VertHandle, s_FragHandle;
	static int          s_AttribLocationTex, s_AttribLocationProjMtx;
	static int          s_AttribLocationPosition, s_AttribLocationUV, s_AttribLocationColor;
	static unsigned int s_VboHandle, s_ElementsHandle;
	std::map<std::string, std::pair<std::string, ImVec2 > > m_Buttons;
};

typedef Singleton<InterfaceManager> GUI;