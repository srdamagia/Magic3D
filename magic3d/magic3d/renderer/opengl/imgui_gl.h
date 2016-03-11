/******************************************************************************
 @Copyright    Copyright (C) 2008 - 2016 by MagicTech.

 @Platform     ANSI compatible
******************************************************************************/
/*
Magic3D Engine
Copyright (c) 2008-2016
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

#ifndef IMGUI_GL_H
#define IMGUI_GL_H

#include <magic3d/input_define.h>

namespace Magic3D {

IMGUI_API bool        ImGui_GL_Init();
IMGUI_API void        ImGui_GL_Shutdown();
IMGUI_API void        ImGui_GL_NewFrame(float ticks);
IMGUI_API bool        ImGui_GL_ProcessEvent(EVENT event, int x, int y, int z, int button);
IMGUI_API void        ImGui_GL_RenderDrawLists(ImDrawData* draw_data);
IMGUI_API const char* ImGui_GL_GetClipboardText();
IMGUI_API void        ImGui_GL_SetClipboardText(const char* text);
IMGUI_API void        ImGui_GL_CreateFontsTexture();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void ImGui_GL_InvalidateDeviceObjects();
IMGUI_API bool ImGui_GL_CreateDeviceObjects();

}
#endif //IMGUI_GL_H
