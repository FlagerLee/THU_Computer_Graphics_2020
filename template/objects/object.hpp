#pragma once

#include "../ray.hpp"
#include "../vec3.hpp"
#include "../utils.hpp"
#include "../Coordinate.hpp"

class object
{
    //base class
    public:
    Refl_t type;
    vec3 color, emission;
    double ratio; //refraction ratio
    Coordinate co; //new coordinate system
    //need to add texture and normal map
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal) const = 0;
    virtual vec3 normal(vec3 v) const = 0; //get normal vector
    //need to add function to load texture and normal map
};