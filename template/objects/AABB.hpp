#pragma once

//define Aixe Align Bounding box
#include "object.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include <algorithm>

class AABB : public object
{
    //because this is only a bounding box used for accelerate compute
    //so parameters such as color will be ignored, only max, min and coordinate will be used
    //coordinate system will be the origin one
    public:
    vec3 Max, Min;

    AABB(vec3 _Max = vec3(), vec3 _Min = vec3())
    {
        Max = _Max; Min = _Min;
        co = Coordinate();
    }
    AABB(Triangle* T)
    {
        Max = vec3(INF, INF, INF);
        Min = vec3(-INF, -INF, -INF);
        vec3 point[3];
        point[0] = T->co.origin;
        point[1] = T->co.x_axis * T->u_length + T->co.origin;
        point[2] = T->co.y_axis * T->v_length + T->co.origin;
        Max.x = std::max(point[0].x, std::max(point[1].x, point[2].x));
        Max.y = std::max(point[0].y, std::max(point[1].y, point[2].y));
        Max.z = std::max(point[0].z, std::max(point[1].z, point[2].z));
        Min.x = std::min(point[0].x, std::min(point[1].x, point[2].x));
        Min.y = std::min(point[0].y, std::min(point[1].y, point[2].y));
        Min.z = std::min(point[0].z, std::min(point[1].z, point[2].z));
        co = Coordinate();
    }
    AABB(Sphere* S)
    {
        vec3 center = S->co.inv_trans_point(S->center);
        Max = center + vec3(S->radius, S->radius, S->radius);
        Min = center - vec3(S->radius, S->radius, S->radius);
        co = Coordinate();
    }

    virtual vec3 normal(vec3 v)
    {
        if(fabs(v.x - Max.x) < eps) return vec3(1, 0, 0);
        if(fabs(v.y - Max.y) < eps) return vec3(0, 1, 0);
        if(fabs(v.z - Max.z) < eps) return vec3(0, 0, 1);
        if(fabs(v.x - Min.x) < eps) return vec3(-1, 0, 0);
        if(fabs(v.y - Min.y) < eps) return vec3(0, -1, 0);
        return vec3(0, 0, -1);
    }
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal, vec3& color)
    {
        double t;
        t = (Max.x - ray->origin.x) / ray->direction.x;
        if(t > 0)
        {
            vec3 point = ray->origin + t * ray->direction;
            if(point.y < Max.y && point.y > Min.y && point.z < Max.z && point.z > Min.z) return true;
        }
        t = (Max.y - ray->origin.y) / ray->direction.y;
        if(t > 0)
        {
            vec3 point = ray->origin + t * ray->direction;
            if(point.x < Max.x && point.x > Min.x && point.z < Max.z && point.z > Min.z) return true;
        }
        t = (Max.z - ray->origin.z) / ray->direction.z;
        if(t > 0)
        {
            vec3 point = ray->origin + t * ray->direction;
            if(point.x < Max.x && point.x > Min.x && point.y < Max.y && point.y > Min.y) return true;
        }
        t = (Min.x - ray->origin.x) / ray->direction.x;
        if(t > 0)
        {
            vec3 point = ray->origin + t * ray->direction;
            if(point.y < Max.y && point.y > Min.y && point.z < Max.z && point.z > Min.z) return true;
        }
        t = (Min.y - ray->origin.y) / ray->direction.y;
        if(t > 0)
        {
            vec3 point = ray->origin + t * ray->direction;
            if(point.x < Max.x && point.x > Min.x && point.z < Max.z && point.z > Min.z) return true;
        }
        t = (Min.z - ray->origin.z) / ray->direction.z;
        if(t > 0)
        {
            vec3 point = ray->origin + t * ray->direction;
            if(point.x < Max.x && point.x > Min.x && point.y < Max.y && point.y > Min.y) return true;
        }
        return false;
    }

    AABB& operator = (const AABB& box)
    {
        Min = box.Min;
        Max = box.Max;
        return *this;
    }
};