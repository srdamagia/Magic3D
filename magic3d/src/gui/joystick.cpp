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

#include <magic3d/magic3d.h>
#include <magic3d/object.h>

Magic3D::Joystick::Joystick(const Joystick& joystick, std::string name) : GUI(joystick, name)
{
    this->joyDir = Vector3(0.0f, 0.0f, 0.0f);
    this->joyPos = Vector3(0.0f, 0.0f, 0.0f);
    this->startPos = Vector3(0.0f, 0.0f, 0.0f);
    this->force = 0.0f;
    this->button = -1;

    init();
}

Magic3D::Joystick::Joystick(std::string name) : GUI(eOBJECT_JOYSTICK, name)
{
    this->joyDir = Vector3(0.0f, 0.0f, 0.0f);
    this->joyPos = Vector3(0.0f, 0.0f, 0.0f);
    this->startPos = Vector3(0.0f, 0.0f, 0.0f);
    this->force = 0.0f;
    button = -1;

    width = 0.25f;
    height = 0.25f;

    init();
}

void Magic3D::Joystick::init()
{
    float col = 8.0f;
    float row = 8.0f;

    textureX = 1.0f / col;
    textureHalfX = textureX * 0.5f;
    textureDoubleX = textureX * 2.0f;

    textureY = 1.0f / row;
    textureHalfY = textureY * 0.5f;
    textureDoubleY = textureY * 2.0f;

    pixelX = textureX / col;
    pixelY = textureY / row;

    texAreaX = textureX;
    texAreaY = textureY;

    for (int i = 0; i < 2; i++)
    {
        spriteMesh->getData()->addQuad(0.0f, 0.0f, 1.0f, 1.0f, eAXIS_Z);
    }

    spriteMesh->getData()->createVbo();

    needUpdate = true;

    inputMouse = new Joystick_Input_Mouse(this);
    inputTouch = new Joystick_Input_Touch(this);
    Input::getInstance()->addEventListener(inputMouse);
    Input::getInstance()->addEventListener(inputTouch);
}

Magic3D::Joystick::~Joystick()
{
    Input::getInstance()->removeEventListener(inputMouse);
    Input::getInstance()->removeEventListener(inputTouch);

    if (inputMouse)
    {
        delete inputMouse;
        inputMouse = NULL;
    }
    if (inputTouch)
    {
        delete inputTouch;
        inputTouch = NULL;
    }
}

void* Magic3D::Joystick::spawn(std::string name) const
{
    return (new Joystick(*this, name));
}

Magic3D::Vector3 Magic3D::Joystick::getDirection()
{
    return joyDir;
}

float Magic3D::Joystick::getForce()
{
    return force;
}

bool Magic3D::Joystick::updateMeshes()
{
    float widthHalf  = width * 0.5f;
    float heightHalf = height * 0.5f;
    float startX     = -widthHalf;
    float startY     = -heightHalf;

    float texX = 0.0f;
    float texY = 1.0f;

    ColorRGBA color;
    Material* material = getMaterial();

    if (material)
    {
        color = material->getColorDiffuse();
    }

    MeshData* data = spriteMesh->getData();

    float* buffer = data->mapBuffer();

    texX = textureX * 6;
    texY = texY - textureY * 2;
    data->setQuadTexture(buffer, 0, texX, texY, texAreaX, texAreaY);
    data->setQuad(buffer, 0, startX, startY, width, height, color);

    startX += widthHalf * 0.25f;
    startY += heightHalf * 0.25f;
    texX = 0.0f;
    texY = texY - textureY;
    data->setQuadTexture(buffer, 1, texX, texY, texAreaX, texAreaY);
    data->setQuad(buffer, 1, startX + joyPos.getX(), startY + joyPos.getY(), width * 0.75f, height * 0.75f, color);

    data->unmapBuffer();

    return true;
}

void Magic3D::Joystick::calculateDirection(float x, float y)
{
    Vector3 curPos = getPosition();
    Vector3 newPos = Vector3(x, y, 0.0f);

    float min = Math::min(getWidth() * 0.5f, getHeight() * 0.5f);
    float distance = Math::min(min, dist(Point3(curPos), Point3(newPos)));

    if (distance == 0.0f)
    {
        joyDir = Vector3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        joyDir = normalize(newPos - curPos);
    }

    joyPos = joyDir * distance;
    force = distance / min;

    needUpdate = true;
}

void Magic3D::Joystick::down(int x, int y, int button)
{
    if (this->button == -1)
    {
        Vector3 pos = Renderer::getInstance()->getWindow()->getWindowScreenXY((float)x, (float)y);

        float halfW = getWidth() * 0.5f * getScale().getX();
        float halfH = getHeight() * 0.5f * getScale().getY();

        if (pos.getX() > getPosition().getX() - halfW && pos.getX() < getPosition().getX() + halfW &&
            pos.getY() > getPosition().getY() - halfH && pos.getY() < getPosition().getY() + halfH)
        {
            this->button = button;

            startPos.setX(pos.getX());
            startPos.setY(pos.getY());

            calculateDirection(pos.getX(), pos.getY());
        }
    }
}

void Magic3D::Joystick::pressed(int x, int y, int button)
{
    if (x > 0 && y > 0 && button > 0)
    {

    }
}

void Magic3D::Joystick::up(int x, int y, int button)
{
    if (x > 0 && y > 0)
    {

    }
    if (button == this->button)
    {
        this->button = -1;

        calculateDirection(getPosition().getX(), getPosition().getY());
    }
}

void Magic3D::Joystick::move(int deltaX, int deltaY, int button)
{
    if (button == this->button)
    {
        Vector3 pos = Renderer::getInstance()->getWindow()->getWindowScreenXY((float)deltaX, (float)deltaY);

        startPos.setX(pos.getX());
        startPos.setY(pos.getY());
        calculateDirection(startPos.getX(), startPos.getY());
    }
}

//******************************************************************************
Magic3D::Joystick_Input_Mouse::Joystick_Input_Mouse(Joystick* joystick)
{
    this->joystick = joystick;
}

void Magic3D::Joystick_Input_Mouse::down(int x, int y, int button)
{
    if (this->joystick)
    {
        joystick->down(x, y, button);
    }
}

void Magic3D::Joystick_Input_Mouse::pressed(int x, int y, int button)
{
    if (this->joystick)
    {
        joystick->pressed(x, y, button);
    }
}

void Magic3D::Joystick_Input_Mouse::up(int x, int y, int button)
{
    if (this->joystick)
    {
        joystick->up(x, y, button);
    }
}

void Magic3D::Joystick_Input_Mouse::move(int deltaX, int deltaY)
{
    if (this->joystick)
    {
        joystick->move(deltaX, deltaY, 0);
    }
}

void Magic3D::Joystick_Input_Mouse::wheel(int x, int y, int delta)
{
    if (joystick && x > 0 && y > 0 && delta > 0)
    {

    }
}

//******************************************************************************
Magic3D::Joystick_Input_Touch::Joystick_Input_Touch(Joystick* joystick)
{
    this->joystick = joystick;
}

void Magic3D::Joystick_Input_Touch::down(int x, int y, int finger)
{
    if (this->joystick)
    {
        joystick->down(x, y, finger);
    }
}

void Magic3D::Joystick_Input_Touch::pressed(int x, int y, int finger)
{
    if (this->joystick)
    {
        joystick->pressed(x, y, finger);
    }
}

void Magic3D::Joystick_Input_Touch::up(int x, int y, int finger)
{
    if (this->joystick)
    {
        joystick->up(x, y, finger);
    }
}

void Magic3D::Joystick_Input_Touch::move(int deltaX, int deltaY, int finger)
{
    if (this->joystick)
    {
        joystick->move(deltaX, deltaY, finger);
    }
}
