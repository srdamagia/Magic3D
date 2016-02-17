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

#ifndef MAGIC3D_MATH_H
#define MAGIC3D_MATH_H

#include <cmath>
#include <vectormath_aos.h>
#include <magic3d/type.h>

namespace Magic3D
{

#define M3D_MATH_DELTA 1e-10 // error tolerance
#define BIT(x) (1<<(x))
#define M_PI  3.141592653589f

static const float M3D_MATH_EPSILON = 0.005f;            // error tolerance for check
static const int   M3D_MATH_FLOAT_DECIMAL_TOLERANCE = 3; // decimal places for float rounding
static const float M3D_MATH_DEGREE = M_PI / 180.0f;
static const float M3D_MATH_RADIAN = 180.0f / M_PI;

typedef Vectormath::Aos::Point3  Point3;
typedef Vectormath::Aos::Vector3 Vector3;
typedef Vectormath::Aos::Vector4 Vector4;
typedef Vectormath::Aos::Matrix3 Matrix3;
typedef Vectormath::Aos::Matrix4 Matrix4;
typedef Vectormath::Aos::Quat Quaternion;

struct Ray;
struct Box;
class Camera;

class Math
{
public:
    static inline float fract (float f)
    {
        return f - (int)f;
    }

    static inline float radians(float degrees)
    {
        return degrees * M3D_MATH_DEGREE;
    }

    static inline float degrees(float radians)
    {
        return radians * M3D_MATH_RADIAN;
    }

    static inline float sqr(float number)
    {
        return number * number;
    }

    static inline float cube(float number)
    {
        return number * number * number;
    }

    static inline float limitRange(float low, float value, float high)
    {
        return value < low ? low : value > high ? high : value;
    }

    static inline float clamp(float number)
    {
        return M3D_MATH_EPSILON > fabs(number) ? 0.0f : number;
    }

    static inline bool eq(float x, float v)
    {
        return v - M3D_MATH_EPSILON < x && x < v + M3D_MATH_EPSILON;
    }

    static inline float mix(float x, float y, float a)
    {
        return x * (1.0f - a) + y * a;
    }


    static inline float min(float a, float b)
    {
        return a < b ? a : b;
    }

    static inline int min(int a, int b)
    {
        return a < b ? a : b;
    }

    static inline Vector3 min(const Vector3& a, const Vector3& b)
    {
        return Vector3(min(a.getX(), b.getX()), min(a.getY(), b.getY()), min(a.getZ(), b.getZ()));
    }

    static inline float max(float a, float b)
    {
        return a > b ? a : b;
    }

    static inline int max(int a, int b)
    {
        return a > b ? a : b;
    }
    
    static inline Vector3 max(const Vector3& a, const Vector3& b)
    {
        return Vector3(max(a.getX(), b.getX()), max(a.getY(), b.getY()), max(a.getZ(), b.getZ()));
    }

    static inline float maxAbs(float a, float b)
    {
        return fabs(a) > fabs(b) ? a : b;
    }

    static inline float sgn(float a)
    {
        return a > 0.0f ? 1.0f : (a < 0.0f ? -1.0f : 0.0f);
    }

    static inline int randomize(int min, int max)
    {
        return min + (rand() % (max - min + 1));
    }

    static inline float randomize(float min, float max)
    {
        return min + (max - min) * ((float)rand() / (float)RAND_MAX);
    }

    static inline Vector3 randomize(Vector3 min, Vector3 max)
    {
        return Vector3(randomize(min.getX(), max.getX()),randomize(min.getY(), max.getY()),randomize(min.getZ(), max.getZ()));
    }

    static inline Vector3 vector3(btVector3 vector3)
    {
        return Vector3(vector3.x(), vector3.y(), vector3.z());
    }

    static bool collide(const Ray& ray, float start, float end, const Box& box);
    static bool collide(const Ray& ray, float start, float end, const Vector3& v0, const Vector3& v1, const Vector3& v2);
    static Vector3 euler(const Quaternion& quaternion);
    static Quaternion quaternion(const Vector3& euler);
    static void toAxisAngle(const Quaternion& quaternion, Vector3& axis, float& angle);
    static float angle(Vector3 vec1, Vector3 vec2, Vector3 normal);
    /*static float angleX(Vector3 vec1, Vector3 vec2);
    static float angleY(Vector3 vec1, Vector3 vec2);
    static float angleZ(Vector3 vec1, Vector3 vec2);*/
    static Quaternion inverse(const Quaternion& quaternion);

    static bool isInteger(std::string str);
    static bool isOnlyDigits(std::string str);

    static Vector3 intersectLinePlane(Vector3 linePoint, Vector3 lineVec, Vector3 planeNormal, Vector3 planePoint);

    static Vector3 scaleInDirection(Vector3 vector, Vector3 direction, float scale);
    static Vector3 vectorAxisAngle(Vector3 vector, Vector3 axis, float angle);
};

#pragma pack(push,1)
struct ColorRGBpack
{
    byte r;
    byte g;
    byte b;
} __attribute__((__packed__));

struct ColorRGBApack
{
    byte r;
    byte g;
    byte b;
    byte a;
} __attribute__((__packed__));
#pragma pack(pop)

struct Vector4Bool
{
    Vector4Bool() {x = false; y = false; z = false; w = false;}
    Vector4Bool(bool x, bool y, bool z, bool w) {this->x = x; this->y = y; this->z = z; this->w = w;}

    bool x;
    bool y;
    bool z;
    bool w;
};

struct Vector4Int
{
    Vector4Int() {x = 0; y = 0; z = 0; w = 0;}
    Vector4Int(int x, int y, int z, int w) {this->x = x; this->y = y; this->z = z; this->w = w;}

    int x;
    int y;
    int z;
    int w;
};

struct ColorRGB
{
    ColorRGB() {r = 1.0f; g = 1.0f; b = 1.0f;}
    ColorRGB(float r, float g, float b) {this->r = r; this->g = g; this->b = b;}

    float r;
    float g;
    float b;
};

struct ColorRGBA
{
    ColorRGBA() {r = 1.0f; g = 1.0f; b = 1.0f; a = 1.0f;}
    ColorRGBA(float r, float g, float b, float a) {this->r = r; this->g = g; this->b = b; this->a = a;}
    ColorRGBA(ColorRGB color, float a) {r = color.r; g = color.g; b = color.b; this->a = a;}

    float r;
    float g;
    float b;
    float a;
};

struct Texture2D
{
    Texture2D() {u = 0.0f; v = 0.0f;}
    Texture2D(float u, float v) {this->u = u; this->v = v;}

    float u;
    float v;
};

struct TriangleIndexes
{
    TriangleIndexes(vindex index1, vindex index2, vindex index3) {this->index1 = index1; this->index2 = index2; this->index3 = index3;}

    vindex index1;
    vindex index2;
    vindex index3;
};

struct LineIndexes
{
    LineIndexes(vindex index1, vindex index2) {this->index1 = index1; this->index2 = index2;}

    vindex index1;
    vindex index2;
};

struct Vertex3D
{
    Vertex3D() {position = Vector3(0.0f, 0.0f, 0.0f); normal = Vector3(0.0f, 1.0f, 0.0f); tangent = Vector3(0.0f, 0.0f, 0.0f);}
    Vertex3D(Vector3 position, Vector3 normal, ColorRGBA color, Texture2D uv) {this->position = position; this->normal = normal; tangent = Vector3(0.0f, 0.0f, 0.0f); this->color = color; this->uv[0] = uv; this->uv[1] = uv;}
    Vertex3D(Vector3 position, Vector3 normal, ColorRGBA color, Texture2D uv1, Texture2D uv2) {this->position = position; this->normal = normal; tangent = Vector3(0.0f, 0.0f, 0.0f); this->color = color; uv[0] = uv1; uv[1] = uv2;}

    Vector3 position;
    Vector3 normal;
    Vector3 tangent;

    ColorRGBA color;

    Texture2D uv[M3D_MULTI_TEXTURE_COUNT];

    Vector4 weights;
    Vector4 bones;
};

struct Ray {
    Ray(Vector3 pos, Vector3 dir) {
        origin = pos;
        direction = dir;
        inv_direction = Vector3(1/dir.getX(), 1/dir.getY(), 1/dir.getZ());
        sign[0] = (inv_direction.getX() < 0);
        sign[1] = (inv_direction.getY() < 0);
        sign[2] = (inv_direction.getZ() < 0);
    }

    Vector3 origin;
    Vector3 direction;
    Vector3 inv_direction;

    int sign[3];
};

struct Line {
    Line() {start = Vector3(0.0f, 0.0f, 0.0f); finish = Vector3(0.0f, 0.0f, 0.0f);}
    Line(Vector3 p1, Vector3 p2) {
        start = p1;
        finish = p2;
    }

    Vector3 start;
    Vector3 finish;
};

struct Sphere {
    Sphere() {position = Vector3(0.0f, 0.0f, 0.0f); radius = 1.0f;}
    Sphere(Vector3 pos, float rad) {
        position = pos;
        radius = rad;
    }

    Vector3 position;
    float radius;
};

enum BOX_WALL
{
    eBOX_WALL_FRONT,
    eBOX_WALL_BACK,
    eBOX_WALL_LEFT,
    eBOX_WALL_RIGHT,
    eBOX_WALL_TOP,
    eBOX_WALL_BOTTOM
};

struct Box
{
    Box() {corners[0] = Vector3(0.0f, 0.0f, 0.0f); corners[1] = Vector3(0.0f, 0.0f, 0.0f);}
    Box(Vector3 min, Vector3 max) {corners[0] = min; corners[1] = max;}

    inline float getWidth() {return fabs(corners[1].getX() - corners[0].getX());}
    inline float getHeight() {return fabs(corners[1].getY() - corners[0].getY());}
    inline float getDepth() {return fabs(corners[1].getZ() - corners[0].getZ());}
    inline float getMaxSize() {return Math::max(Math::max(getWidth(), getHeight()), getDepth());}
    inline Vector3 getCenter() {return Vector3(corners[0].getX() + getWidth() * 0.5f, corners[0].getY() + getHeight() * 0.5f, corners[0].getZ() + getDepth() * 0.5f);}
    bool contains(const Box& box)
    {
        Box b = box;
        if (b.getMaxSize() > 0.0f)
        {
            return corners[0].getX() <= box.corners[0].getX() && corners[0].getY() <= box.corners[0].getY() && corners[0].getZ() <= box.corners[0].getZ() &&
                   corners[1].getX() >= box.corners[1].getX() && corners[1].getY() >= box.corners[1].getY() && corners[1].getZ() >= box.corners[1].getZ();
        }
        else
        {
            return corners[0].getX() < box.corners[0].getX() && corners[0].getY() < box.corners[0].getY() && corners[0].getZ() < box.corners[0].getZ() &&
                   corners[1].getX() > box.corners[1].getX() && corners[1].getY() > box.corners[1].getY() && corners[1].getZ() > box.corners[1].getZ();
        }
    }

    Vector3 corners[2];
};

}

#endif // MATH_H
