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
#if defined(WIN32_OLD_WINDOW)
#include <magic3d/magic3d.h>
#include <magic3d/renderer/window_win.h>

Magic3D::WindowWin* Magic3D::WindowWin::instance = NULL;
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Declaration For WndProc

Magic3D::WindowWin::WindowWin()
{
    std::string t = Magic3D::getInstance()->getConfiguration().TITLE;
    title = std::wstring(t.begin(), t.end());
    width  = Magic3D::getInstance()->getConfiguration().WINDOW_WIDTH;
    height = Magic3D::getInstance()->getConfiguration().WINDOW_HEIGHT;
    cursorX = 0;
    cursorY = 0;
    orientation = eWINDOW_ORIENTATION_NONE;

    bits   = 32;

    fullscreen = Magic3D::getInstance()->getConfiguration().FULLSCREEN;
    cursor     = Magic3D::getInstance()->getConfiguration().CURSOR;
    active     = false;
    needUpdateView = false;

    hDC       = NULL;
    hRC       = NULL;
    hWnd      = NULL;
    hInstance = NULL;
}

Magic3D::WindowWin::~WindowWin()
{
    instance = NULL;
}

bool Magic3D::WindowWin::start()
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

bool Magic3D::WindowWin::finish()
{
    bool result = true;

    if (fullscreen)
    {
        ChangeDisplaySettings(NULL, 0);
        ShowCursor(cursor);
    }

    if (hRC)
    {
        if (!wglMakeCurrent(NULL, NULL))
        {
            result = false;
            Log::log(eLOG_FAILURE, "Release Of DC And RC Failed.");
        }

        if (!wglDeleteContext(hRC))
        {
            result = false;
            Log::log(eLOG_FAILURE, "Release Rendering Context Failed.");
        }
    }
    hRC = NULL;

    if (hDC && !ReleaseDC(hWnd, hDC))
    {
        result = false;
        Log::log(eLOG_FAILURE, "Release Device Context Failed.");
    }
    hDC = NULL;

    if (hWnd && !DestroyWindow(hWnd))
    {
        result = false;
        Log::log(eLOG_FAILURE, "Could Not Release hWnd.");
    }
    hWnd = NULL;

#ifdef UNICODE
    if (!UnregisterClass((WCHAR*)"OpenGL", hInstance))
#else
    if (!UnregisterClass("OpenGL", hInstance))
#endif
    {
        result = false;
        Log::log(eLOG_FAILURE, "Could Not Unregister Class.");
    }
    hInstance = NULL;

    Log::log(eLOG_SUCCESS, "Window sucessfully finished.");

    return result;
}

bool Magic3D::WindowWin::render()
{
    MSG  msg;
    if (PeekMessage(&msg,NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            getInstance()->setActive(false);
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return SwapBuffers(hDC);
}

Magic3D::WindowWin* Magic3D::WindowWin::getInstance()
{
    if (!instance)
    {
        instance = new WindowWin();
    }

    return instance;
}

/* This Code Creates Our Window.  Parameters Are:                       *
 * title          - Title To Appear At The Top Of The Window            *
 * width          - Width Of The GL Window Or Fullscreen Mode           *
 * height         - Height Of The GL Window Or Fullscreen Mode          *
 * bits           - Number Of Bits To Use For Color (8/16/24/32)        *
 * fullscreenflag - Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE) */
bool Magic3D::WindowWin::create()
{
    unsigned int PixelFormat;
    WNDCLASS     wc;
    DWORD        dwExStyle;
    DWORD        dwStyle;
    RECT         WindowRect;

    //GetClientRect( GetDesktopWindow(), &WindowRect ) ;

    int xPos = GetSystemMetrics(SM_CXSCREEN) / 2;
    int yPos = GetSystemMetrics(SM_CYSCREEN) / 2;

    int halfW = width / 2;
    int halfH = height / 2;
    WindowRect.left   = (long)(xPos - halfW);
    WindowRect.right  = (long)(xPos + halfW);
    WindowRect.top    = (long)(yPos - halfH);
    WindowRect.bottom = (long)(yPos + halfH);

    hInstance        = GetModuleHandle(NULL);
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = (WNDPROC) WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
#ifdef UNICODE
    wc.lpszClassName = (WCHAR*)"OpenGL";
#else
    wc.lpszClassName = "OpenGL";
#endif

    if (!RegisterClass(&wc))
    {
        Log::log(eLOG_FAILURE, "Failed To Register The Window Class.");
        return false;
    }

    if (fullscreen)
    {
        DEVMODE dmScreenSettings;
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize         = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth	= width;
        dmScreenSettings.dmPelsHeight	= height;
        dmScreenSettings.dmBitsPerPel	= bits;
        dmScreenSettings.dmFields       = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            fullscreen = false;
        }
    }

    if (fullscreen)
    {
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle   = WS_POPUP;        
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle   = WS_OVERLAPPEDWINDOW;
    }

    AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);

    LONG rw = WindowRect.right-WindowRect.left;
    LONG rh = WindowRect.bottom-WindowRect.top;

#ifdef UNICODE
    if (!(hWnd=CreateWindowEx(dwExStyle, (WCHAR*)"OpenGL", title.c_str(), dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WindowRect.left, WindowRect.top, rw, rh, NULL, NULL, hInstance, NULL)))
#else
    if (!(hWnd=CreateWindowEx(dwExStyle, "OpenGL", title.c_str(), dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WindowRect.left, WindowRect.top, rw, rh, NULL, NULL, hInstance, NULL)))
#endif
    {
        finish();
        Log::log(eLOG_FAILURE, "Window Creation Error.");
        return false;
    }

    static PIXELFORMATDESCRIPTOR pfd = // pfd Tells Windows How We Want Things To Be
    {
            sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
            1,                             // Version Number
            PFD_DRAW_TO_WINDOW |           // Format Must Support Window
            PFD_SUPPORT_OPENGL |           // Format Must Support OpenGL
            PFD_DOUBLEBUFFER,              // Must Support Double Buffering
            PFD_TYPE_RGBA,                 // Request An RGBA Format
            bits,                          // Select Our Color Depth
            0, 0, 0, 0, 0, 0,              // Color Bits Ignored
            0,                             // No Alpha Buffer
            0,                             // Shift Bit Ignored
            0,                             // No Accumulation Buffer
            0, 0, 0, 0,                    // Accumulation Bits Ignored
            16,                            // 16Bit Z-Buffer (Depth Buffer)
            0,                             // No Stencil Buffer
            0,                             // No Auxiliary Buffer
            PFD_MAIN_PLANE,                // Main Drawing Layer
            0,                             // Reserved
            0, 0, 0                        // Layer Masks Ignored
    };

    if (!(hDC = GetDC(hWnd)))
    {
        finish();
        Log::log(eLOG_FAILURE, "Can't Create A GL Device Context.");
        return false;
    }

    if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
    {
        finish();
        Log::log(eLOG_FAILURE, "Can't Find A Suitable PixelFormat.");
        return false;
    }

    if(!SetPixelFormat(hDC, PixelFormat, &pfd))
    {
        finish();
        Log::log(eLOG_FAILURE, "Can't Set The PixelFormat.");
        return false;
    }

    if (!(hRC = wglCreateContext(hDC)))
    {
        finish();
        Log::log(eLOG_FAILURE, "Can't Create A GL Rendering Context.");
        return false;
    }

    if(!wglMakeCurrent(hDC, hRC))
    {
        finish();
        Log::log(eLOG_FAILURE, "Can't Activate The GL Rendering Context.");
        return false;
    }

    ShowCursor(cursor);
    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    Renderer::getInstance()->initialize();

    Renderer::getInstance()->resizeWindow(width, height);

    return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_ACTIVATE:
        {
            if (!HIWORD(wParam))
            {
                Magic3D::WindowWin::getInstance()->setActive(true);
            }

            return 0;
        }

        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                    return 0;
            }
            break;
        }

        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_KEYDOWN:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_KEYBOARD, Magic3D::eEVENT_KEYBOARD_DOWN, (int)wParam);
            return 0;
        }

        case WM_KEYUP:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_KEYBOARD, Magic3D::eEVENT_KEYBOARD_UP, (int)wParam);
            return 0;
        }

        case WM_SIZE:
        {
            Magic3D::Renderer::getInstance()->resizeWindow(LOWORD(lParam), HIWORD(lParam));
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_MOUSE, Magic3D::eEVENT_MOUSE_DOWN, LOWORD(lParam), HIWORD(lParam), 1);
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_DOWN, LOWORD(lParam), HIWORD(lParam), 1);
            return 0;
        }

        case WM_LBUTTONUP:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_MOUSE, Magic3D::eEVENT_MOUSE_UP, LOWORD(lParam), HIWORD(lParam), 1);
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_UP, LOWORD(lParam), HIWORD(lParam), 1);
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_MOUSE, Magic3D::eEVENT_MOUSE_DOWN, LOWORD(lParam), HIWORD(lParam), 2);
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_DOWN, LOWORD(lParam), HIWORD(lParam), 2);
            return 0;
        }

        case WM_RBUTTONUP:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_MOUSE, Magic3D::eEVENT_MOUSE_UP, LOWORD(lParam), HIWORD(lParam), 2);
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_UP, LOWORD(lParam), HIWORD(lParam), 2);
            return 0;
        }

        case WM_MBUTTONDOWN:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_MOUSE, Magic3D::eEVENT_MOUSE_DOWN, LOWORD(lParam), HIWORD(lParam), 3);
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_DOWN, LOWORD(lParam), HIWORD(lParam), 3);
            return 0;
        }

        case WM_MBUTTONUP:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_MOUSE, Magic3D::eEVENT_MOUSE_UP, LOWORD(lParam), HIWORD(lParam), 3);
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_UP, LOWORD(lParam), HIWORD(lParam), 3);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_MOUSE, Magic3D::eEVENT_MOUSE_MOVE, LOWORD(lParam), HIWORD(lParam), wParam);
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_TOUCH, Magic3D::eEVENT_TOUCH_MOVE, LOWORD(lParam), HIWORD(lParam), wParam);
            Magic3D::Renderer::getInstance()->getWindow()->setCursorPosition(LOWORD(lParam), HIWORD(lParam));
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_MOUSE, Magic3D::eEVENT_MOUSE_WHEEL, LOWORD(lParam), HIWORD(lParam), HIWORD(wParam));
            return 0;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
#endif
#endif
