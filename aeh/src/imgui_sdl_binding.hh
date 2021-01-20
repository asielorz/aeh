#pragma once

#if defined AEH_WITH_SDL2 && defined AEH_WITH_IMGUI

struct SDL_Window;
struct SDL_Cursor;
typedef union SDL_Event SDL_Event;
struct ImDrawData;
#include <cstdint>

namespace aeh::ImGui_SDL
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

	bool Init(Renderer & renderer, SDL_Window * window, const char * glsl_version = nullptr);
	void Shutdown(Renderer & renderer);
	void NewFrame(Renderer & renderer, SDL_Window * window);
	void RenderDrawData(Renderer const & renderer, ImDrawData * draw_data);
	bool ProcessEvent(Renderer & renderer, SDL_Event const * event);

	// Use if you want to reset your rendering device without losing ImGui state.
	void InvalidateDeviceObjects(Renderer & renderer);
	bool CreateDeviceObjects(Renderer & renderer);
	bool HasDeviceObjects(Renderer const & renderer);

	// Substep of Invalidate/CreateDeviceObjects that invalidate and create the fonts texture. Used to add fonts at runtime.
	void InvalidateFontsTexture(Renderer & renderer);
	void CreateFontsTexture(Renderer & renderer);
	void RefreshFontsTexture(Renderer & renderer);

} // namespace aeh::ImGui_SDL

#endif // AEH_WITH_SDL2 && AEH_WITH_IMGUI
