#pragma once

#include "object.hpp"
#include <algorithm>

class Cylinder : public object
{
    public:
    double radius, height;

    Cylinder(double _radius, double _height,  vec3 _color, vec3 _emission, double _ratio, Coordinate _co, Refl_t _type)
    {
        radius = _radius;
        height = _height;
        color = _color;
        emission = _emission;
        ratio = _ratio;
        co = _co;
        type = _type;
    }

    virtual vec3 normal(vec3 v) const //v: (theta, height, 0)
    {
        if(fabs(v.y - height) < eps) return vec3(0, 0, 1);
        else if(fabs(v.y - 0.0) < eps) return vec3(0, 0, -1);
        else return vec3(v.x, v.y, 0).normalize();
    }
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal) const
    {
        Ray new_ray;
        if(!(this->co == Coordinate())) new_ray = Ray(co.trans_point(ray->origin), co.trans_vector(ray->direction));
        else new_ray = *ray;
        //first judge if the ray hits the two circle surface
        double t1 = (0.0 - new_ray.origin.z) / new_ray.direction.z;
        double t2 = (height - new_ray.origin.z) / new_ray.direction.z;
        double t = t1 > 0.0 ? (t2 > t1 ? t1 : (t2 > 0.0 ? t2 : t1)) : (t2 > 0.0 ? t2 : INF);

        double dx2 = new_ray.direction.x * new_ray.direction.x;
        double dy2 = new_ray.direction.y * new_ray.direction.y;
        double ox2 = new_ray.origin.x * new_ray.origin.x;
        double oy2 = new_ray.origin.y * new_ray.origin.y;
        double A = dx2 + dy2, B = 2 * (new_ray.direction.x * new_ray.origin.x + new_ray.direction.y * new_ray.origin.y), C = ox2 + oy2 - radius * radius;
        double delta = B * B / 4 - A * C;
        if(delta < 0.0) return false;
        double B_ = (-B / 2) / A;
        double delta_ = sqrt(delta) / A;
        vec3 new_intersection;
        t1 = B_ - delta_; t2 = B_ + delta_;
        
        //select the smallest positive value among t, t1 and t2
        double ans[3] = {t1, t2, t}; int number = 3;
        //bubble sort

        if(ans[0] > ans[1]) std::swap(ans[0], ans[1]);
        if(ans[1] > ans[2]) std::swap(ans[1], ans[2]);
        if(ans[0] > ans[1]) std::swap(ans[0], ans[1]);
        if(ans[1] > ans[2]) std::swap(ans[1], ans[2]);
        double l = -1;
        for(int i = 0; i < number; i ++)
        {
            if(ans[i] > 0.0)
            {
                new_intersection = new_ray.origin + ans[i] * new_ray.direction;
                if(new_intersection.z > 0.0 && new_intersection.z < height)
                {
                    l = ans[i];
                    break;
                }
            }
        }
        if(l < 0.0) return false;
        intersection = ray->origin + l * ray->direction;
        normal = co.inv_trans_vector(this->normal(new_intersection));
        return true;
    }
};