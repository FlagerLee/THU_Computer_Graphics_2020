#include "template/pt.hpp"
#include "template/ppm.hpp"
#include "template/Parser.hpp"
#include <time.h>
#include <random>

using std::default_random_engine;

int main(int argc, char** argv)
{
    srand(time(NULL));
    Parser scene_parser(argv[1]);
    Camera* cam = scene_parser.Get_Camera();
    Scene* light_obj = scene_parser.Get_Light();
    Scene* obj = scene_parser.Get_Objects();
    Algo a = scene_parser.Get_Algorithm();
    
    std::vector<object*> objects = light_obj->Objects;
    objects.insert(objects.end(), obj->Objects.begin(), obj->Objects.end());
    if(a == PPM) ppm(*cam, 100000, objects);
    else
    {
        int spp = 100;
        default_random_engine e(time(NULL));
        int width = cam->x_pixel, height = cam->y_pixel;
        const int total_pixels = width * height;
        vec3* picture = new vec3[total_pixels];
        vec3 r;
        #pragma omp parallel for schedule(dynamic, 1) private(r)
            for(int h = 0; h < height; h ++)
            {
                fprintf(stderr, "\rRendering %5.2lf%% (%dspp)", 100.0 * (double)(h + 1) / (double)height, spp);
                for(int w = 0; w < width; w ++)
                {
                    r = vec3(0, 0, 0);
                    for(int s = 0; s < spp; s ++)
                    {
                        for(int ssaa = 0; ssaa < 4; ssaa ++)
                        {
                            Ray ray;
                            switch(ssaa)
                            {
                                case 0:
                                    ray = cam->get_ordinary_ray(w + double(e()) / double(e.max()) - 0.5, h + double(e()) / double(e.max()) - 0.5);
                                    r = r + pt(ray, 1, objects);
                                    break;
                                case 1:
                                    ray = cam->get_ordinary_ray(w + double(e()) / double(e.max()) + 0.5, h + double(e()) / double(e.max()) - 0.5);
                                    r = r + pt(ray, 1, objects);
                                    break;
                                case 2:
                                    ray = cam->get_ordinary_ray(w + double(e()) / double(e.max()) - 0.5, h + double(e()) / double(e.max()) + 0.5);
                                    r = r + pt(ray, 1, objects);
                                    break;
                                case 3:
                                    ray = cam->get_ordinary_ray(w + double(e()) / double(e.max()) + 0.5, h + double(e()) / double(e.max()) + 0.5);
                                    r = r + pt(ray, 1, objects);
                                    break;
                            }
                        }
                        
                    }
                    picture[h * width + w] = r / (double)(spp * 4);
                }
            }
        printf("intersect times: %d\n", width * height * 5 * 100);
        printf("\nWriting Image\n");
        FILE* image = fopen("image.ppm", "w");
        fprintf(image, "P3\n%d %d\n%d\n", width, height, 255);
        for(int i = 0; i < total_pixels; i ++)
        fprintf(image, "%d %d %d ", gamma_trans(picture[i].x), gamma_trans(picture[i].y), gamma_trans(picture[i].z));
        printf("Finished\n");
        
        return 0;
    }
    
    /*
    srand(time(NULL));
    init_scene();
    int spp = 4;
    if(argc > 1) spp = atoi(argv[1]);
    const int width = 1024, height = 1024;
    vec3 camera_position(0, 25, 100);
    Camera camera(Coordinate(vec3(-1, 0, 0), vec3(0, -1, 0), vec3(0, 0, -1), camera_position),
                  PI/5, PI/5, width, height);
    ppm(camera, 100000);
    */
    /*
    const int total_pixels = width * height;
    vec3* picture = new vec3[total_pixels];
    vec3 r;
    #pragma omp parallel for schedule(dynamic, 1) private(r)
        for(int h = 0; h < height; h ++)
        {
            fprintf(stderr, "\rRendering %5.2lf%% (%dspp)", 100.0 * (double)(h + 1) / (double)height, spp);
            for(int w = 0; w < width; w ++)
            {
                r = vec3(0, 0, 0);
                for(int s = 0; s < spp; s ++)
                {
                    Ray ray = camera.get_ordinary_ray(w + frand() * 2 - 1.0, h + frand() * 2 - 1.0);
                    r = r + pt(ray, 1);
                }
                picture[h * width + w] = r / (double)spp;
            }
        }
    printf("intersect times: %d\n", width * height * 5 * 100);
    printf("\nWriting Image\n");
    FILE* image = fopen("image.ppm", "w");
    fprintf(image, "P3\n%d %d\n%d\n", width, height, 255);
    for(int i = 0; i < total_pixels; i ++)
    fprintf(image, "%d %d %d ", gamma_trans(picture[i].x), gamma_trans(picture[i].y), gamma_trans(picture[i].z));
    printf("Finished\n");
    
    return 0;
    */
}