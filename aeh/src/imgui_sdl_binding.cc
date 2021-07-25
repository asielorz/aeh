#if defined AEH_WITH_SDL2 && defined AEH_WITH_IMGUI

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4121) // alignment of a member was sensitive to packing
#endif

#include "imgui_sdl_binding.hh"
#include <imgui.h>

// SDL,GL3W
#include <SDL.h>
#include <SDL_syswm.h>
#include <gl/gl_core_4_5.hh>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.

namespace aeh::ImGui_SDL
{

	// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
	// Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL engine that doesn't do so. 
	// If text or lines are blurry when integrating ImGui in your engine: in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
	void RenderDrawData(Renderer const & renderer, ImDrawData * draw_data)
	{
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		ImGuiIO & io = ImGui::GetIO();
		int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
		if (fb_width == 0 || fb_height == 0)
			return;
		draw_data->ScaleClipRects(io.DisplayFramebufferScale);

		// Backup GL state
		GLenum last_active_texture; gl::GetIntegerv(gl::ACTIVE_TEXTURE, (GLint *)&last_active_texture);
		gl::ActiveTexture(gl::TEXTURE0);
		GLint last_program; gl::GetIntegerv(gl::CURRENT_PROGRAM, &last_program);
		GLint last_texture; gl::GetIntegerv(gl::TEXTURE_BINDING_2D, &last_texture);
		GLint last_sampler; gl::GetIntegerv(gl::SAMPLER_BINDING, &last_sampler);
		GLint last_array_buffer; gl::GetIntegerv(gl::ARRAY_BUFFER_BINDING, &last_array_buffer);
		GLint last_element_array_buffer; gl::GetIntegerv(gl::ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
		GLint last_vertex_array; gl::GetIntegerv(gl::VERTEX_ARRAY_BINDING, &last_vertex_array);
		GLint last_polygon_mode[2]; gl::GetIntegerv(gl::POLYGON_MODE, last_polygon_mode);
		GLint last_viewport[4]; gl::GetIntegerv(gl::VIEWPORT, last_viewport);
		GLint last_scissor_box[4]; gl::GetIntegerv(gl::SCISSOR_BOX, last_scissor_box);
		GLenum last_blend_src_rgb; gl::GetIntegerv(gl::BLEND_SRC_RGB, (GLint *)&last_blend_src_rgb);
		GLenum last_blend_dst_rgb; gl::GetIntegerv(gl::BLEND_DST_RGB, (GLint *)&last_blend_dst_rgb);
		GLenum last_blend_src_alpha; gl::GetIntegerv(gl::BLEND_SRC_ALPHA, (GLint *)&last_blend_src_alpha);
		GLenum last_blend_dst_alpha; gl::GetIntegerv(gl::BLEND_DST_ALPHA, (GLint *)&last_blend_dst_alpha);
		GLenum last_blend_equation_rgb; gl::GetIntegerv(gl::BLEND_EQUATION_RGB, (GLint *)&last_blend_equation_rgb);
		GLenum last_blend_equation_alpha; gl::GetIntegerv(gl::BLEND_EQUATION_ALPHA, (GLint *)&last_blend_equation_alpha);
		GLboolean last_enable_blend = gl::IsEnabled(gl::BLEND);
		GLboolean last_enable_cull_face = gl::IsEnabled(gl::CULL_FACE);
		GLboolean last_enable_depth_test = gl::IsEnabled(gl::DEPTH_TEST);
		GLboolean last_enable_scissor_test = gl::IsEnabled(gl::SCISSOR_TEST);

		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
		gl::Enable(gl::BLEND);
		gl::BlendEquation(gl::FUNC_ADD);
		gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
		gl::Disable(gl::CULL_FACE);
		gl::Disable(gl::DEPTH_TEST);
		gl::Enable(gl::SCISSOR_TEST);
		gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);

		// Setup viewport, orthographic projection matrix
		gl::Viewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
		const float ortho_projection[4][4] =
		{
			{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
			{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
			{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
			{-1.0f,                  1.0f,                   0.0f, 1.0f },
		};
		gl::UseProgram(renderer.ShaderHandle);
		gl::Uniform1i(renderer.AttribLocationTex, 0);
		gl::UniformMatrix4fv(renderer.AttribLocationProjMtx, 1, gl::FALSE_, &ortho_projection[0][0]);
		gl::BindSampler(0, 0); // Rely on combined texture/sampler state.

		// Recreate the VAO every time 
		// (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
		GLuint vao_handle = 0;
		gl::GenVertexArrays(1, &vao_handle);
		gl::BindVertexArray(vao_handle);
		gl::BindBuffer(gl::ARRAY_BUFFER, renderer.VboHandle);
		gl::EnableVertexAttribArray(renderer.AttribLocationPosition);
		gl::EnableVertexAttribArray(renderer.AttribLocationUV);
		gl::EnableVertexAttribArray(renderer.AttribLocationColor);
		gl::VertexAttribPointer(renderer.AttribLocationPosition, 2, gl::FLOAT, gl::FALSE_, sizeof(ImDrawVert), (GLvoid *)IM_OFFSETOF(ImDrawVert, pos));
		gl::VertexAttribPointer(renderer.AttribLocationUV, 2, gl::FLOAT, gl::FALSE_, sizeof(ImDrawVert), (GLvoid *)IM_OFFSETOF(ImDrawVert, uv));
		gl::VertexAttribPointer(renderer.AttribLocationColor, 4, gl::UNSIGNED_BYTE, gl::TRUE_, sizeof(ImDrawVert), (GLvoid *)IM_OFFSETOF(ImDrawVert, col));

		// Draw
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList * cmd_list = draw_data->CmdLists[n];
			const ImDrawIdx * idx_buffer_offset = 0;

			gl::BindBuffer(gl::ARRAY_BUFFER, renderer.VboHandle);
			gl::BufferData(gl::ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid *)cmd_list->VtxBuffer.Data, gl::STREAM_DRAW);

			gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, renderer.ElementsHandle);
			gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid *)cmd_list->IdxBuffer.Data, gl::STREAM_DRAW);

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd * pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					gl::BindTexture(gl::TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					gl::Scissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
					gl::DrawElements(gl::TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? gl::UNSIGNED_SHORT : gl::UNSIGNED_INT, idx_buffer_offset);
				}
				idx_buffer_offset += pcmd->ElemCount;
			}
		}
		gl::DeleteVertexArrays(1, &vao_handle);

		// Restore modified GL state
		gl::UseProgram(last_program);
		gl::BindTexture(gl::TEXTURE_2D, last_texture);
		gl::BindSampler(0, last_sampler);
		gl::ActiveTexture(last_active_texture);
		gl::BindVertexArray(last_vertex_array);
		gl::BindBuffer(gl::ARRAY_BUFFER, last_array_buffer);
		gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
		gl::BlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
		gl::BlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
		if (last_enable_blend) gl::Enable(gl::BLEND); else gl::Disable(gl::BLEND);
		if (last_enable_cull_face) gl::Enable(gl::CULL_FACE); else gl::Disable(gl::CULL_FACE);
		if (last_enable_depth_test) gl::Enable(gl::DEPTH_TEST); else gl::Disable(gl::DEPTH_TEST);
		if (last_enable_scissor_test) gl::Enable(gl::SCISSOR_TEST); else gl::Disable(gl::SCISSOR_TEST);
		gl::PolygonMode(gl::FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
		gl::Viewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
		gl::Scissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
	}

	static const char * GetClipboardText(void *)
	{
		return SDL_GetClipboardText();
	}

	static void SetClipboardText(void *, const char * text)
	{
		SDL_SetClipboardText(text);
	}

	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	bool ProcessEvent(Renderer & renderer, SDL_Event const * event)
	{
		ImGuiIO & io = ImGui::GetIO();
		switch (event->type)
		{
		case SDL_MOUSEWHEEL:
		{
			if (event->wheel.x > 0) io.MouseWheelH += 1;
			if (event->wheel.x < 0) io.MouseWheelH -= 1;
			if (event->wheel.y > 0) io.MouseWheel += 1;
			if (event->wheel.y < 0) io.MouseWheel -= 1;
			return true;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			if (event->button.button == SDL_BUTTON_LEFT) renderer.MousePressed[0] = true;
			if (event->button.button == SDL_BUTTON_RIGHT) renderer.MousePressed[1] = true;
			if (event->button.button == SDL_BUTTON_MIDDLE) renderer.MousePressed[2] = true;
			return true;
		}
		case SDL_TEXTINPUT:
		{
			io.AddInputCharactersUTF8(event->text.text);
			return true;
		}
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			int key = event->key.keysym.scancode;
			IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
			io.KeysDown[key] = (event->type == SDL_KEYDOWN);
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
			io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			return true;
		}
		}
		return false;
	}

	void CreateFontsTexture(Renderer & renderer)
	{
		// Build texture atlas
		ImGuiIO & io = ImGui::GetIO();
		unsigned char * pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

		// Upload texture to graphics system
		GLint last_texture;
		gl::GetIntegerv(gl::TEXTURE_BINDING_2D, &last_texture);
		gl::GenTextures(1, &renderer.FontTexture);
		gl::BindTexture(gl::TEXTURE_2D, renderer.FontTexture);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
		gl::PixelStorei(gl::UNPACK_ROW_LENGTH, 0);
		gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGBA, width, height, 0, gl::RGBA, gl::UNSIGNED_BYTE, pixels);

		// Store our identifier
		io.Fonts->TexID = (void *)(intptr_t)renderer.FontTexture;

		// Restore state
		gl::BindTexture(gl::TEXTURE_2D, last_texture);
	}

	void RefreshFontsTexture(Renderer & renderer)
	{
		if (!HasDeviceObjects(renderer))
		{
			// No device objects them. Create them.
			CreateDeviceObjects(renderer);
		}
		else
		{
			// Already has device objecs. Just reset the font texture.
			InvalidateFontsTexture(renderer);
			CreateFontsTexture(renderer);
		}
	}

	bool CreateDeviceObjects(Renderer & renderer)
	{
		// Backup GL state
		GLint last_texture, last_array_buffer, last_vertex_array;
		gl::GetIntegerv(gl::TEXTURE_BINDING_2D, &last_texture);
		gl::GetIntegerv(gl::ARRAY_BUFFER_BINDING, &last_array_buffer);
		gl::GetIntegerv(gl::VERTEX_ARRAY_BINDING, &last_vertex_array);

		const GLchar * vertex_shader =
			"uniform mat4 ProjMtx;\n"
			"in vec2 Position;\n"
			"in vec2 UV;\n"
			"in vec4 Color;\n"
			"out vec2 FraUV;\n"
			"out vec4 FraColor;\n"
			"void main()\n"
			"{\n"
			"	FraUV = UV;\n"
			"	FraColor = Color;\n"
			"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
			"}\n";

		const GLchar * fragment_shader =
			"uniform sampler2D Texture;\n"
			"in vec2 FraUV;\n"
			"in vec4 FraColor;\n"
			"out vec4 Out_Color;\n"
			"void main()\n"
			"{\n"
			"	Out_Color = FraColor * texture( Texture, FraUV.st);\n"
			"}\n";

		GLchar const * vertex_shader_with_version[2] = { renderer.GlslVersion, vertex_shader };
		GLchar const * fragment_shader_with_version[2] = { renderer.GlslVersion, fragment_shader };

		renderer.ShaderHandle = gl::CreateProgram();
		renderer.VertHandle = gl::CreateShader(gl::VERTEX_SHADER);
		renderer.FragHandle = gl::CreateShader(gl::FRAGMENT_SHADER);
		gl::ShaderSource(renderer.VertHandle, 2, vertex_shader_with_version, NULL);
		gl::ShaderSource(renderer.FragHandle, 2, fragment_shader_with_version, NULL);
		gl::CompileShader(renderer.VertHandle);
		gl::CompileShader(renderer.FragHandle);
		gl::AttachShader(renderer.ShaderHandle, renderer.VertHandle);
		gl::AttachShader(renderer.ShaderHandle, renderer.FragHandle);
		gl::LinkProgram(renderer.ShaderHandle);

		renderer.AttribLocationTex = gl::GetUniformLocation(renderer.ShaderHandle, "Texture");
		renderer.AttribLocationProjMtx = gl::GetUniformLocation(renderer.ShaderHandle, "ProjMtx");
		renderer.AttribLocationPosition = gl::GetAttribLocation(renderer.ShaderHandle, "Position");
		renderer.AttribLocationUV = gl::GetAttribLocation(renderer.ShaderHandle, "UV");
		renderer.AttribLocationColor = gl::GetAttribLocation(renderer.ShaderHandle, "Color");

		gl::GenBuffers(1, &renderer.VboHandle);
		gl::GenBuffers(1, &renderer.ElementsHandle);

		ImGui_SDL::CreateFontsTexture(renderer);

		// Restore modified GL state
		gl::BindTexture(gl::TEXTURE_2D, last_texture);
		gl::BindBuffer(gl::ARRAY_BUFFER, last_array_buffer);
		gl::BindVertexArray(last_vertex_array);

		return true;
	}

	void InvalidateFontsTexture(Renderer & renderer)
	{
		if (renderer.FontTexture)
		{
			gl::DeleteTextures(1, &renderer.FontTexture);
			ImGui::GetIO().Fonts->TexID = 0;
			renderer.FontTexture = 0;
		}
	}

	void InvalidateDeviceObjects(Renderer & renderer)
	{
		if (renderer.VboHandle) gl::DeleteBuffers(1, &renderer.VboHandle);
		if (renderer.ElementsHandle) gl::DeleteBuffers(1, &renderer.ElementsHandle);
		renderer.VboHandle = renderer.ElementsHandle = 0;

		if (renderer.ShaderHandle && renderer.VertHandle) gl::DetachShader(renderer.ShaderHandle, renderer.VertHandle);
		if (renderer.VertHandle) gl::DeleteShader(renderer.VertHandle);
		renderer.VertHandle = 0;

		if (renderer.ShaderHandle && renderer.FragHandle) gl::DetachShader(renderer.ShaderHandle, renderer.FragHandle);
		if (renderer.FragHandle) gl::DeleteShader(renderer.FragHandle);
		renderer.FragHandle = 0;

		if (renderer.ShaderHandle) gl::DeleteProgram(renderer.ShaderHandle);
		renderer.ShaderHandle = 0;

		InvalidateFontsTexture(renderer);
	}

	bool HasDeviceObjects(Renderer const & renderer)
	{
		return renderer.ShaderHandle != 0;
	}

	bool Init(Renderer & renderer, SDL_Window * window, const char * glsl_version)
	{
		// Store GL version string so we can refer to it later in case we recreate shaders.
		if (glsl_version == NULL)
			glsl_version = "#version 150";
		IM_ASSERT((int)strlen(glsl_version) + 2 < IM_ARRAYSIZE(renderer.GlslVersion));
		strcpy(renderer.GlslVersion, glsl_version);
		strcat(renderer.GlslVersion, "\n");

		// Setup back-end capabilities flags
		ImGuiIO & io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;   // We can honor GetMouseCursor() values (optional)

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
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

		io.SetClipboardTextFn = ImGui_SDL::SetClipboardText;
		io.GetClipboardTextFn = ImGui_SDL::GetClipboardText;
		io.ClipboardUserData = NULL;

		renderer.MouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		renderer.MouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
		renderer.MouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		renderer.MouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
		renderer.MouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
		renderer.MouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
		renderer.MouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);

#ifdef _WIN32
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window, &wmInfo);
		io.ImeWindowHandle = wmInfo.info.win.window;
#else
		(void)window;
#endif

		return true;
	}

	void Shutdown(Renderer & renderer)
	{
		// Destroy SDL mouse cursors
		for (SDL_Cursor * const cursor : renderer.MouseCursors)
			SDL_FreeCursor(cursor);
		memset(renderer.MouseCursors, 0, sizeof(renderer.MouseCursors));

		// Destroy OpenGL objects
		ImGui_SDL::InvalidateDeviceObjects(renderer);
	}

	void NewFrame(Renderer & renderer, SDL_Window * window)
	{
		if (!HasDeviceObjects(renderer))
			ImGui_SDL::CreateDeviceObjects(renderer);

		ImGuiIO & io = ImGui::GetIO();

		// Setup display size (every frame to accommodate for window resizing)
		int w, h;
		int display_w, display_h;
		SDL_GetWindowSize(window, &w, &h);
		SDL_GL_GetDrawableSize(window, &display_w, &display_h);
		io.DisplaySize = ImVec2((float)w, (float)h);
		io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

		// Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
		static Uint64 frequency = SDL_GetPerformanceFrequency();
		Uint64 current_time = SDL_GetPerformanceCounter();
		io.DeltaTime = renderer.Time > 0 ? (float)((double)(current_time - renderer.Time) / frequency) : (float)(1.0f / 60.0f);
		renderer.Time = current_time;

		// Setup mouse inputs (we already got mouse wheel, keyboard keys & characters from our event handler)
		int mx, my;
		Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		io.MouseDown[0] = renderer.MousePressed[0] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		io.MouseDown[1] = renderer.MousePressed[1] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
		io.MouseDown[2] = renderer.MousePressed[2] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
		renderer.MousePressed[0] = renderer.MousePressed[1] = renderer.MousePressed[2] = false;

		// We need to use SDL_CaptureMouse() to easily retrieve mouse coordinates outside of the client area. This is only supported from SDL 2.0.4 (released Jan 2016)
#if (SDL_MAJOR_VERSION >= 2) && (SDL_MINOR_VERSION >= 0) && (SDL_PATCHLEVEL >= 4)   
		if ((SDL_GetWindowFlags(window) & (SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_MOUSE_CAPTURE)) != 0)
			io.MousePos = ImVec2((float)mx, (float)my);
		bool any_mouse_button_down = false;
		for (int n = 0; n < IM_ARRAYSIZE(io.MouseDown); n++)
			any_mouse_button_down |= io.MouseDown[n];
		if (any_mouse_button_down && (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_CAPTURE) == 0)
			SDL_CaptureMouse(SDL_TRUE);
		if (!any_mouse_button_down && (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_CAPTURE) != 0)
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
				SDL_SetCursor(renderer.MouseCursors[cursor] ? renderer.MouseCursors[cursor] : renderer.MouseCursors[ImGuiMouseCursor_Arrow]);
				SDL_ShowCursor(1);
			}
		}

		// Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
		ImGui::NewFrame();
	}

} // namespace aeh::ImGui_SDL

#endif // AEH_WITH_SDL2 && AEH_WITH_IMGUI
