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

#if !defined(MAGIC3D_OES)
#include <magic3d/magic3d.h>
#include <magic3d/renderer/window_sdl.h>
#include <magic3d/renderer/opengl/imgui_gl.h>

Magic3D::WindowSDL* Magic3D::WindowSDL::instance = NULL;

//******************************************************************************
Magic3D::WindowSDL::WindowSDL()
{
    window = NULL;
    title  = Magic3D::getInstance()->getConfiguration().TITLE;
    width  = Magic3D::getInstance()->getConfiguration().WINDOW_WIDTH;
    height = Magic3D::getInstance()->getConfiguration().WINDOW_HEIGHT;
    cursorX = 0;
    cursorY = 0;
    orientation = eWINDOW_ORIENTATION_NONE;

    bits   = 24;

    fullscreen = Magic3D::getInstance()->getConfiguration().FULLSCREEN;
    cursor     = Magic3D::getInstance()->getConfiguration().CURSOR;
    active     = false;
    needUpdateView = false;    
}

Magic3D::WindowSDL::~WindowSDL()
{
    instance = NULL;
}

bool Magic3D::WindowSDL::start()
{
    bool result = create();

    if (result)
    {
        Log::log(eLOG_SUCCESS, "Window sucessfully started.");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Window not started.");
    }

    return result;
}

bool Magic3D::WindowSDL::finish()
{
    bool result = true;

    SDL_ShowCursor(true);
    ImGui_GL_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    Log::log(eLOG_SUCCESS, "Window sucessfully finished.");

    return result;
}

bool Magic3D::WindowSDL::render()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        EVENT evt = eEVENT_KEYBOARD_PRESSED;
        int button  = 0;
        int x = 0;
        int y = 0;
        int z = 0;
        switch (event.type)
        {
            case SDL_MOUSEWHEEL:
            {
                y = event.wheel.y;
                evt = eEVENT_MOUSE_WHEEL;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                x = event.button.x;
                y = event.button.y;
                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT: button = 0; break;
                    case SDL_BUTTON_RIGHT: button = 1; break;
                    case SDL_BUTTON_MIDDLE: button = 2; break;
                }
                evt = eEVENT_MOUSE_DOWN;
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                button = event.key.keysym.sym & ~SDLK_SCANCODE_MASK;
                x = (SDL_GetModState() & KMOD_SHIFT) != 0;
                y = (SDL_GetModState() & KMOD_CTRL) != 0;
                z = (SDL_GetModState() & KMOD_ALT) != 0;
                evt = event.type == SDL_KEYDOWN ? eEVENT_KEYBOARD_DOWN : eEVENT_KEYBOARD_UP;

                if (z && button == SDLK_RETURN)
                {
                    setFullScreen(!fullscreen);
                }
                break;
            }
            case SDL_TEXTINPUT:
            {
                ImGuiIO& io = ImGui::GetIO();
                io.AddInputCharactersUTF8(event.text.text);
                break;
            }

            default: break;
        }

        ImGui_GL_ProcessEvent(evt, x, y, z, button);

        if (!ImGui::IsAnyItemActive() && !ImGui::IsAnyItemHovered() && !ImGui::IsMouseHoveringAnyWindow())
        {
            switch (event.type)
            {
                case SDL_MOUSEMOTION:
                {
                    Input::getInstance()->dispatchEvent(eINPUT_MOUSE, eEVENT_MOUSE_MOVE, event.button.x, event.button.y, event.button.button);
                    Renderer::getInstance()->getWindow()->setCursorPosition(event.button.x, event.button.y);
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    Input::getInstance()->dispatchEvent(eINPUT_MOUSE, eEVENT_MOUSE_DOWN, event.button.x, event.button.y, event.button.button);
                    break;
                }

                case SDL_MOUSEBUTTONUP:
                {
                    Input::getInstance()->dispatchEvent(eINPUT_MOUSE, eEVENT_MOUSE_UP, event.button.x, event.button.y, event.button.button);
                    break;
                }

                case SDL_MOUSEWHEEL:
                {
                    Input::getInstance()->dispatchEvent(eINPUT_MOUSE, eEVENT_MOUSE_WHEEL, event.wheel.x, event.wheel.y, 0);
                    break;
                }

                case SDL_FINGERMOTION:
                {
                    Input::getInstance()->dispatchEvent(eINPUT_TOUCH, eEVENT_TOUCH_MOVE, event.tfinger.x, event.tfinger.y, event.tfinger.fingerId);
                    break;
                }

                case SDL_FINGERDOWN:
                {
                    Input::getInstance()->dispatchEvent(eINPUT_TOUCH, eEVENT_TOUCH_DOWN, event.tfinger.x, event.tfinger.y, event.tfinger.fingerId);
                    break;
                }

                case SDL_FINGERUP:
                {
                    Input::getInstance()->dispatchEvent(eINPUT_TOUCH, eEVENT_TOUCH_UP, event.tfinger.x, event.tfinger.y, event.tfinger.fingerId);
                    break;
                }
            }
        }

        if (!ImGui::IsAnyItemActive())
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                {
                    Input::getInstance()->dispatchEvent(eINPUT_KEYBOARD, eEVENT_KEYBOARD_DOWN, event.key.keysym.sym & ~SDLK_SCANCODE_MASK);
                    break;
                }

                case SDL_KEYUP:
                {
                    Input::getInstance()->dispatchEvent(eINPUT_KEYBOARD, eEVENT_KEYBOARD_UP, event.key.keysym.sym & ~SDLK_SCANCODE_MASK);
                    break;
                }
            }
        }

        if (event.type == SDL_QUIT)
        {
            getInstance()->setActive(false);
        }

        if (event.type == SDL_WINDOWEVENT)
        {
            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    Renderer::getInstance()->resizeWindow(event.window.data1, event.window.data2);
                    break;
                }
            }
        }
    }

    if (!Scene::getInstance()->isLoading())
    {
        ImGui_GL_NewFrame(SDL_GetTicks());

        Network::getInstance()->render();

        ImGui::Render();
    }
    SDL_GL_SwapWindow(window);

    return true;
}

void* Magic3D::WindowSDL::getWindowHandle()
{
#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    return wmInfo.info.win.window;
#else
    return NULL;
#endif
}

void Magic3D::WindowSDL::showCursor(bool show)
{
    cursor = show;
    if (cursor)
    {
        ImGui::GetIO().MouseDrawCursor = true;
        SDL_ShowCursor(false);
    }
    else
    {
        ImGui::GetIO().MouseDrawCursor = false;
        SDL_ShowCursor(false);
    }
}

void Magic3D::WindowSDL::setClipboardText(const char* text)
{
    SDL_SetClipboardText(text);
}

const char* Magic3D::WindowSDL::getClipboardText()
{
    return SDL_GetClipboardText();
}

void Magic3D::WindowSDL::grabInput(bool grabbed)
{
    SDL_SetWindowGrab(window, grabbed ? SDL_TRUE : SDL_FALSE);
}

bool Magic3D::WindowSDL::hasGrabbedInput()
{
    return SDL_GetWindowGrab(window);
}

bool Magic3D::WindowSDL::hasInputFocus()
{
    return SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS;
}

bool Magic3D::WindowSDL::hasMouseFocus()
{
    return SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS;
}

void Magic3D::WindowSDL::mapGUIKeys(void* gui)
{
    if (gui)
    {
    }
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB; // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDLK_a;
    io.KeyMap[ImGuiKey_C] = SDLK_c;
    io.KeyMap[ImGuiKey_V] = SDLK_v;
    io.KeyMap[ImGuiKey_X] = SDLK_x;
    io.KeyMap[ImGuiKey_Y] = SDLK_y;
    io.KeyMap[ImGuiKey_Z] = SDLK_z;
}

int Magic3D::WindowSDL::getMouseState(int* x, int* y, bool* left, bool* right, bool* middle)
{
    Uint32 flags = SDL_GetMouseState(x, y);
    *left = (flags & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    *right = (flags & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    *middle = (flags & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;

    return flags;
}

void Magic3D::WindowSDL::setFullScreen(bool fullscreen)
{
    this->fullscreen = fullscreen;
    SDL_SetWindowFullscreen(window, this->fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
}

Magic3D::WindowSDL* Magic3D::WindowSDL::getInstance()
{
    if (!instance)
    {
        instance = new WindowSDL();
    }

    return instance;
}

bool Magic3D::WindowSDL::create()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Log::logFormat(eLOG_FAILURE, "SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Setup window
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, bits);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (fullscreen)
    {
        flags = flags | SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);    
    glcontext = SDL_GL_CreateContext(window);

    if (SDL_GL_SetSwapInterval(Magic3D::getInstance()->getConfiguration().VSYNC ? 1 : 0))
    {
        Log::logFormat(eLOG_FAILURE, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }

    // Setup ImGui binding
    ImGui_GL_Init();

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    showCursor(cursor);
    setActive(true);

    Renderer::getInstance()->initialize();

    Renderer::getInstance()->resizeWindow(width, height);

    return true;
}

#endif
