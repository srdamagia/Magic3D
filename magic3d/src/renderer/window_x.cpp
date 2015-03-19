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

#if defined(__X__)
#include <magic3d/magic3d.h>
#include <magic3d/renderer/window_x.h>

void create()
{
m_display = XOpenDisplay(0);  //Open default display
if (m_display == NULL)
{
    std::cerr << "Could not open the display" << std::endl;
    return false;
}

XF86VidModeModeInfo **modes;
if (!XF86VidModeGetAllModeLines(m_display, m_screenID, &modeNum, &modes))
{
    std::cerr << "Could not query the video modes" << std::endl;
    return false;
}

int bestMode = -1;
for (int i = 0; i < modeNum; i++)
{
    if ((modes[i]->hdisplay == width) &&
        (modes[i]->vdisplay == height))
    {
        bestMode = i;
    }
}

if (bestMode == -1)
{
    std::cerr << "Could not find a suitable graphics mode" << std::endl;
    return false;
}

int doubleBufferedAttribList [] = {
    GLX_RGBA, GLX_DOUBLEBUFFER,
    GLX_RED_SIZE, 4,
    GLX_GREEN_SIZE, 4,
    GLX_BLUE_SIZE, 4,
    GLX_DEPTH_SIZE, 16,
    None
};

XVisualInfo* vi = NULL;
//Attempt to create a double buffered window
vi = glXChooseVisual(m_display, m_screenID, doubleBufferedAttribList);

if (vi == NULL)
{
    std::cerr << "Could not create a double buffered window" << std::endl;
    return false;
}

//Create a GL 2.1 context
GLXContext gl2Context = glXCreateContext(m_display, vi, 0, GL_TRUE);

if (gl2Context == NULL)
{
    std::cerr << "Could not create a GL 2.1 context, please check your graphics drivers" << std::endl;
    return false;
}

//Get a pointer to the GL 3.0 context creation
PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribs = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((GLubyte*)"glXCreateContextAttribsARB");

if (glXCreateContextAttribs == NULL)
{
    std::cerr << "OpenGL 3.0 is not supported, falling back to 2.1" << std::endl;
    m_glContext = gl2Context;
    m_GL3Supported = false;
}
else
{
    //Create a GL 3.0 context

    int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,//we want a 3.0 context
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        0 //zero indicates the end of the array
    };

    m_glContext = glXCreateContextAttribs(m_display, framebufferConfig, 0, true, &attribs[0]);
    glXDestroyContext(m_display, gl2Context); //We can destroy the GL 2.0 context once the 3.0 one has bene created
    m_GL3Supported = true;
}

Colormap cmap = XCreateColormap(m_display, RootWindow(m_display, vi->screen),vi->visual, AllocNone);
m_XSetAttr.colormap = cmap;
m_XSetAttr.border_pixel = 0;
m_XSetAttr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask |
                            StructureNotifyMask;

m_XSetAttr.override_redirect = False;

unsigned long windowAttributes = CWBorderPixel | CWColormap | CWEventMask;

if (fullscreen)
{
    windowAttributes = CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;

    XF86VidModeSwitchToMode(m_display, m_screenID, modes[bestMode]);
    XF86VidModeSetViewPort(m_display, m_screenID, 0, 0);
    m_XSetAttr.override_redirect = True;
}

m_XWindow = XCreateWindow(m_display, RootWindow(m_display, vi->screen),
                              0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
                              CWBorderPixel | CWColormap | CWEventMask, &m_XSetAttr);

if (fullscreen)
{
    XWarpPointer(m_display, None, m_XWindow, 0, 0, 0, 0, 0, 0);
    XMapRaised(m_display, m_XWindow);
    XGrabKeyboard(m_display, m_XWindow, True, GrabModeAsync, GrabModeAsync, CurrentTime);
    XGrabPointer(m_display, m_XWindow, True, ButtonPressMask,
                 GrabModeAsync, GrabModeAsync, m_XWindow, None, CurrentTime);

    m_isFullscreen = true;
}
else
{
    Atom wmDelete = XInternAtom(m_display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(m_display, m_XWindow, &wmDelete, 1);
    XSetStandardProperties(m_display, m_XWindow, title.c_str(), None, NULL, NULL, 0, NULL);
    XMapRaised(m_display, m_XWindow);
}


XFree(modes);
}

void SimpleGLXWindow::destroy()
{
    m_mouse->showCursor(true);
    if (m_glContext)
    {
        glXMakeCurrent(m_display, None, NULL);
        glXDestroyContext(m_display, m_glContext);
        m_glContext = NULL;
    }

    if (m_isFullscreen)
    {
        XF86VidModeSwitchToMode(m_display, m_screenID, &m_XF86DeskMode);
        XF86VidModeSetViewPort(m_display, m_screenID, 0, 0);
    }

    XCloseDisplay(m_display);
}
#endif
