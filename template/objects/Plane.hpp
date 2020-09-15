#pragma once

#include "object.hpp"

class Plane : public object
{
    //infinite plane
    public:
    vec3 origin, Normal;

    Plane(vec3 _origin, vec3 _normal, vec3 _color, vec3 _emission, double _ratio, Coordinate _co, Refl_t _type)
    {
        origin = _origin;
        Normal = _normal;
        color = _color;
        emission = _emission;
        ratio = _ratio;
        co = _co;
        type = _type;
    }

    virtual vec3 normal(vec3 v)
    {
        return this->Normal;
    }
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal, vec3& color)
    {
        Ray new_ray;
        if(!(this->co == Coordinate())) new_ray = Ray(this->co.trans_point(ray->origin), this->co.trans_vector(ray->direction));
        else new_ray = *ray;
        double d = new_ray.direction * this->Normal;
        if(fabs(d - 0.0) < eps) return false; //parallel
        double t = (this->origin - new_ray.origin) * this->Normal / d;
        if(t < eps) return false;
        intersection = ray->origin + t * ray->direction;
        normal = co.inv_trans_vector(this->Normal);
        color = this->color;
        return true;
    }
};
