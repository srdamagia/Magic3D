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

#ifndef MAGIC3D_RENDERER_WINDOW_X_H
#define MAGIC3D_RENDERER_WINDOW_X_H

#if defined(__X__)
#include <X11/Xlib.h>
#include <magic3d/renderer/renderer.h>

namespace Magic3D
{

class SimpleGLXWindow : public Window
{
public:
    WindowX(); //default constructor
    virtual ~WindowX();

    bool create(int width, int height, int bpp, bool fullscreen);
    void destroy();
    void processEvents();
    void attachExample(Example* example);

    bool isRunning(); //Is the window running?

    void swapBuffers() { glXSwapBuffers(m_display, m_XWindow); }

    float getElapsedSeconds();

    KeyboardInterface* getKeyboard() const { return m_keyboard; }
    MouseInterface* getMouse() const { return m_mouse; }
private:
    Example* m_example; //A link to the example program
    bool m_isRunning; //Is the window still running?


    Example* getAttachedExample() { return m_example; }

    unsigned int m_lastTime;

    Display* m_display;
    Window m_XWindow;
    GLXContext m_glContext;
    XF86VidModeModeInfo m_XF86DeskMode;
    XSetWindowAttributes m_XSetAttr;
    int m_screenID;

    bool m_isFullscreen;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_bpp;

    bool m_GL3Supported;

    KeyboardInterface* m_keyboard;
    MouseInterface* m_mouse;
};

}
#endif

#endif // MAGIC3D_RENDERER_WINDOW_X_H
