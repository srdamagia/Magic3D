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

#ifndef MAGIC3D_RENDERER_WINDOW_H
#define MAGIC3D_RENDERER_WINDOW_H

namespace Magic3D
{

enum WINDOW_ORIENTATION
{
    eWINDOW_ORIENTATION_NONE,
    eWINDOW_ORIENTATION_PORTRAIT_UP,
    eWINDOW_ORIENTATION_PORTRAIT_DOWN,
    eWINDOW_ORIENTATION_LANDSCAPE_LEFT,
    eWINDOW_ORIENTATION_LANDSCAPE_RIGHT
};

class Window
{
protected:
#if defined(WIN32_OLD_WINDOW)
    std::wstring title;
#else
    std::string title;
#endif

    int width;
    int height;
    int cursorX;
    int cursorY;
    int bits;

    float screenScale;

    WINDOW_ORIENTATION orientation;

    bool fullscreen;
    bool cursor;
    bool active;
    bool needUpdateView;

public:
    virtual ~Window() {}
    virtual bool start() = 0;
    virtual bool finish() = 0;
    virtual bool render() = 0;    

    virtual void* getWindowHandle() = 0;
    virtual void showCursor(bool show) = 0;
    virtual void setClipboardText(const char* text) = 0;
    virtual const char* getClipboardText() = 0;

    virtual void grabInput(bool grabbed) = 0;
    virtual bool hasGrabbedInput() = 0;
    virtual bool hasInputFocus() = 0;
    virtual bool hasMouseFocus() = 0;

    virtual void mapGUIKeys(void* gui) = 0;
    virtual int getMouseState(int* x, int* y, bool* left, bool* right, bool* middle) = 0;

    bool isShowingCursor()
    {
        return cursor;
    }

    bool isActive()
    {
        return active;
    }

    void setActive(bool active)
    {
        this->active = active;
    }

    bool isNeedingUpdateView()
    {
        return needUpdateView;
    }

    void setNeedingUpdateView(bool update)
    {
        needUpdateView = update;
    }

    int getWidth()
    {
        return width;
    }

    int getHeight()
    {
        return height;
    }

    void resize(int width, int height)
    {
        resize(getOrientation(), width, height);
    }

    void resize(WINDOW_ORIENTATION orientation, int width, int height)
    {
        if (this->orientation != orientation || this->width != width || this->height != height)
        {
            needUpdateView = true;
        }
        setOrientation(orientation);
        switch (getOrientation())
        {
            case eWINDOW_ORIENTATION_NONE:
            case eWINDOW_ORIENTATION_PORTRAIT_UP:
            case eWINDOW_ORIENTATION_PORTRAIT_DOWN:
            {
                this->width = width;
                this->height = height;
                break;
            }
            case eWINDOW_ORIENTATION_LANDSCAPE_LEFT:
            case eWINDOW_ORIENTATION_LANDSCAPE_RIGHT:
            {
                this->width = height;
                this->height = width;
                break;
            }
        }

        float min = 320.0f;
        float screenMin = min;

        if (this->width > this->height)
        {
            screenMin = this->height;
        }

        if (this->height > this->width)
        {
            screenMin = this->width;
        }

        screenScale = screenMin / min;
    }

    float getScreenScale()
    {
        return screenScale;
    }

    void setCursorPosition(int x, int y)
    {
        cursorX = x;
        cursorY = y;
    }

    int getCursorX()
    {
        return cursorX;
    }

    int getCursorY()
    {
        return cursorY;
    }

    virtual void setFullScreen(bool fullscreen)
    {
        this->fullscreen = fullscreen;
    }

    bool isFullScreen()
    {
        return fullscreen;
    }

    void setOrientation(WINDOW_ORIENTATION orientation)
    {
        if (orientation >= 0 && orientation <= 4)
        {
            if (this->orientation != orientation)
            {
                needUpdateView = true;
            }
            this->orientation = orientation;
        }
    }

    WINDOW_ORIENTATION getOrientation()
    {
        return orientation;
    }

    Vector3 getWindowScreenAspect()
    {
        float w = (float)getWidth();
        float h = (float)getHeight();
        float sx = w > h ? w / h : 1.0f;
        float sy = w > h ? 1.0f : h / w;
        return Vector3(sx, sy, 0.0f);
    }

    Vector3 getWindowScreenXY(float x, float y)
    {
        Vector3 aspect = getWindowScreenAspect();
        return Vector3(x / (float)getWidth() * aspect.getX(), y / (float)getHeight() * aspect.getY(), 0.0f);
    }
};

}

#endif // MAGIC3D_RENDERER_WINDOW_H
