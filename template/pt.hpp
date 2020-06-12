#pragma once

//path tracing engine
#include "ray.hpp"
#include "Scene.hpp"

const int MAXIMUM_DEPTH = 5; //maximum recursive depth

vec3 pt(Ray ray, int depth, std::vector<object*> scene)
{
    if(depth > MAXIMUM_DEPTH) return vec3(0, 0, 0); //keep the origin color
    int scene_size = scene.size();
    int intersect_id = -1;
    double minimum_distance = INF;
    vec3 intersection, normal, color;
    //find closest object
    for(int i = 0; i < scene_size; i ++)
    {
        vec3 new_intersection, new_normal, new_color;
        if(scene[i]->intersect(&ray, new_intersection, new_normal, new_color))
        {
            double new_distance = (new_intersection - ray.origin).length();
            if(new_distance < minimum_distance)
            {
                minimum_distance = new_distance;
                intersection = new_intersection;
                normal = new_normal;
                color = new_color;
                intersect_id = i;
            }
        }
    }
    if(intersect_id != -1)
    {
        const object* obj = scene[intersect_id];
        double R;
        Ray new_ray;
        vec3 ans;
        switch (obj->type)
        {
        case DIFF:
            new_ray = ray.diffuse(ray, intersection, normal);
            ans = obj->emission + (color ^ pt(new_ray, depth + 1, scene));
            break;

        case SPEC:
            new_ray = ray.reflect(ray, intersection, normal);
            ans = obj->emission + (color ^ pt(new_ray, depth + 1, scene));
            break;

        case REFR:
            R = ray.fersnel(ray, intersection, normal, obj->ratio);
            new_ray = ray.reflect(ray, intersection, normal);
            if(fabs(R - 1.0) < eps) ans = obj->emission + (color ^ pt(new_ray, depth + 1, scene));
            else
            {
                Ray new_refr_ray = ray.refract(ray, intersection, normal, obj->ratio);
                ans = obj->emission + (color ^ (R * pt(new_ray, depth + 1, scene) + (1.0 - R) * pt(new_refr_ray, depth + 1, scene)));
            }
            break;
        
        default:
            fprintf(stderr, "Unrecognized surface type");
            return vec3(1, 1, 1);
            break;
        }
        if(isnan(ans.x)) puts("Error");
        /*
        if(intersect_id == 0)
        {
            printf("position: %.5lf %.5lf %.5lf\nnormal: %.5lf %.5lf %.5lf\nout: %.5lf %.5lf %.5lf\n", intersection.x, intersection.y, intersection.z, normal.x, normal.y, normal.z, new_ray.direction.x, new_ray.direction.y, new_ray.direction.z);
        }
        */
        return ans;
    }
    return vec3(0, 0, 0);
}