#pragma once

//rotate bezier
#include "object.hpp"
#include "Mesh.hpp"
#include <algorithm>
#include <vector>

class r_Bezier : public object
{
    private:
    double B(int n, int i, double t) const
    {
        return C[n][i] * pow(t, i) * pow(1 - t, n - i);
    }

    public:
    vec3 controll[9]; //9 controll point at most
    vec3 d_point[8]; //derivative point
    Mesh* M; //write rotate Bezier curve as a Mesh
    std::vector<double> sampling; //sampling point
    int size; //the number of the controll point

    r_Bezier(const vec3 _controll[], int _size, vec3 _color, vec3 _emission, double _ratio, Coordinate _co, Refl_t _type)
    {
        size = _size;
        for(int i = 0; i < size; i ++) controll[i] = _controll[i];
        for(int i = 0; i < size - 1; i ++) d_point[i] = (size - 1) * (controll[i + 1] - controll[i]);
        color = _color;
        emission = _emission;
        ratio = _ratio;
        co = _co;
        type = _type;
        init_Mesh(0, 1);
        sampling.push_back(1.0);
        double delta_theta = 2 * PI / 72;
        M = new Mesh(1.0, color, emission, ratio, co, type);
        for(int i = 0; i < 72; i ++)
        {
            for(auto iter = sampling.begin(); iter != sampling.end() - 1; iter ++)
            {
                vec3 point[3];
                vec3 p1 = get_position(vec3(*iter, i * delta_theta, 0)), p2 = get_position(vec3(*(iter + 1), i * delta_theta, 0));
                vec3 p3 = get_position(vec3(*iter, (i + 1) * delta_theta, 0)), p4 = get_position(vec3(*(iter + 1), (i + 1) * delta_theta, 0));
                point[0] = co.inv_trans_point(p1);
                point[1] = co.inv_trans_point(p2);
                point[2] = co.inv_trans_point(p3);
                Triangle* tri = new Triangle(point, color, emission, ratio, type);
                M->T.push_back(tri);
                point[0] = co.inv_trans_point(p2);
                point[1] = co.inv_trans_point(p3);
                point[2] = co.inv_trans_point(p4);
                Triangle* tri2 = new Triangle(point, color, emission, ratio, type);
                M->T.push_back(tri2);
            }
        }
        M->tree.build(M->T);
    }
    void init_Mesh(double start, double end) //t belongs to [start, end]
    {
        //cut the curve into species using self-adapted method
        vec3 t_start = d_curve(vec3(start, 0, 0)).normalize(), t_end = d_curve(vec3(end, 0, 0)).normalize();
        if(fabs(t_start.x - t_end.x) < 0.001)
        {
            double t1 = frand() * (end - start) + start;
            if(fabs(t_start.x - (d_curve(vec3(t1, 0, 0)).normalize()).x) > 0.001)
            {
                init_Mesh(start, t1);
                init_Mesh(t1, end);
            }
            else
            {
                double t2 = frand() * (end - start) + start;
                if(fabs(t_start.x - (d_curve(vec3(t2, 0, 0)).normalize()).x) > 0.001)
                {
                    init_Mesh(start, t2);
                    init_Mesh(t2, end);
                }
                else
                {
                    sampling.push_back(start);
                }
            }
        }
        else
        {
            double t1 = (start + end) / 2;
            init_Mesh(start, t1);
            init_Mesh(t1, end);
        }
        return;
    }

    vec3 get_position(vec3 v) //v: (t, theta, 0)
    {
        if(v.x < 0.0) v.x = 0.0;
        else if(v.x > 1.0) v.x = 1.0;
        vec3 ans;
        for(int i = 0; i < size; i ++) ans = ans + controll[i] * B(size - 1, i, v.x);
        return vec3(ans.x * cos(v.y), ans.x * sin(v.y), ans.y);
    }
    vec3 d_curve(vec3 v) //v: (d, theta, 0)
    {
        if(v.x < 0.0) v.x = 0.0;
        else if(v.x > 1.0) v.x = 1.0;
        vec3 ans;
        for(int i = 0; i < size - 1; i ++) ans = ans + d_point[i] * B(size - 2, i, v.x);
        return vec3(ans.x * cos(v.y), ans.x * sin(v.y), ans.y);
    }
    vec3 d_rotate(vec3 v) //v: (d, theta, 0)
    {
        vec3 ans = get_position(v);
        return vec3(-ans.y, ans.x, 0);
    }

    virtual vec3 normal(vec3 v)
    {
        return (this->d_curve(v) % this->d_rotate(v)).normalize();
    }
    double find_t(vec3 pos, double start, double end, double theta)
    {
        //printf("begin: %.5lf end: %.5lf\n", start, end);
        if(end - start < 0.01) return -1;
        double mid = (start + end) / 2;
        vec3 new_point = get_position(vec3(mid, theta, 0));
        //printf("new_point: %.5lf %.5lf %.5lf\npos: %.5lf %.5lf %.5lf\n", new_point.x, new_point.y, new_point.z, pos.x, pos.y, pos.z);
        if((new_point - pos).length() < 1) return mid;
        double ans = find_t(pos, start, mid, theta);
        if(ans > -eps) return ans;
        ans = find_t(pos, mid, end, theta);
        if(ans > -eps) return ans;
        return -1;
    }
    virtual bool intersect(const Ray* ray, vec3& intersection, vec3& normal, vec3& color)
    {
        if(!M->intersect(ray, intersection, normal, color)) return false;
        vec3 co_intersection = co.trans_point(intersection);
        double l = (intersection - ray->origin).length();
        double theta = acos(co_intersection.x / vec3(co_intersection.x, co_intersection.y, 0).length());
        if(co_intersection.y < 0) theta = 2 * PI - theta;
        double t = find_t(co_intersection, 0.0, 1.0, theta);
        Ray new_ray(co.trans_point(ray->origin), co.trans_vector(ray->direction));
        vec3 start_point = get_position(vec3(t, theta, 0));
        const int MAX_DEPTH = 15;
        vec3 X(l, t, theta);
        for(int i = 0; i < MAX_DEPTH; i ++)
        {
            vec3 dt = d_curve(vec3(t, theta, 0));
            vec3 dtheta = d_rotate(vec3(t, theta, 0));
            vec3 point = get_position(vec3(t, theta, 0));
            vec3 f = new_ray.origin + l * new_ray.direction - point;
            if(f.length() < 0.0001)
            {
                if(l < eps || t < 0.0 || t > 1.0) return false;
                intersection = ray->origin + ray->direction * l;
                normal = co.inv_trans_vector(dt % dtheta);
                if(texture.loaded()) color = texture.get_color(theta / (2 * PI), t); 
                else color = this->color;
                return true;
            }
            double D = new_ray.direction * (dt % dtheta);
            double D0 = dtheta * (dt % new_ray.direction);
            l = l - (dtheta * (dt % f) / D0);
            t = t - (new_ray.direction * (dtheta % f) / D);
            theta = theta + (new_ray.direction * (dt % f) / D);
            if(theta < 0.0) theta = PI - theta;
            if(theta > 2 * PI) theta = fmod(theta, 2 * PI);
        }
        return false;
    }
};