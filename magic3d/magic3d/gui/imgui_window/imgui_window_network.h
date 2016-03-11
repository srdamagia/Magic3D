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

#ifndef MAGIC3D_GUI_NETWORK_H
#define MAGIC3D_GUI_NETWORK_H

#include <imgui.h>

namespace Magic3D
{

int textEditCallbackStub(ImGuiTextEditCallbackData* data);
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))
class GUINetwork
{
private:
    char                  inputBuf[256];
    char                  nick[256];
    char                  ip[256];
    ImVector<char*>       items;
    bool                  scrollToBottom;
    ImVector<char*>       history;
    int                   historyPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> commands;

    int port;
    bool nickError;

public:
    GUINetwork();
    ~GUINetwork();

    int   stricmp(const char* str1, const char* str2);
    int   strnicmp(const char* str1, const char* str2, int n);
    char* strdup(const char *str);

    void clearLog();
    void addLog(const char* fmt, ...);

    void draw(const char* title, bool* opened);

    void execCommand(const char* command_line);    

    int textEditCallback(ImGuiTextEditCallbackData* data);
};
}
#endif //MAGIC3D_GUI_NETWORK_H
