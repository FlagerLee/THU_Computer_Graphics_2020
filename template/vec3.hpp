//written by Flager Lee

#pragma once

#include "utils.hpp"
#include <math.h>

/*提供图形学中使用的三维向量
 *使用场景包括但不限于：空间坐标，三维向量，颜色空间
 *定义向量间加减法，点积，叉积，哈达马积
 *定义与整数、浮点数的乘除法
 *不提供越界检查
/*/

class vec3
{
    public:
    double x, y, z;

    vec3(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {}
    vec3(const vec3& v) {x = v.x, y = v.y, z = v.z;}

    vec3& operator = (const vec3& v)
    {
        x = v.x; y = v.y; z = v.z;
        return *this;
    }

    double length() //get euclid length of the vector
    {
        return sqrt(x * x + y * y + z * z);
    }
    vec3 normalize() //normalize the vector
    {
        double length = this->length();
        x /= length;
        y /= length;
        z /= length;
        return vec3(x, y, z);
    }
};

vec3 operator + (const vec3& v1, const vec3& v2) //vector add
{
    return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
vec3 operator - (const vec3& v1, const vec3& v2) //vector minus
{
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
double operator * (const vec3& v1, const vec3& v2) //dot product
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
vec3 operator ^ (const vec3& v1, const vec3& v2) //hadamard product
{
    return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
vec3 operator % (const vec3& v1, const vec3& v2) //cross product
{
    return vec3(v1.y * v2.z - v2.y * v1.z, v1.z * v2.x - v2.z * v1.x, v1.x * v2.y - v2.x * v1.y);
}

vec3 operator * (const vec3& v, const double d)
{
    return vec3(v.x * d, v.y * d, v.z * d);
}
vec3 operator * (const double d, const vec3& v)
{
    return vec3(v.x * d, v.y * d, v.z * d);
}
vec3 operator * (const vec3& v, const float f)
{
    double d = (double)f;
    return vec3(v.x * d, v.y * d, v.z * d);
}
vec3 operator * (const float f, const vec3& v)
{
    double d = (double)f;
    return vec3(v.x * d, v.y * d, v.z * d);
}
vec3 operator * (const vec3& v, const int i)
{
    return vec3(v.x * i, v.y * i, v.z * i);
}
vec3 operator * (const int i, const vec3& v)
{
    return vec3(v.x * i, v.y * i, v.z * i);
}
vec3 operator / (const vec3& v, const double d)
{
    return vec3(v.x / d, v.y / d, v.z / d);
}
vec3 operator / (const vec3& v, const float f)
{
    double d = (double)f;
    return vec3(v.x / d, v.y / d, v.z / d);
}
vec3 operator / (const vec3& v, const int i)
{
    return vec3(v.x / i, v.y / i, v.z / i);
}
bool operator == (const vec3& v1, const vec3& v2)
{
    return (fabs(v1.x - v2.x) < eps) && (fabs(v1.y - v2.y) < eps) && (fabs(v1.z - v2.z) < eps);
}

typedef vec3 color;
