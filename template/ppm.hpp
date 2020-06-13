# pragma once

#include "Camera.hpp"
#include <vector>
#include <algorithm>

class hitpoint
{
    public:
    vec3 position;
    vec3 normal;
    vec3 dir;
    int w, h; //pixel location
    vec3 color;
    static double radius;
    static double decrease;
    double weight;
    long long N;
    long long M;
    vec3 flux;
    vec3 M_flux;
    static constexpr double alpha = 0.7;

    hitpoint(vec3 _pos, vec3 _normal, vec3 _color, int _w, int _h, double _weight)
    {
        position = _pos;
        normal = _normal;
        color = _color;
        w = _w;
        h = _h;
        flux = vec3();
        M_flux = vec3();
        N = 0;
        M = 0;
        weight = _weight;
        left = nullptr;
        right = nullptr;
    }

    //these parameters are used for building kdtrees and querying
    hitpoint* left;
    hitpoint* right;
    int dimention; //0: compare x; 1: compare y; 2: compare z
};

double hitpoint::radius = 1.0;
double hitpoint::decrease = 0.0;

bool cmp(hitpoint* a, hitpoint* b, int dimention)
{
    if(dimention == 0) return a->position.x < b->position.x;
    if(dimention == 1) return a->position.y < b->position.y;
    return a->position.z < b->position.z;
}

bool cmpx(hitpoint* a, hitpoint* b)
{
    return a->position.x < b->position.x;
}
bool cmpy(hitpoint* a, hitpoint* b)
{
    return a->position.y < b->position.y;
}
bool cmpz(hitpoint* a, hitpoint* b)
{
    return a->position.z < b->position.z;
}

typedef std::vector<hitpoint*>::iterator hpkditer;

void find_mid(hpkditer Begin, hpkditer End, int dimention)
{
    int size = End - Begin;
    if(size <= 2) return;
    hpkditer mid = Begin + size / 2;
    for(hpkditer lo = Begin, hi = End - 1; hi - lo > 0;)
    {
        hpkditer i = lo, j = hi;
        hitpoint* pivot = *lo;
        while(j - i > 0)
        {
            while(j - i > 0 && cmp(pivot->position, (*j)->position, dimention))
            *i = *j;
            while(j - i > 0 && cmp((*i)->position, pivot->position, dimention)) i++;
            *j = *i;
        }
        *i = pivot;
        if(i - mid >= 0) hi = i - 1;
        if(mid - i >= 0) lo = i + 1;
    }
    return;
}
hitpoint* build_tree(hpkditer Begin, hpkditer End, int dimention)
{
    if(End - Begin < 1) return nullptr;
    if(End - Begin == 1)
    {
        (*Begin)->dimention = dimention;
        return *Begin;
    }
    if(End - Begin == 2)
    {
        if(cmp(*Begin, *(Begin + 1), dimention)) (*Begin)->right = *(Begin + 1);
        else (*Begin)->left = *(Begin + 1);
        (*Begin)->dimention = dimention;
        return *Begin;
    }
    //find_mid(Begin, End, dimention);
    if(dimention == 0) std::sort(Begin, End, cmpx);
    else if(dimention == 1) std::sort(Begin, End, cmpy);
    else std::sort(Begin, End, cmpz);
    hpkditer Mid = Begin + (End - Begin) / 2;
    (*Mid)->left = build_tree(Begin, Mid, (dimention + 1) % 3);
    (*Mid)->right = build_tree(Mid + 1, End, (dimention + 1) % 3);
    (*Mid)->dimention = dimention;
    return *Mid;
}

class photon
{
    public:
    vec3 point; //hit point
    vec3 in_dir; //ray's direction
    vec3 color;

    photon(vec3 _p, vec3 _i, vec3 _c)
    {
        point = _p;
        in_dir = _i;
        color = _c;
    }
};

void query(hitpoint* node, photon p)
{
    if(node == nullptr) return;
    if((p.point - node->position).length() < node->radius)
    {
        node->M ++;
        //BRDF: f(l, v) = C_diff / PI = surface's color
        //cos = p.in_dir * node->normal < 0
        node->M_flux = node->M_flux - (p.color ^ node->color) * (p.in_dir * node->normal);
        if(p.point.x > -20.0 && p.point.x < 20.0 && p.point.y > 5.0 && p.point.y < 20.0 && p.point.z > -20 && p.point.z < 20)
        {
            //std::cerr << "p.color: " << p.color << "; node->color: " << node->color << "; p.in_dir: " << p.in_dir << "; node->normal: " << node->normal << std::endl;
            //std::cerr << (p.color ^ node->color) * (p.in_dir * node->normal) << std::endl;
            //std::cerr << node->M_flux << std::endl;
            //std::cerr << "Color: " << node->color << std::endl;
            //std::cerr << "Position: " << node->position << std::endl;
            //std::cerr << "ray position: " << p.point << std::endl;
        }
        /*
        if(p.point.y == 49.99)
        {
            printf("p.color: %.5lf %.5lf %.5lf\n", p.color.x, p.color.y, p.color.z);
            printf("node->color: %.5lf %.5lf %.5lf\n", node->color.x, node->color.y, node->color.z);
            printf("p.in_dir: %.5lf %.5lf %.5lf\n", p.in_dir.x, p.in_dir.y, p.in_dir.z);
            printf("node->normal: %.5lf %.5lf %.5lf\n", node->normal.x, node->normal.y, node->normal.z);
        }
        */
    }
    
    if(node->dimention == 0) //x
    {
        if(p.point.x < node->position.x)
        {
            query(node->left, p);
            if(node->position.x - p.point.x < node->radius) query(node->right, p);
        }
        else
        {
            query(node->right, p);
            if(p.point.x - node->position.x < node->radius) query(node->left, p);
        }
    }
    if(node->dimention == 1) //y
    {
        if(p.point.y < node->position.y)
        {
            query(node->left, p);
            if(node->position.y - p.point.y < node->radius) query(node->right, p);
        }
        else
        {
            query(node->right, p);
            if(p.point.y - node->position.y < node->radius) query(node->left, p);
        }
    }
    if(node->dimention == 2) //z
    {
        if(p.point.z < node->position.z)
        {
            query(node->left, p);
            if(node->position.z - p.point.z < node->radius) query(node->right, p);
        }
        else
        {
            query(node->right, p);
            if(p.point.z - node->position.z < node->radius) query(node->left, p);
        }
    }
}

const int MAX_DEPTH = 10;
std::vector<hitpoint*> get_hit_points(Ray ray, int depth, int w, int h, double weight, vec3 _color, std::vector<object*> scene)
{
    std::vector<hitpoint*> points;
    if(depth > MAX_DEPTH) return points; //need to be changed to Russian Rollute
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
        object* obj = scene[intersect_id];
        switch (obj->type)
        {
        case SPEC:
        {
            Ray new_ray = ray.reflect(ray, intersection, normal);
            std::vector<hitpoint*> new_points = get_hit_points(new_ray, depth + 1, w, h, weight, _color ^ color, scene);
            points.insert(points.end(), new_points.begin(), new_points.end());
            break;
        }
        case REFR:
        {
            double R = ray.fersnel(ray, intersection, normal, obj->ratio);
            Ray new_refl_ray = ray.reflect(ray, intersection, normal);
            if(1.0 - R < eps)
            {
                std::vector<hitpoint*> new_points = get_hit_points(new_refl_ray, depth + 1, w, h, weight, _color ^ color, scene);
                points.insert(points.end(), new_points.begin(), new_points.end());
            }
            else
            {
                Ray new_refr_ray = ray.refract(ray, intersection, normal, obj->ratio);
                std::vector<hitpoint*> new_points = get_hit_points(new_refr_ray, depth + 1, w, h, weight * (1 - R), _color ^ color, scene);
                if(points.size() == 0)
                {
                    new_points = get_hit_points(new_refl_ray, depth + 1, w, h, weight, _color ^ color, scene);
                }
                else
                {
                    points.insert(points.end(), new_points.begin(), new_points.end());
                    new_points = get_hit_points(new_refl_ray, depth + 1, w, h, weight * R, _color ^ color, scene);
                }
                points.insert(points.end(), new_points.begin(), new_points.end());
            }
            break;
        }
        case DIFF:
        {
            hitpoint* new_point = new hitpoint(intersection, normal, _color ^ color, w, h, weight);
            points.push_back(new_point);
            break;
        }
        
        default:
            break;
        }
    }
    if(points.size() == 0)
    {
        std::cerr << "Not reaching: light origin: " << ray.origin << ", light direction: " << ray.direction << ", depth: " << depth << ", normal: " << normal << std::endl;
    }
    //assert(points.size() != 0);
    return points;
}

void trace_photons(Ray ray, int depth, vec3 _color, hitpoint* root, std::vector<object*> scene)
{
    if(depth > MAX_DEPTH) return; //need to be changed to Russian Rollute
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
        object* obj = scene[intersect_id];
        switch (obj->type)
        {
        case SPEC:
        {
            Ray new_ray = ray.reflect(ray, intersection, normal);
            trace_photons(new_ray, depth + 1, _color ^ color, root, scene);
            break;
        }
        case REFR:
        {
            double R = ray.fersnel(ray, intersection, normal, obj->ratio);
            Ray new_refl_ray = ray.reflect(ray, intersection, normal);
            if(1.0 - R < eps) trace_photons(new_refl_ray, depth + 1, _color ^ color, root, scene);
            else
            {
                Ray new_refr_ray = ray.refract(ray, intersection, normal, obj->ratio);
                trace_photons(new_refl_ray, depth + 1, R * _color ^ color, root, scene);
                trace_photons(new_refr_ray, depth + 1, (1 - R) * _color ^ color, root, scene);
            }
            break;
        }
        case DIFF:
        {
            //printf("intersection: %.5lf %.5lf %.5lf\n", intersection.x, intersection.y, intersection.z);
            photon p(intersection, ray.direction, _color);
            query(root, p);
            Ray new_ray = ray.diffuse(ray, intersection, normal);
            trace_photons(new_ray, depth + 1, _color ^ color, root, scene);
        }
        default:
            break;
        }
    }
}
/*
void correct(hpkditer Begin, hpkditer End, vec3* picture, int width)
{
    for(hpkditer iter = Begin; iter != End; iter ++)
    {
        if((*iter)->M == 0) continue;
        double d1 = (double)(*iter)->N + (double)(*iter)->M * hitpoint::alpha;
        double d2 = (double)(*iter)->N + (double)(*iter)->M;
        double d = d1 / d2;
        //printf("N: %lld, M: %lld, d1: %.5lf, d2: %.5lf, d: %.5lf\n", (*iter)->N, (*iter)->M, d1, d2, d);
        hitpoint::decrease = std::max(hitpoint::decrease, d);
    }
    for(hpkditer iter = Begin; iter != End; iter ++)
    {
        //(0, 36)
        if((*iter)->h == 3 && (*iter)->w == 47)
        {
            printf("flux: %.5lf %.5lf %.5lf, N: %lld, M: %lld, w: %d, left: %x, right: %x\n", (*iter)->M_flux.x, (*iter)->M_flux.y, (*iter)->M_flux.z, (*iter)->N, (*iter)->M, (*iter)->w, (*iter)->left, (*iter)->right);
            printf("%x\n", *iter);
        }
        int pos = (*iter)->h * width + (*iter)->w;
        picture[pos] = picture[pos] + ((*iter)->weight * (*iter)->emission);
        (*iter)->flux = ((*iter)->flux + (*iter)->M_flux) * hitpoint::decrease;
        (*iter)->N += (*iter)->M * hitpoint::decrease;
        (*iter)->M = 0; (*iter)->M_flux = vec3();
        if((*iter)->N != 0)
        picture[pos] = picture[pos] + ((*iter)->weight * ((*iter)->flux / (*iter)->N));
    }
    hitpoint::radius *= hitpoint::decrease;
    hitpoint::decrease = 0;
}
*/
void update_decrease(hitpoint* node)
{
    if(node == nullptr) return;
    double d1 = (double)node->N + (double)node->M * hitpoint::alpha;
    double d2 = (double)node->N + (double)node->M;
    if(d2 != 0 && d2 != d1) hitpoint::decrease = std::max(hitpoint::decrease, d1 / d2);
    update_decrease(node->left);
    update_decrease(node->right);
}
void update_picture(hitpoint* node, vec3* picture, int width)
{
    if(node == nullptr) return;
    int pos = node->h * width + node->w;
    node->flux = (node->flux + node->M_flux) * hitpoint::decrease;
    node->N += node->M * hitpoint::alpha;
    node->M = 0; node->M_flux = vec3();
    //if(node->position.y == 49.99 && node->weight < 1.0 && node->weight > 0.1) printf("%.5lf\n", node->weight);
    if(node->N != 0)
    //picture[pos] = picture[pos] + (node->weight * (node->flux / node->N));
    picture[pos] = picture[pos] + (1 / (PI * hitpoint::radius * hitpoint::radius)) * node->weight * (node->flux / node->N);
    update_picture(node->left, picture, width);
    update_picture(node->right, picture, width);
}
void correct(hitpoint* root, vec3* picture, int width)
{
    hitpoint::decrease = 0.0;
    std::cerr << "start update decrease\n";
    update_decrease(root);
    std::cerr << "decrese: " << hitpoint::decrease << std::endl;
    hitpoint::radius *= sqrt(hitpoint::decrease);
    update_picture(root, picture, width);
}

void ppm(Camera cam, const int MAX_PHOTONS, std::vector<object*> scene)
{
    std::vector<hitpoint*> points;
    int scene_size = scene.size();
    int width = cam.x_pixel;
    int height = cam.y_pixel;
    int size = width * height;
    for(int h = 0; h < height; h ++)
    {
        printf("\rtracing hit points %.2lf%%", (double)(h + 1) * 100.0 / (double)height);
        for(int w = 0; w < width; w ++)
        {
            Ray ray = cam.get_ordinary_ray(w, h);
            std::vector<hitpoint*> new_points = get_hit_points(ray, 1, w, h, 1.0, vec3(1.0, 1.0, 1.0), scene);
            if(new_points.size() == 0) printf("Error!: w: %d, h: %d\n", w, h);
            points.insert(points.end(), new_points.begin(), new_points.end());
        }
    }
    printf("\nsize of points: %ld\nBuilding kdtree\n", points.size());
    //for(int i = 0; i < points.size(); i ++)
    {
        //if(points[i]->position.x > -20 && points[i]->position.x < 20 && points[i]->position.y > 5.0 && points[i]->position.y < 15.0 && points[i]->position.z > -20 && points[i]->position.z < 20)
        //std::cerr << points[i] << std::endl;
    }
    hitpoint* root = build_tree(points.begin(), points.end(), 0);

    for(int iteration = 1; iteration <= 10000; iteration ++)
    {
        printf("Iteration %d\n", iteration);
        for(int id = 0; id < scene_size; id ++)
        {
            if(scene[id]->emission == vec3()) continue;
            int n = 0;
            for(int i = 0; i < MAX_PHOTONS; i ++)
            {
                n ++;
                if(n % 10 == 0) printf("\rEmitting photons %5.2lf%%", (double)n * 100.0 / (double)MAX_PHOTONS);
                Ray emit = scene[id]->random_emit();
                //printf("%d\n", hits);
                photon p(emit.origin, scene[id]->co.inv_trans_vector(scene[id]->normal(vec3())) * (-1), scene[id]->emission * 10);
                query(root, p);
                trace_photons(emit, 1, scene[id]->emission, root, scene);
            }
        }
        
        vec3* picture = new vec3[size];
        std::cerr << "start drawing\n";
        //correct(points.begin(), points.end(), picture, width);
        correct(root, picture, width);
        printf("\nradius: %lf\n", hitpoint::radius);
        printf("Writing Image\n");
        FILE* image = fopen("iteration.ppm", "w");
        fprintf(image, "P3\n%d %d\n%d\n", width, height, 255);
        for(int i = 0; i < size; i ++)
        fprintf(image, "%d %d %d ", gamma_trans(picture[i].x), gamma_trans(picture[i].y), gamma_trans(picture[i].z));
        fflush(image);
        printf("Finished\n");
        delete[] picture;
    }
}