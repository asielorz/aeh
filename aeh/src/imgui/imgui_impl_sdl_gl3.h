// ImGui SDL2 binding with OpenGL3
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

// Implemented features:
//  [X] User texture binding. Cast 'GLuint' OpenGL texture identifier as void*/ImTextureID. Read the FAQ about ImTextureID in imgui.cpp.
// Missing features:
//  [ ] SDL2 handling of IME under Windows appears to be broken and it explicitly disable the regular Windows IME. You can restore Windows IME by compiling SDL with SDL_DISABLE_WINDOWS_IME.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#pragma once
struct SDL_Window;
struct SDL_Cursor;
typedef union SDL_Event SDL_Event;
struct ImDrawData;
#include <cstdint>

namespace ImGui
{
	namespace SDL
	{
		struct Renderer
		{
			// SDL data
			uint64_t     Time = 0;
			bool         MousePressed[3] = {false, false, false};
			SDL_Cursor * MouseCursors[7] = {0};

			// OpenGL data
			char         GlslVersion[32] = "#version 150";
			unsigned     FontTexture = 0;
			int          ShaderHandle = 0, VertHandle = 0, FragHandle = 0;
			int          AttribLocationTex = 0, AttribLocationProjMtx = 0;
			int          AttribLocationPosition = 0, AttribLocationUV = 0, AttribLocationColor = 0;
			unsigned int VboHandle = 0, ElementsHandle = 0;
		};

		IMGUI_API bool        Init(Renderer & renderer, SDL_Window * window, const char * glsl_version = NULL);
		IMGUI_API void        Shutdown(Renderer & renderer);
		IMGUI_API void        NewFrame(Renderer & renderer, SDL_Window * window);
		IMGUI_API void        RenderDrawData(Renderer const & renderer, ImDrawData * draw_data);
		IMGUI_API bool        ProcessEvent(Renderer & renderer, SDL_Event const * event);

		// Use if you want to reset your rendering device without losing ImGui state.
		IMGUI_API void        InvalidateDeviceObjects(Renderer & renderer);
		IMGUI_API bool        CreateDeviceObjects(Renderer & renderer);
	}
}
