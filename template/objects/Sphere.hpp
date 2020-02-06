#pragma once

#include "object.hpp"

class Sphere : public object
{
    public:
    vec3 center;
    double radius;

    Sphere(vec3 _center, double _radius, vec3 _color, vec3 _emission, double _ratio, Coordinate _co, Refl_t _type)
    {
        center = _center;
        radius = _radius;
        color = _color;
        emission = _emission;
        ratio = _ratio;
        co = _co;
        type = _type;
    }
    
    virtual vec3 normal(vec3 v) const
    {
        return (v - this->center).normalize();
    }
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal) const
    {
        Ray new_ray;
        if(!(this->co == Coordinate())) new_ray = Ray(this->co.trans_point(ray->origin), this->co.trans_vector(ray->direction));
        else new_ray = *ray;
        vec3 e = new_ray.origin - this->center;
        double B = new_ray.direction * e;
        double C = e * e - this->radius * this->radius;
        double delta = B * B - C;
        if(delta < 0.0) return false;
        double s_delta = sqrt(delta);
        if(-B - s_delta > eps) intersection = ray->origin + (-B - s_delta) * ray->direction;
        else if(-B + s_delta > eps) intersection = ray->origin + (-B + s_delta) * ray->direction;
        else return false;
        normal = co.inv_trans_vector(this->normal(intersection));
        return true;
    }
};