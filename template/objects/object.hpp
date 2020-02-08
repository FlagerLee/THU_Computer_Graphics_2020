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
    Texture() {}
    unsigned int load_texture(const char* filename);
    vec3 get_color(double w, double h); //w = (column id)/(number of columns), the same as h
    bool loaded() {return texture.size() != 0;}
    void test()
    {
        FILE* f = fopen("test.ppm", "w");
        fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
        for(int i = 0; i < width * height; i ++)
        {
            fprintf(f, "%d %d %d ", texture[i * 4], texture[i * 4 + 1], texture[i * 4 + 2]);
            fflush(f);
        }
    }
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
    //need to add texture and normal map
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal, vec3& color) = 0;
    virtual vec3 normal(vec3 v) = 0; //get normal vector
    //need to add function to load texture and normal map
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
    int r = h * height, c = w * width;
    int pos = r * width + c;
    vec3 color;
    color.x = (double)texture[pos * 4] / 255.0;
    color.y = (double)texture[pos * 4 + 1] / 255.0;
    color.z = (double)texture[pos * 4 + 2] / 255.0;
    //printf("%.5lf %.5lf %.5lf\n", color.x, color.y, color.z);
    return color;
}