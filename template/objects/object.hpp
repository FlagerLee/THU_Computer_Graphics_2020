#pragma once

#include "../ray.hpp"
#include "../vec3.hpp"
#include "../utils.hpp"
#include "../Coordinate.hpp"
#include "../png/lodepng.h"
#include <vector>

class Texture
{
    private:
    std::vector<unsigned char> texture; //texture map
    unsigned int width, height; //width and height of the texture picture
    public:
    Texture() {texture.clear();}
    unsigned int load_texture(const char* filename);
    vec3 get_color(double w, double h); //w = (column id)/(number of columns), the same as h
    bool loaded() {return texture.size() != 0;}
};

class object
{
    //base class
    public:
    Refl_t type;
    vec3 color, emission;
    double ratio; //refraction ratio
    Coordinate co; //new coordinate system
    Texture texture; //texture map
    Texture normal_texture; //normal texture map
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal, vec3& color) = 0;
    virtual vec3 normal(vec3 v) = 0; //get normal vector
    virtual Ray random_emit(vec3& Color, vec3& Normal) //ppm
    {
        fprintf(stderr, "Do not exist overloaded function \"random_emit()\"\n");
        return Ray();
    }
};

unsigned int Texture::load_texture(const char* filename)
{
    unsigned int error = lodepng::decode(texture, width, height, filename);
    if(error != 0)
    {
        printf("Decode error %d: ", error);
        printf("%s\n", lodepng_error_text(error));
    }
    assert(texture.size() == width * height * 4);
    return error;
}

vec3 Texture::get_color(double w, double h)
{
    if(w < 0.0) w = 0.0;
    else if(w > 1.0) w = 1.0;
    if(h < 0.0) h = 0.0;
    else if(h > 1.0) h = 1.0;
    w = 1.0 - w;
    h = 1.0 - h;
    int r = h * height, c = w * width;
    int pos = r * width + c;
    vec3 color;
    color.x = (double)texture[pos * 4] / 255.0;
    color.y = (double)texture[pos * 4 + 1] / 255.0;
    color.z = (double)texture[pos * 4 + 2] / 255.0;
    //printf("%.5lf %.5lf %.5lf\n", color.x, color.y, color.z);
    return color;
}