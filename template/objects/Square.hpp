#pragma once

#include "object.hpp"
#include <random>
#include <time.h>

using std::default_random_engine;

class Square: public object
{
    private:
    double width, height; //x: width, y: height
    default_random_engine e;
    public:
    Square(double _width, double _height, vec3 _color, vec3 _emission, double _ratio, Coordinate _co, Refl_t _type)
    {
        width = _width;
        height = _height;
        color = _color;
        emission = _emission;
        ratio = _ratio;
        type = _type;
        co = _co;
        e.seed(time(NULL));
    }
    virtual vec3 normal(vec3 v){return co.z_axis;}
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal, vec3& color)
    {
        Ray new_ray;
        if(!(this->co == Coordinate())) new_ray = Ray(this->co.trans_point(ray->origin), this->co.trans_vector(ray->direction));
        else new_ray = *ray;
        double t = - new_ray.origin.z / new_ray.direction.z;
        if(t < eps) return false;
        vec3 hit_point = new_ray.origin + t * new_ray.direction;
        if(hit_point.x < 0.0 || hit_point.x > width || hit_point.y < 0.0 || hit_point.y > height) return false;
        intersection = ray->origin + t * ray->direction;
        normal = this->normal(vec3());
        if(texture.loaded()) color = texture.get_color(hit_point.x / width, hit_point.y / height);
        else color = this->color;
        return true;
    }
    virtual Ray random_emit(vec3& Color, vec3& Normal)
    {
        double random_w = double(e()) / double(e.max());
        double random_h = double(e()) / double(e.max());
        if(texture.loaded()) Color = texture.get_color(random_w, random_h);
        else Color = color;
        Normal = normal(vec3());
        vec3 point(random_w * width, random_h * height);
        Ray in(point, (0, 0, -1));
        Ray out = in.diffuse(in, point, vec3(0, 0, 1));
        return out;
    }
};