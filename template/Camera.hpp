#pragma once

//define the position and direction of camera
//define image surface
//define camera type
#include "ray.hpp"
#include "Coordinate.hpp"
#include <math.h>
#include <time.h>
#include <random>

enum Camera_type{Ordinary, Fish};

using std::default_random_engine;

class Camera
{
    private:
    double radius; //DOF
    double f; //DOF
    default_random_engine e;
    public:
    const Coordinate co; //in this coordinate system, camera faces (0, 0, 1)
    const double x_angle, y_angle; //field angle
    const int x_pixel, y_pixel;
    double x_length, y_length; //size of image surface

    Camera(Coordinate _co, double _x_angle, double _y_angle, int _x_pixel, int _y_pixel, double _radius, double _f) :
    co(_co), x_angle(_x_angle), y_angle(_y_angle), x_pixel(_x_pixel), y_pixel(_y_pixel), radius(_radius), f(_f)
    {
        x_length = 2 * tan(x_angle / 2);
        y_length = 2 * tan(y_angle / 2);
        e.seed(time(NULL));
    }
    
    Ray get_ordinary_ray(double x, double y)
    {
        double delta_x = 2 * f * ((double)(x_pixel - x * 2) / (double)x_pixel) * x_length / 2;
        double delta_y = 2 * f * ((double)(y * 2 - y_pixel) / (double)y_pixel) * y_length / 2;
        if(fabs(radius) < eps)
        {
            Ray ray(co.origin, co.inv_trans_vector(vec3(delta_x, delta_y, 2 * f).normalize()));
            return ray;
        }
        else
        {
            double r = (double)e() / (double)e.max();
            double theta = 2.0 * PI * (double)e() / (double)e.max();
            vec3 lens_pos(r * radius * cos(theta), r * radius * sin(theta));
            vec3 img_pos(delta_x, delta_y, 2 * f);
            Ray ray(lens_pos, (img_pos - lens_pos).normalize());
            return Ray(co.inv_trans_point(ray.origin), co.inv_trans_vector(ray.direction).normalize());
        }
    }
    Ray get_fish_ray(double x, double y)
    {
        double delta_x = tan(x_angle * ((double)(x * 2 - x_pixel) / (double)x_pixel));
        double delta_y = tan(y_angle * ((double)(y * 2 - y_pixel) / (double)y_pixel));
        return Ray(co.origin, co.inv_trans_vector(vec3(delta_x, delta_y, 1.0).normalize()));
    }
};