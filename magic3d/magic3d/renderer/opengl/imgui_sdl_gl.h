/******************************************************************************
 @Copyright    Copyright (C) 2008 - 2015 by MagicTech.

 @Platform     ANSI compatible
******************************************************************************/
/*
Magic3D Engine
Copyright (c) 2008-2015
Thiago C. Moraes
http://www.magictech.com.br

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef IMGUI_SDL_GL_H
#define IMGUI_SDL_GL_H

struct SDL_Window;
typedef union SDL_Event SDL_Event;

IMGUI_API bool        ImGui_SDL_GL_Init(SDL_Window *window);
IMGUI_API void        ImGui_SDL_GL_Shutdown();
IMGUI_API void        ImGui_SDL_GL_NewFrame();
IMGUI_API bool        ImGui_SDL_GL_ProcessEvent(SDL_Event* event);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_SDL_GL_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_SDL_GL_CreateDeviceObjects();

#endif //IMGUI_SDL_GL_H
