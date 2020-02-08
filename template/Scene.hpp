#pragma once

//using objects to build scens

#include "Coordinate.hpp"
#include "objects/Plane.hpp"
#include "objects/Sphere.hpp"
#include "objects/Cylinder.hpp"
#include "objects/r_Bezier.hpp"
#include "objects/Triangle.hpp"
#include "objects/Mesh.hpp"
#include "Loadobj.hpp"
#include <fstream>
#include <string>
/*
const vec3 controll[] =
{
    vec3(13.5),
    vec3(7.5, 5.0),
    vec3(10.0, 10.0),
    vec3(12.5, 20.0),
    vec3(15.0, 25.0),
    vec3(15.0, 30.0),
    vec3(15.0, 35.0),
    vec3(15.0, 40.0),
    vec3(12.5, 40.0)
};
*/
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

/*
const vec3 tri[3] = {vec3(-5, 10, 5), vec3(0, 15, -5), vec3(5, 10, 5)};
const vec3 tri2[3] = {vec3(-5, 10, 5), vec3(0, 20, 0), vec3(0, 15, -5)};
const vec3 tri3[3] = {vec3(5, 10, 5), vec3(0, 20, 0), vec3(0, 15, -5)};
const vec3 norm[3] = {vec3(-1, 1, 1).normalize(), vec3(0, 1, 0).normalize(), vec3(1, 1, 1).normalize()};
const vec3 norm2[3] = {vec3(1, 1, 1).normalize(), vec3(0, 1, 0).normalize(), vec3(1, 0, 0).normalize()};

const object* scene[] = 
{
    new Plane(vec3(), vec3(0, 1, 0), vec3(0.25, 0, 0), vec3(), 0, Coordinate(), DIFF),
    new Plane(vec3(0, 50, 0), vec3(0, -1, 0), vec3(0, 0, 1.0), vec3(), 0, Coordinate(), DIFF),
    new Plane(vec3(0, 0, -50), vec3(0, 0, 1), vec3(1, 1, 1), vec3(), 0, Coordinate(), DIFF),
    new Plane(vec3(25, 0, 0), vec3(-1, 0, 0), vec3(1, 1, 1), vec3(), 0, Coordinate(), DIFF),
    new Plane(vec3(-25, 0, 0), vec3(1, 0, 0), vec3(1, 1, 1), vec3(), 0, Coordinate(), DIFF),
    new Sphere(vec3(0, 99, 0), 50.0, vec3(), vec3(12, 12, 12), 0, Coordinate(), DIFF),
    //new Sphere(vec3(10, 10, 4), 10.0, vec3(1, 1, 1), vec3(), 1.5, Coordinate(), SPEC),
    //new Sphere(vec3(-10, 7, -6), 7, vec3(1, 1, 1), vec3(), 1.5, Coordinate(), REFR),
    //new Sphere(vec3(-5.0, 2.0, 12.0), 2, vec3(278.0/256.0, 190.0/256.0, 195.0/256.0), vec3(), 0, Coordinate(), DIFF)
    //#55efc4
    //new Cylinder(5.0, 16.0, vec3(85.0/256.0, 239.0/256.0, 200.0/256.0), vec3(), 0, Coordinate(vec3(0, 0, 1), vec3(1, 0, 0), vec3(0, 1, 0), vec3(-2, 0, -20)), DIFF)
    //new r_Bezier(controll, 9, vec3(85.0/256.0, 239.0/256.0, 200.0/256.0), vec3(), 0, Coordinate(vec3(0, 0, 1), vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, -15)), DIFF)
    new Triangle(tri, vec3(0.25, 0.75, 0.25), vec3(), 0, DIFF),
    new Triangle(tri2, vec3(0.75, 0.25, 0.25), vec3(), 0, SPEC),
    new Triangle(tri3, vec3(0.25, 0.25, 0.75), vec3(), 0, SPEC)
};

const int scene_size = 9;
*/

object* scene[7];
int scene_size = 7;

void init_scene()
{
    printf("Loading Object\n");
    scene[0] = new r_Bezier(controll, 9, vec3(1, 1, 1), vec3(), 0, Coordinate(vec3(-1, 0, 0), vec3(0, 0, 1), vec3(0, -1, 0), vec3(0, 25, 0)), DIFF);
    const char* texture_name = "/users/flagerlee/desktop/files/tsinghua/graphic/prj_2/vase.png";
    scene[0]->texture.load_texture(texture_name);
    /*
    std::string obj_file_path = "/users/flagerlee/desktop/files/tsinghua/graphic/prj_2/bunny.obj";
    std::fstream obj_file;
    obj_file.open(obj_file_path, std::ios::in);
    if(!obj_file.good()) puts("Open file error!");
    Mesh* M = new Mesh(200.0, vec3(.75, .75, .75), vec3(), 1.5, Coordinate(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), vec3(5, 0, 0)), REFR);
    Loadobj(M, obj_file);
    obj_file.close();
    scene[0] = M;
    */
    scene[1] = new Plane(vec3(), vec3(0, 1, 0), vec3(1, 1, 1), vec3(), 0, Coordinate(), DIFF);
    scene[2] = new Plane(vec3(0, 50, 0), vec3(0, -1, 0), vec3(1, 1, 1), vec3(), 0, Coordinate(), DIFF);
    scene[3] = new Plane(vec3(0, 0, -50), vec3(0, 0, 1), vec3(1, 1, 1), vec3(), 0, Coordinate(), DIFF);
    scene[4] = new Plane(vec3(25, 0, 0), vec3(-1, 0, 0), vec3(0.75, 0.25, 0.25), vec3(), 0, Coordinate(), DIFF);
    scene[5] = new Plane(vec3(-25, 0, 0), vec3(1, 0, 0), vec3(0.25, 0.25, 0.75), vec3(), 0, Coordinate(), DIFF);
    scene[6] = new Sphere(vec3(0, 99, 0), 50.0, vec3(), vec3(12, 12, 12), 0, Coordinate(), DIFF);
    printf("Finished\n");
}