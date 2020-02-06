#pragma once

//define triangle mesh
#include "Triangle.hpp"
#include "datastructure/kdtree.hpp"
#include <vector>

class Mesh : public object
{
    public:
    std::vector<Triangle*> T;
    kdtree tree;
    double times; //magnification of the model

    Mesh(std::vector<Triangle*> _T, double _times, vec3 _color, vec3 _emission, double _ratio, Coordinate _co, Refl_t _type)
    {
        T.assign(_T.begin(), _T.end());
        tree.build(T);
        times = _times;
        color = _color;
        emission = _emission;
        ratio = _ratio;
        co = _co;
        type = _type;
    }
    Mesh(double _times, vec3 _color, vec3 _emission, double _ratio, Coordinate _co, Refl_t _type)
    {
        times = _times;
        color = _color;
        emission = _emission;
        ratio = _ratio;
        co = _co;
        type = _type;
    }
    ~Mesh()
    {
        for(auto iter = T.begin(); iter != T.end(); iter ++) if(*iter) delete(*iter);
    }

    virtual vec3 normal(vec3 v) const {return vec3(1, 0, 0);} //do not use this
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal) const
    {
        std::vector<object*> possible = tree.query(*ray, tree.root);
        vec3 new_intersection, new_normal;
        double distance = INF;
        for(auto iter = possible.begin(); iter != possible.end(); iter ++)
        {
            if((*iter)->intersect(ray, new_intersection, new_normal))
            {
                double new_distance = (new_intersection - ray->origin).length();
                if(new_distance < distance)
                {
                    distance = new_distance;
                    intersection = new_intersection;
                    normal = new_normal;
                }
            }
        } 
        if(fabs(distance - INF) < eps) return false;
        return true;
    }
};