#pragma once

#include "object.hpp"
#include <random>
#include <time.h>

using std::default_random_engine;

class Circle : public object
{
    private:
    default_random_engine e;
    public:
    double radius;
    //the circle is on xoy plane
    //its center is (0, 0, 0), and its normal is (0, 0, 1)

    Circle(double _radius, vec3 _color, vec3 _emission, double _ratio, Coordinate _co, Refl_t _type)
    {
        radius = _radius;
        color = _color;
        emission = _emission;
        ratio = _ratio;
        co = _co;
        type = _type;
        e.seed(time(NULL));
    }

    virtual vec3 normal(vec3 v) {return vec3(0, 0, 1);}
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal, vec3& color)
    {
        Ray new_ray;
        if(!(co == Coordinate())) new_ray = Ray(co.trans_point(ray->origin), co.trans_vector(ray->direction));
        else new_ray = *ray;
        if(fabs(new_ray.direction.z) < eps) return false;
        double t = (0.0 - new_ray.origin.z) / new_ray.direction.z;
        if(t < eps) return false;
        vec3 point = new_ray.origin + t * new_ray.direction;
        if(point.length() < radius) //hit
        {
            intersection = co.inv_trans_point(point);
            normal = co.inv_trans_vector(this->normal(vec3()));
            color = this->color;
            return true;
        }
        else return false;
    }

    virtual Ray random_emit(vec3& Color, vec3& Normal)
    {
        //first find out the origin of the emitted ray
        Color = vec3(1, 1, 1);
        Normal = co.inv_trans_vector(normal(vec3()));
        double r = radius * double(e()) / double(e.max());
        double theta = 2 * PI * double(e()) / double(e.max());
        vec3 origin = co.inv_trans_point(vec3(r * cos(theta), r * sin(theta)));

        //then find out the direction of the emitted ray, using diffuse model
        double radi = double(e()) / double(e.max()), angle = 2 * PI * double(e()) / double(e.max());
        vec3 direction = co.inv_trans_vector(vec3(sqrt(radi) * cos(theta), sqrt(radi) * sin(theta), sqrt(1.0 - radi * radi)));

        return Ray(origin, direction.normalize());
        //return Ray(origin, Normal);
    }
};