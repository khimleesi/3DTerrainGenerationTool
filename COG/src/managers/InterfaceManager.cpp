#include "InterfaceManager.h"
#include "managers/ScreenManager.h"
#include "managers/InputManager.h"
#include "managers/GameManager.h"

const char* InterfaceManager::GetClipboardText(void*)
{
	if (s_ClipboardTextData) { SDL_free(s_ClipboardTextData); }
	s_ClipboardTextData = SDL_GetClipboardText();
	return s_ClipboardTextData;
}

void InterfaceManager::SetClipboardText(void*, const char* text)
{
	SDL_SetClipboardText(text);
}

bool InterfaceManager::Initialize(bool classic, const char* glsl_version)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Store GLSL version string so we can refer to it later in case we recreate shaders. Note: GLSL version is NOT the same as GL version. Leave this to NULL if unsure.
	if (glsl_version == NULL)
		glsl_version = "#version 400";
	IM_ASSERT((int)strlen(glsl_version) + 2 < IM_ARRAYSIZE(s_GlslVersion));
	strcpy(s_GlslVersion, glsl_version);
	strcat(s_GlslVersion, "\n");

	// Setup back-end capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;   // We can honor GetMouseCursor() values (optional)

															// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

	io.SetClipboardTextFn = SetClipboardText;
	io.GetClipboardTextFn = GetClipboardText;
	io.ClipboardUserData = nullptr;

	s_MouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	s_MouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	s_MouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	s_MouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	s_MouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	s_MouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
	s_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);

#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(Screen::Instance()->GetWindow(), &wmInfo);
	io.ImeWindowHandle = wmInfo.info.win.window;
#else
	(void)window;
#endif

	(classic) ? ImGui::StyleColorsClassic() : ImGui::StyleColorsDark();

	Input::Instance()->GetEvents()->Register(SDL_MOUSEWHEEL, [&](SDL_Event const& event) {
		(event.wheel.x > 0.0f) ? io.MouseWheelH += 1.0f : io.MouseWheelH -= 1.0f;
		(event.wheel.y > 0.0f) ? io.MouseWheel += 1.0f : io.MouseWheel -= 1.0f;
	});

	Input::Instance()->GetEvents()->Register(SDL_TEXTINPUT, [&](SDL_Event const& event) {
		io.AddInputCharactersUTF8(event.text.text);	
	});

	Input::Instance()->GetEvents()->Register(SDL_KEYUP, SDL_KEYDOWN, [&](SDL_Event const& event) {
		int key = event.key.keysym.scancode;
		IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
		io.KeysDown[key] = (event.type == SDL_KEYDOWN);
		io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
		io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
		io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
		io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
	});

	return true;
}


void InterfaceManager::NewFrame()
{
	if (!s_FontTexture) { CreateDeviceObjects(); }

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	SDL_GetWindowSize(Screen::Instance()->GetWindow(), &w, &h);
	SDL_GL_GetDrawableSize(Screen::Instance()->GetWindow(), &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	float currentTicks = (Game::Instance()->GetCurrentFrameTime() / 1000.0f);
	io.DeltaTime = currentTicks > 0 ? currentTicks : float(1.0f / 60.0f);
	
	// Setup mouse inputs (we already got mouse wheel, keyboard keys & characters from our event handler)
	int mx, my;
	Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	io.MouseDown[0] = s_MousePressed[0] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = s_MousePressed[1] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	io.MouseDown[2] = s_MousePressed[2] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
	s_MousePressed[0] = s_MousePressed[1] = s_MousePressed[2] = false;

	// We need to use SDL_CaptureMouse() to easily retrieve mouse coordinates outside of the client area. This is only supported from SDL 2.0.4 (released Jan 2016)
#if (SDL_MAJOR_VERSION >= 2) && (SDL_MINOR_VERSION >= 0) && (SDL_PATCHLEVEL >= 4)   
	if ((SDL_GetWindowFlags(Screen::Instance()->GetWindow()) & (SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_MOUSE_CAPTURE)) != 0)
		io.MousePos = ImVec2((float)mx, (float)my);
	bool any_mouse_button_down = false;
	for (int n = 0; n < IM_ARRAYSIZE(io.MouseDown); n++)
		any_mouse_button_down |= io.MouseDown[n];
	if (any_mouse_button_down && (SDL_GetWindowFlags(Screen::Instance()->GetWindow()) & SDL_WINDOW_MOUSE_CAPTURE) == 0)
		SDL_CaptureMouse(SDL_TRUE);
	if (!any_mouse_button_down && (SDL_GetWindowFlags(Screen::Instance()->GetWindow()) & SDL_WINDOW_MOUSE_CAPTURE) != 0)
		SDL_CaptureMouse(SDL_FALSE);
#else
	if ((SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0)
		io.MousePos = ImVec2((float)mx, (float)my);
#endif

	// Update OS/hardware mouse cursor if imgui isn't drawing a software cursor
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
	{
		ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
		if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None)
		{
			SDL_ShowCursor(0);
		}
		else
		{
			SDL_SetCursor(s_MouseCursors[cursor] ? s_MouseCursors[cursor] : s_MouseCursors[ImGuiMouseCursor_Arrow]);
			SDL_ShowCursor(1);
		}
	}

	// Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
	ImGui::NewFrame();
}


void InterfaceManager::InvalidateDeviceObjects()
{
	if (s_VboHandle) glDeleteBuffers(1, &s_VboHandle);
	if (s_ElementsHandle) glDeleteBuffers(1, &s_ElementsHandle);
	s_VboHandle = s_ElementsHandle = 0;

	if (s_ShaderHandle && s_VertHandle) glDetachShader(s_ShaderHandle,s_VertHandle);
	if (s_VertHandle) glDeleteShader(s_VertHandle);
	s_VertHandle = 0;

	if (s_ShaderHandle && s_FragHandle) glDetachShader(s_ShaderHandle, s_FragHandle);
	if (s_FragHandle) glDeleteShader(s_FragHandle);
	s_FragHandle = 0;

	if (s_ShaderHandle) glDeleteProgram(s_ShaderHandle);
	s_ShaderHandle = 0;

	if (s_FontTexture)
	{
		glDeleteTextures(1, &s_FontTexture);
		ImGui::GetIO().Fonts->TexID = 0;
		s_FontTexture = 0;
	}
}


bool InterfaceManager::CreateDeviceObjects()
{
	// Backup GL state
	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	const GLchar *vertex_shader =
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	const GLchar* vertex_shader_with_version[2] = { s_GlslVersion, vertex_shader };
	const GLchar* fragment_shader_with_version[2] = { s_GlslVersion, fragment_shader };

	s_ShaderHandle = glCreateProgram();
	s_VertHandle = glCreateShader(GL_VERTEX_SHADER);
	s_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(s_VertHandle, 2, vertex_shader_with_version, NULL);
	glShaderSource(s_FragHandle, 2, fragment_shader_with_version, NULL);
	glCompileShader(s_VertHandle);
	glCompileShader(s_FragHandle);
	glAttachShader(s_ShaderHandle, s_VertHandle);
	glAttachShader(s_ShaderHandle, s_FragHandle);
	glLinkProgram(s_ShaderHandle);

	s_AttribLocationTex = glGetUniformLocation(s_ShaderHandle, "Texture");
	s_AttribLocationProjMtx = glGetUniformLocation(s_ShaderHandle, "ProjMtx");
	s_AttribLocationPosition = glGetAttribLocation(s_ShaderHandle, "Position");
	s_AttribLocationUV = glGetAttribLocation(s_ShaderHandle, "UV");
	s_AttribLocationColor = glGetAttribLocation(s_ShaderHandle, "Color");

	glGenBuffers(1, &s_VboHandle);
	glGenBuffers(1, &s_ElementsHandle);

	CreateFontsTexture();

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);

	return true;
}

void InterfaceManager::CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.
																// Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &s_FontTexture);
	glBindTexture(GL_TEXTURE_2D,s_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)s_FontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);
}


void InterfaceManager::Shutdown()
{
	// Destroy SDL mouse cursors
	for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
		SDL_FreeCursor(s_MouseCursors[cursor_n]);
	memset(s_MouseCursors, 0, sizeof(s_MouseCursors));

	// Destroy last known clipboard data
	if (s_ClipboardTextData)
		SDL_free(s_ClipboardTextData);

	// Destroy OpenGL objects
	InvalidateDeviceObjects();

	ImGui::DestroyContext();
}

bool InterfaceManager::Render()
{
	if (!ImGui::GetDrawData()) { return false; }
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return false;
	ImGui::GetDrawData()->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
	{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
	{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
	{ -1.0f,                  1.0f,                   0.0f, 1.0f },
	};
	glUseProgram(s_ShaderHandle);
	glUniform1i(s_AttribLocationTex, 0);
	glUniformMatrix4fv(s_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
	if (glBindSampler) glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.

											// Recreate the VAO every time 
											// (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
	GLuint vao_handle = 0;
	glGenVertexArrays(1, &vao_handle);
	glBindVertexArray(vao_handle);
	glBindBuffer(GL_ARRAY_BUFFER, s_VboHandle);
	glEnableVertexAttribArray(s_AttribLocationPosition);
	glEnableVertexAttribArray(s_AttribLocationUV);
	glEnableVertexAttribArray(s_AttribLocationColor);
	glVertexAttribPointer(s_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(s_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(s_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

	// Draw
	for (int n = 0; n < ImGui::GetDrawData()->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = ImGui::GetDrawData()->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, s_VboHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ElementsHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}
	glDeleteVertexArrays(1, &vao_handle);

	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	if (glBindSampler) glBindSampler(0, last_sampler);
	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);

	return true;
}

void InterfaceManager::CreateButton(const std::string& tag, const std::string& description, float width, float height)
{
	m_Buttons.try_emplace(tag, std::make_pair(description, ImVec2(width, height)));
}

bool InterfaceManager::IsButtonClicked(const std::string& tag)
{
	const auto& button = m_Buttons.find(tag);
	if (button != m_Buttons.end())
	{
		if (ImGui::Button(button->second.first.c_str(), button->second.second))
		{
			Audio::Instance()->PlayAudio("Click", 1.0f);
			return true;
		}
		else { return false; }
	}

	return false;
}

void InterfaceManager::BeginWindow(const std::string& title)
{
	float width = Screen::Instance()->GetWidth() / 3;
	float height = Screen::Instance()->GetHeight() / 2;
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(Screen::Instance()->GetWidth() - width, height), ImGuiCond_Once);
	ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoResize);
}


void InterfaceManager::CreateDragVec3(const std::string& label, const std::string& tooltip, glm::vec3& data)
{
	ImGui::Text(label.c_str());
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip(tooltip.c_str()); }
	ImGui::DragFloat((std::string("##X") + label).c_str(), &data.x, 1.0f, 0.0f, 0.0f, "X: %.2f");
	ImGui::DragFloat((std::string("##Y") + label).c_str(), &data.y, 1.0f, 0.0f, 0.0f, "Y: %.2f");
	ImGui::DragFloat((std::string("##Z") + label).c_str(), &data.z, 1.0f, 0.0f, 0.0f, "Z: %.2f");
}

void InterfaceManager::Popup(const std::string& title, const std::string& message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, title.c_str(), message.c_str(), nullptr);
}

Uint64       InterfaceManager::s_Time = 0;
bool         InterfaceManager::s_MousePressed[3] = { false, false, false };
SDL_Cursor*  InterfaceManager::s_MouseCursors[ImGuiMouseCursor_COUNT] = { nullptr };
char*        InterfaceManager::s_ClipboardTextData = nullptr;
char         InterfaceManager::s_GlslVersion[32] = "#version 150";
GLuint		InterfaceManager::s_FontTexture = 0;
int          InterfaceManager::s_ShaderHandle = 0, InterfaceManager::s_VertHandle =0, InterfaceManager::s_FragHandle = 0;
int         InterfaceManager::s_AttribLocationTex = 0, InterfaceManager::s_AttribLocationProjMtx = 0;
int          InterfaceManager::s_AttribLocationPosition = 0, InterfaceManager::s_AttribLocationUV = 0, InterfaceManager::s_AttribLocationColor = 0;
unsigned int InterfaceManager::s_VboHandle = 0, InterfaceManager::s_ElementsHandle = 0;