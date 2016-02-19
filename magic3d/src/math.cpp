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

#include <magic3d/math.h>

bool Magic3D::Math::collide(const Ray& ray, float start, float end, const Box& box)
{
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (box.corners[ray.sign[0]].getX() - ray.origin.getX()) * ray.inv_direction.getX();
    tmax = (box.corners[1-ray.sign[0]].getX() - ray.origin.getX()) * ray.inv_direction.getX();
    tymin = (box.corners[ray.sign[1]].getY() - ray.origin.getY()) * ray.inv_direction.getY();
    tymax = (box.corners[1-ray.sign[1]].getY() - ray.origin.getY()) * ray.inv_direction.getY();

    if ( (tmin > tymax) || (tymin > tmax) )
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (box.corners[ray.sign[2]].getZ() - ray.origin.getZ()) * ray.inv_direction.getZ();
    tzmax = (box.corners[1-ray.sign[2]].getZ() - ray.origin.getZ()) * ray.inv_direction.getZ();

    if ( (tmin > tzmax) || (tzmin > tmax) )
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    return ( (tmin < end) && (tmax > start) );
}

bool Magic3D::Math::collide(const Ray& ray, float start, float end, const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
    bool result = false;
    Vector3 n = ray.origin + ray.direction * start;
    Vector3 f = ray.origin + ray.direction * end;

    Vector3 l = f - n;

    Vector3 N = cross(v1 - v0, v2 - v0);
    float sin_alpha = asin(Vectormath::Aos::length(N) / (Vectormath::Aos::length(v1 - v0) * Vectormath::Aos::length(v2 - v0)));


    Vector3 normal = normalize(N) * (sin_alpha > 0.0f ? sin_alpha : 1.0f);


    float nDotL = dot(normal, l);

    if (nDotL <= 0.0) // ray is parallel to plane (triangle)
    {
        return false;
    }

    float d = dot(normal, (v0 - n) / nDotL);
    if (d < 0.0f || d > 1.0f) // plane is beyond the ray we consider
    {
        return false;
    }

    Vector3 p = n + d * l; // p intersect the plane (triangle)

    Vector3 n1 = cross(v1 - v0, p - v0);
    Vector3 n2 = cross(v2 - v1, p - v1);
    Vector3 n3 = cross(v0 - v2, p - v2);

    if (dot(n, n1) >= 0.0f &&
        dot(n, n2) >= 0.0f &&
        dot(n, n3) >= 0.0f)
    {
        result = true;
    }

    return result;
}

Magic3D::Vector3 Magic3D::Math::euler(const Quaternion& quaternion)
{
    float x;
    float y;
    float z;

    float sqw = quaternion.getW() * quaternion.getW();
    float sqx = quaternion.getX() * quaternion.getX();
    float sqy = quaternion.getY() * quaternion.getY();
    float sqz = quaternion.getZ() * quaternion.getZ();
    float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
    float test = quaternion.getX() * quaternion.getY() + quaternion.getZ() * quaternion.getW();
    if (test > 0.499f * unit) { // singularity at north pole
        y = 2.0f * atan2(quaternion.getX(), quaternion.getW());
        z = M_PI * 0.5f;
        x = 0.0f;
        return Vector3(Math::degrees(x), Math::degrees(y), Math::degrees(z));
    }
    if (test < -0.499f * unit) { // singularity at south pole
        y = -2.0f * atan2(quaternion.getX(), quaternion.getW());
        z = -M_PI * 0.5f;
        x = 0;
        return Vector3(Math::degrees(x), Math::degrees(y), Math::degrees(z));
    }
    y = atan2(2.0f * quaternion.getY() * quaternion.getW() - 2.0f * quaternion.getX() * quaternion.getZ() , sqx - sqy - sqz + sqw);
    z = asin(2.0f * test / unit);
    x = atan2(2.0f * quaternion.getX() * quaternion.getW() - 2.0f * quaternion.getY() * quaternion.getZ() , -sqx + sqy - sqz + sqw);

    return Vector3(Math::degrees(x), Math::degrees(y), Math::degrees(z));
}

Magic3D::Quaternion Magic3D::Math::quaternion(const Vector3& euler)
{
    float c1 = cosf(Math::radians(euler.getY()) * 0.5f);
    float s1 = sinf(Math::radians(euler.getY()) * 0.5f);
    float c2 = cosf(Math::radians(euler.getZ()) * 0.5f);
    float s2 = sinf(Math::radians(euler.getZ()) * 0.5f);
    float c3 = cosf(Math::radians(euler.getX()) * 0.5f);
    float s3 = sinf(Math::radians(euler.getX()) * 0.5f);
    float c1c2 = c1 * c2;
    float s1s2 = s1 * s2;
    float w =c1c2 * c3 - s1s2 * s3;
    float x =c1c2 * s3 + s1s2 * c3;
    float y =s1 * c2 * c3 + c1 * s2 * s3;
    float z =c1 * s2 * c3 - s1 * c2 * s3;
    return Quaternion(x, y, z, w);
}

void Magic3D::Math::toAxisAngle(const Quaternion& quaternion, Vector3& axis, float& angle)
{
    Magic3D::Quaternion q = quaternion.getW() > 1.0f ? normalize(quaternion): quaternion;

    angle = 2.0f * acosf(q.getW());

    float s = sqrtf(1.0f - q.getW() * q.getW()); // assuming quaternion normalised then w is less than 1, so term always positive.
    if (s < 0.001f)
    {
        // test to avoid divide by zero, s is always positive due to sqrt
        // if s close to zero then direction of axis not important
        axis.setX(q.getX()); // if it is important that axis is normalised then replace with x=1; y=z=0;
        axis.setY(q.getY());
        axis.setZ(q.getZ());
    }
    else
    {
        axis.setX(q.getX() / s); // normalise axis
        axis.setY(q.getY() / s);
        axis.setZ(q.getZ() / s);
    }
}

float Magic3D::Math::angle(Vector3 vec1, Vector3 vec2, Vector3 normal)
{
    float dp, angPi;
    Vector3 v1 = normalize(vec1);
    Vector3 v2 = normalize(vec2);    
    Vector3 n = normalize(normal);
    Vector3 perp = cross(n, v1);

    dp = dot(vec1, v2);

    if (dp < -1.0f)
    {
        dp = -1.0f;
    }
    if (dp > 1.0f)
    {
        dp = 1.0f;
    }

    angPi = acosf(dp) * (dot(perp, vec2) >= 0.0f ? 1.0f : -1.0f);

    return angPi;
}

/*float Magic3D::Math::angleX(Vector3 vec1, Vector3 vec2)
{
    float dp, angPi;
    Vector3 v1 = normalize(vec1);
    Vector3 v2 = normalize(vec2);
    dp = dot(v1, v2);

    //angPi = atan2f(length(cross(v1, v2)),  dp);
    angPi = acosf(dp);

    if (v1.getZ() * v2.getY() > v1.getY() * v2.getZ())
        return -angPi;
    else
        return angPi;

    return angPi;
}

float Magic3D::Math::angleY(Vector3 vec1, Vector3 vec2)
{
    float dp, angPi;
    Magic3D::Vector3 v1 = normalize(vec1);
    Magic3D::Vector3 v2 = normalize(vec2);
    dp = dot(v1, v2);

    //angPi = atan2f(length(cross(v1, v2)),  dp);
    angPi = acosf(dp);

    if (v1.getZ() * v2.getX() > v1.getX() * v2.getZ())
        return -angPi;
    else
        return angPi;

    return angPi;
}

float Magic3D::Math::angleZ(Vector3 vec1, Vector3 vec2)
{
    float dp, angPi;
    Vector3 v1 = normalize(vec1);
    Vector3 v2 = normalize(vec2);
    dp = dot(v1, v2);

    //angPi = atan2f(length(cross(v1, v2)),  dp);
    angPi = acosf(dp);

    if (v1.getX() * v2.getY() > v1.getY() * v2.getX())
        return -angPi;
    else
        return angPi;

    return angPi;
}*/

Magic3D::Quaternion Magic3D::Math::inverse(const Quaternion& quaternion)
{
    return Magic3D::Quaternion(-quaternion.getX(), -quaternion.getY(), -quaternion.getZ(), quaternion.getW());
}


bool Magic3D::Math::isInteger(std::string str)
{
   if(str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+'))) return false;

   char * p ;
   strtol(str.c_str(), &p, 10) ;

   return (*p == 0) ;
}

bool Magic3D::Math::isOnlyDigits(std::string str)
{
    return str.find_first_not_of( "0123456789" ) == str.npos;
}

Magic3D::Vector3 Magic3D::Math::intersectLinePlane(Vector3 linePoint, Vector3 lineVec, Vector3 planeNormal, Vector3 planePoint)
{
    float length;
    float dotNumerator;
    float dotDenominator;

    //calculate the distance between the linePoint and the line-plane intersection point
    dotNumerator = dot((planePoint - linePoint), planeNormal);
    dotDenominator = dot(lineVec, planeNormal);

    //line and plane are not parallel
    if(dotDenominator != 0.0f)
    {
        length =  dotNumerator / dotDenominator;

        //create a vector from the linePoint to the intersection point
        return linePoint + lineVec * length;
    }
    //output not valid
    else
    {
        return linePoint;
    }
}

Magic3D::Vector3 Magic3D::Math::scaleInDirection(Vector3 vector, Vector3 direction, float scale)
{
    float currentMag = dot(vector, direction);

    Vector3 change = direction * (currentMag * scale - currentMag);
    return vector + change;
}

Magic3D::Vector3 Magic3D::Math::vectorAxisAngle(Vector3 vector, Vector3 axis, float angle)
{
    float cosr = cosf(angle);
    float sinr = sinf(angle);
    return ((vector * cosr) + (cross(axis, vector) * sinr)) + (axis * dot(axis, vector) * (1.0f - cosr));
}
