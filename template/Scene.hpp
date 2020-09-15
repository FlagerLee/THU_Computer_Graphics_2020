#pragma once

//using objects to build scens

#include "Coordinate.hpp"
#include "objects/Plane.hpp"
#include "objects/Circle.hpp"
#include "objects/Sphere.hpp"
#include "objects/Cylinder.hpp"
#include "objects/r_Bezier.hpp"
#include "objects/Triangle.hpp"
#include "objects/Mesh.hpp"
#include "Loadobj.hpp"
#include <fstream>
#include <string>

vec3 controll[] =
{
    vec3(7.50, 25.0),
    vec3(9.375, 25.0),
    vec3(9.375, 21.875),
    vec3(9.375, 18.75),
    vec3(9.375, 15.625),
    vec3(7.8125, 12.5),
    vec3(6.25,  6.25),
    vec3(4.6875,  3.125),
    vec3(8.4375,  0.0)
};



object* scene[8];

void init_scene()
{
    printf("Loading Object\n");
    //scene[0] = new r_Bezier(controll, 9, vec3(1, 1, 1), vec3(), 0, Coordinate(vec3(-1, 0, 0), vec3(0, 0, 1), vec3(0, -1, 0), vec3(0, 25, 0)), DIFF);
    //const char* texture_name = "/users/flagerlee/desktop/files/tsinghua/graphic/prj_2/vase.png";
    //scene[0]->texture.load_texture(texture_name);
    
    std::string obj_file_path = "/users/flagerlee/desktop/files/tsinghua/graphic/prj_2/waves.obj";
    Mesh* M = new Mesh(25, vec3(1, 1, 1), vec3(), 1.5, Coordinate(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), vec3(0, 0, 0)), REFR);
    Loadobj(M, obj_file_path);
    scene[0] = M;
    
    scene[1] = new Plane(vec3(), vec3(0, 1, 0), vec3(1, 1, 1), vec3(), 0, Coordinate(), DIFF);
    scene[2] = new Plane(vec3(0, 50, 0), vec3(0, -1, 0), vec3(1, 1, 1), vec3(), 0, Coordinate(), DIFF);
    scene[3] = new Plane(vec3(0, 0, -50), vec3(0, 0, 1), vec3(1, 1, 1), vec3(), 0, Coordinate(), DIFF);
    scene[4] = new Plane(vec3(25, 0, 0), vec3(-1, 0, 0), vec3(0.75, 0.25, 0.25), vec3(), 0, Coordinate(), DIFF);
    scene[5] = new Plane(vec3(-25, 0, 0), vec3(1, 0, 0), vec3(0.25, 0.25, 0.75), vec3(), 0, Coordinate(), DIFF);
    //scene[6] = new Sphere(vec3(0, 99, 0), 50.0, vec3(), vec3(12, 12, 12), 0, Coordinate(), DIFF);
    scene[7] = new Sphere(vec3(15, 15, 2), 10.0, vec3(.25, .75, .75), vec3(), 1.5, Coordinate(), DIFF);
    scene[6] = new Circle(10.0, vec3(1, 1, 1), vec3(1, 1, 1), 0, Coordinate(vec3(1, 0, 0), vec3(0, 0, 1), vec3(0, -1, 0), vec3(0, 49.99, 0)), DIFF);
    //scene[6] = new Sphere(vec3(0, 47, 47), 3.0, vec3(1, 1, 1), vec3(12, 12, 12), 0, Coordinate(), DIFF);
    printf("Finished\n");
}
