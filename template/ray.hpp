//wirtten by Flager Lee

#pragma once

#include "vec3.hpp"
#include "Coordinate.hpp"
#include <stdio.h>
#include <random>
#include <time.h>

using std::default_random_engine;

//define ray

class Ray
{
    private:
    static default_random_engine e;
    public:
    vec3 origin;
    vec3 direction;
    
    Ray(vec3 _origin = vec3(), vec3 _direction = vec3()) : origin(_origin), direction(_direction) {}

    Ray& operator = (const Ray& ray)
    {
        this->origin = ray.origin;
        this->direction = ray.direction;
        return *this;
    }

    Ray diffuse(Ray in, vec3 point, vec3 normal) //get random diffused ray
    {
        Coordinate co(normal, point);
        double r = double(e()) / double(e.max()), theta = 2 * PI * double(e()) / double(e.max());
        vec3 out_point(sqrt(r) * cos(theta), sqrt(r) * sin(theta), sqrt(1.0 - r * r));
        return Ray(point, co.inv_trans_vector(out_point).normalize());
    }
    Ray reflect(Ray in, vec3 point, vec3 normal) //get reflected ray
    {
        double x = in.direction * normal;
        return Ray(point, in.direction - normal * x * 2);
    }
    double fersnel(Ray in, vec3 point, vec3 normal, double ratio)
    {
        double cos_theta = in.direction * normal;
        vec3 right_normal = cos_theta > 0 ? (-1) * normal : normal;
        double sin_theta = sqrt(1 - cos_theta * cos_theta);
        double sin_phi = sin_theta * (cos_theta > 0 ? ratio : 1.0/ratio);
        if(sin_phi > 1.0) return 1.0;
        double cos_phi = sqrt(1 - sin_phi * sin_phi);
        if(cos_theta > 0)
        {
            double squared_R_s = (ratio * cos_theta - cos_phi) / (ratio * cos_theta + cos_phi);
            double squared_R_p = (ratio * cos_phi - cos_theta) / (ratio * cos_phi + cos_theta);
            return (squared_R_s * squared_R_s + squared_R_p * squared_R_p) / 2;
        }
        else
        {
            double squared_R_s = (-cos_theta - ratio * cos_phi) / (-cos_theta + ratio * cos_phi);
            double squared_R_p = (cos_phi + ratio * cos_theta) / (cos_phi - ratio * cos_theta);
            return (squared_R_s * squared_R_s + squared_R_p * squared_R_p) / 2;
        }
    }
    Ray refract(Ray in, vec3 point, vec3 normal, double ratio)
    {
        double eta = 1.0 / ratio;
        if(in.direction * normal > 0)
        {
            normal = -1 * normal; //ray inside the object
            eta = ratio;
        }
        double dot_product = in.direction * normal;
        double delta = 1.0 - eta * eta * (1.0 - dot_product * dot_product);
        if(delta < 0.0) return reflect(in, point, normal);
        vec3 new_direction = in.direction * eta - normal * (dot_product * eta + sqrt(delta));
        new_direction.normalize();
        return Ray(point, new_direction);
    }
};

default_random_engine Ray::e = default_random_engine(time(NULL));