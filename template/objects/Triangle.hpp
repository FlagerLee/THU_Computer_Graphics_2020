#pragma once

//define triangles
#include "object.hpp"
#include <algorithm>

class Triangle : public object
{
    public:
    vec3 Ng;
    //in coordinate system co, z_axis is its normal, and x_axis is u, y_axis is v
    double u_length, v_length; //represents when u = 1 and v = 1, the value of x and y
    vec3 v_normal[3]; //vertex normal
    bool is_inv; //determine if the face normal direction is opposite to the default one
    Triangle(const vec3 point[], const vec3 _v_normal[], vec3 _color, vec3 _emission, double _ratio, Refl_t _type)
    {
        co.origin = point[1];
        co.x_axis = (point[0] - point[1]).normalize();
        co.y_axis = (point[2] - point[1]).normalize();
        Ng = (co.x_axis % co.y_axis).normalize();
        vec3 delta1 = point[0] - point[1];
        if(fabs(co.x_axis.x - 0.0) > eps && fabs(delta1.x - 0.0) > eps) u_length = delta1.x / co.x_axis.x;
        else if(fabs(co.x_axis.y - 0.0) > eps && fabs(delta1.y - 0.0) > eps) u_length = delta1.y / co.x_axis.y;
        else u_length = delta1.z / co.x_axis.z;
        vec3 delta2 = point[2] - point[1];
        if(fabs(co.y_axis.x - 0.0) > eps && fabs(delta2.x - 0.0) > eps) v_length = delta2.x / co.y_axis.x;
        else if(fabs(co.y_axis.y - 0.0) > eps && fabs(delta2.y - 0.0) > eps) v_length = delta2.y / co.y_axis.y;
        else v_length = delta2.z / co.y_axis.z;
        v_normal[0] = _v_normal[0]; v_normal[1] = _v_normal[1]; v_normal[2] = _v_normal[2];
        vec3 average_v_normal = (_v_normal[0] + _v_normal[1] + _v_normal[2]) / 3;
        //decide the direction of face normal
        if((co.x_axis % co.y_axis) * average_v_normal < 0.0)
        {
            std::swap(co.x_axis, co.y_axis);
            std::swap(u_length, v_length);
            is_inv = true;
        }
        else is_inv = false;
        co.z_axis = (co.x_axis % co.y_axis).normalize();
        color = _color;
        emission = _emission;
        ratio = _ratio;
        type = _type;
    }
    Triangle(const vec3 point[], vec3 _color, vec3 _emission, double _ratio, Refl_t _type)
    {
        co.origin = point[1];
        co.x_axis = (point[0] - point[1]).normalize();
        co.y_axis = (point[2] - point[1]).normalize();
        Ng = (co.x_axis % co.y_axis).normalize();
        vec3 delta1 = point[0] - point[1];
        if(fabs(co.x_axis.x - 0.0) > eps && fabs(delta1.x - 0.0) > eps) u_length = delta1.x / co.x_axis.x;
        else if(fabs(co.x_axis.y - 0.0) > eps && fabs(delta1.y - 0.0) > eps) u_length = delta1.y / co.x_axis.y;
        else u_length = delta1.z / co.x_axis.z;
        vec3 delta2 = point[2] - point[1];
        if(fabs(co.y_axis.x - 0.0) > eps && fabs(delta2.x - 0.0) > eps) v_length = delta2.x / co.y_axis.x;
        else if(fabs(co.y_axis.y - 0.0) > eps && fabs(delta2.y - 0.0) > eps) v_length = delta2.y / co.y_axis.y;
        else v_length = delta2.z / co.y_axis.z;
        is_inv = false;
        co.z_axis = (co.x_axis % co.y_axis).normalize();
        v_normal[0] = v_normal[1] = v_normal[2] = co.z_axis;
        color = _color;
        emission = _emission;
        ratio = _ratio;
        type = _type;
    }
    vec3 normal_g(){return Ng;}
    virtual vec3 normal(vec3 v)
    {
        if(is_inv) return v.x * v_normal[2] + v.y * v_normal[0] + (1 - v.x - v.y) * v_normal[1];
        else return v.x * v_normal[0] + v.y * v_normal[2] + (1 - v.x - v.y) * v_normal[1];
    }
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal, vec3& color)
    {
        Ray new_ray;
        if(!(co == Coordinate())) new_ray = Ray(co.trans_point(ray->origin), co.trans_vector(ray->direction));
        else new_ray = *ray;
        double t = -new_ray.origin.z / new_ray.direction.z;
        if(t < eps) return false;
        vec3 point = new_ray.origin + t * new_ray.direction;
        double u = point.x / u_length, v = point.y / v_length;
        if(u + v > 1.0 || u < 0.0 || v < 0.0) return false;
        intersection = ray->origin + t * ray->direction;
        normal = this->normal(vec3(u, v));
        color = this->color;
        //printf("intersection: %.5lf %.5lf %.5lf\nnormal: %.5f %.5f %.5f\n", intersection.x, intersection.y, intersection.z, normal.x, normal.y, normal.z);
        return true;
    }
};