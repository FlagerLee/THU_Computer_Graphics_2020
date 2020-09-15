#pragma once

#include "Square.hpp"
#include <string>
#include <iostream>
#include <random>
#include <time.h>

using std::string;
using std::default_random_engine;

class Box: public object
{
    private:
    Square* faces[6];
    default_random_engine e;
    public:
    Box(vec3 _size, string textures[], vec3 _color, vec3 _emission, double _ratio, Coordinate _co, Refl_t _type)
    {
        //size: (long, width, height)/(x, y, z)
        faces[1] = new Square(_size.x, _size.y, _color, _emission, _ratio, Coordinate(_co.x_axis, _co.y_axis, _co.z_axis, _co.origin + _size.z * _co.z_axis), _type); //up
        faces[0] = new Square(_size.x, _size.y, _color, _emission, _ratio, Coordinate(-_co.x_axis, _co.y_axis, -_co.z_axis, _co.origin + _size.x * _co.x_axis), _type); //down
        faces[2] = new Square(_size.y, _size.z, _color, _emission, _ratio, Coordinate(-_co.y_axis, _co.z_axis, -_co.x_axis, _co.origin + _size.y * _co.y_axis), _type); //left
        faces[3] = new Square(_size.y, _size.z, _color, _emission, _ratio, Coordinate(_co.y_axis, _co.z_axis, _co.x_axis, _co.origin + _size.x * _co.x_axis), _type); //right
        faces[4] = new Square(_size.x, _size.z, _color, _emission, _ratio, Coordinate(_co.x_axis, _co.z_axis, -_co.y_axis, _co.origin), _type); //front
        faces[5] = new Square(_size.x, _size.z, _color, _emission, _ratio, Coordinate(-_co.x_axis, _co.z_axis, _co.y_axis, _co.origin + _size.x * _co.x_axis + _size.y * _co.y_axis), _type); //back
        if(textures[0] != "") faces[0]->texture.load_texture(textures[0].c_str());
        if(textures[1] != "") faces[1]->texture.load_texture(textures[1].c_str());
        if(textures[2] != "") faces[2]->texture.load_texture(textures[2].c_str());
        if(textures[3] != "") faces[3]->texture.load_texture(textures[3].c_str());
        if(textures[4] != "") faces[4]->texture.load_texture(textures[4].c_str());
        if(textures[5] != "") faces[5]->texture.load_texture(textures[5].c_str());
        e.seed(time(NULL));
        emission = _emission;
    }
    virtual vec3 normal(vec3 v)
    {
        std::cerr << "You shouldn't use function 'normal' in box.hpp\n";
        return vec3(0, 0, 0);
    }
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal, vec3& color)
    {
        double t = INF;
        vec3 new_color, new_intersection, new_normal;
        for(int i = 0; i < 6; i ++)
        {
            if(faces[i]->intersect(ray, new_intersection, new_normal, new_color))
            {
                double dist = (new_intersection - ray->origin).length();
                if(t > dist)
                {
                    t = dist;
                    color = new_color;
                    normal = new_normal;
                    intersection = new_intersection;
                }
            }
        }
        if(fabs(t - INF) < eps) return false;
        return true;
    }
    virtual Ray random_emit(vec3& Color, vec3& Normal)
    {
        int i = e() % 6;
        return faces[i]->random_emit(Color, Normal);
    }
};