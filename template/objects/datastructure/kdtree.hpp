#pragma once

#include "../AABB.hpp"
#include "../../ray.hpp"
#include <vector>

class kdnode
{
    public:
    int type; //type == 0 means it is splited by x_axis
              //type == 1 => y_axis; type == 2 => z_axis;
              //type == 3 means this node is a leaf node;
    kdnode* left; kdnode* right; //two child node
                                 //if this is a leaf node, then right cursor is NULL
                                 //and left cursor is an object
    AABB box;
    object* obj; //object in leaf node
    kdnode(int _type, AABB _box, kdnode* _left, kdnode* _right, object* _obj)
    {
        type = _type; box = _box;
        left = _left; right = _right;
        obj = _obj;
    }
    ~kdnode()
    {
        if(left) delete(left);
        if(right) delete(right);
    }
};

bool cmp(AABB a, AABB b, int type)
{
    if(type == 0) return (a.Max.x + a.Min.x) <= (b.Max.x + b.Min.x);
    if(type == 1) return (a.Max.y + a.Min.y) <= (b.Max.y + b.Min.y);
    return (a.Max.z + a.Min.z) <= (b.Max.z + b.Min.z);
}

typedef std::vector<kdnode*>::iterator kditer;

class kdtree
{
    public:
    std::vector<kdnode*> nodes;
    kdnode* root;

    void build(std::vector<Triangle*> obj);
    kdnode* build(kditer Begin, kditer End, int type);
    void find_mid(kditer Begin, kditer End, int type);
    std::vector<object*> query(Ray ray, kdnode* node) const;

    ~kdtree()
    {
        for(kditer iter = nodes.begin(); iter != nodes.end(); iter ++)
        {
            if(*iter != NULL) delete(*iter);
        }
        nodes.clear();
        delete(root);
    }
};

void kdtree::build(std::vector<Triangle*> obj)
{
    for(auto iter = obj.begin(); iter != obj.end(); iter ++)
    {
        kdnode* new_node = new kdnode(3, AABB(*iter), NULL, NULL, (*iter));
        nodes.push_back(new_node);
    }
    root = build(nodes.begin(), nodes.end(), 0);
}

kdnode* kdtree::build(kditer Begin, kditer End, int type)
{
    if(End - Begin == 2)
    {
        vec3 Max(std::max((*Begin)->box.Max.x, (*(Begin + 1))->box.Max.x),
                 std::max((*Begin)->box.Max.y, (*(Begin + 1))->box.Max.y),
                 std::max((*Begin)->box.Max.z, (*(Begin + 1))->box.Max.z));
        vec3 Min(std::min((*Begin)->box.Min.x, (*(Begin + 1))->box.Min.x),
                 std::min((*Begin)->box.Min.y, (*(Begin + 1))->box.Min.y),
                 std::min((*Begin)->box.Min.z, (*(Begin + 1))->box.Min.z));
        AABB new_box(Max, Min);
        kdnode* new_node = new kdnode(type, new_box, (*Begin), (*(Begin + 1)), NULL);
        return new_node;
    }
    else if(End - Begin == 1) return (*Begin);
    else if(End == Begin) return NULL;
    find_mid(Begin, End, type);
    int size = End - Begin;
    kditer mid = Begin + size / 2;
    kdnode* left = build(Begin, mid, (type + 1) % 3);
    kdnode* right = build(mid, End, (type + 1) % 3);
    vec3 Max(std::max(left->box.Max.x, right->box.Max.x),
             std::max(left->box.Max.y, right->box.Max.y),
             std::max(left->box.Max.z, right->box.Max.z));
    vec3 Min(std::min(left->box.Min.x, right->box.Min.x),
             std::min(left->box.Min.y, right->box.Min.y),
             std::min(left->box.Min.z, right->box.Min.z));
    AABB new_box(Max, Min);
    kdnode* new_node = new kdnode(type, new_box, left, right, NULL);
    return new_node;
}

void kdtree::find_mid(kditer Begin, kditer End, int type)
{
    //type = 0: x; type = 1: y; type = 2: z;
    //ensure size >= 2 before using it
    int size = End - Begin;
    if(size <= 2) return;
    kditer mid = Begin + size / 2;
    for(kditer lo = Begin, hi = End - 1; hi - lo > 0;)
    {
        kditer i = lo, j = hi;
        kdnode* pivot = *lo;
        while(j - i > 0)
        {
            while(j - i > 0 && cmp(pivot->box, (*j)->box, type)) j--;
            *i = *j;
            while(j - i > 0 && cmp((*i)->box, pivot->box, type)) i++;
            *j = *i;
        }
        *i = pivot;
        if(i - mid >= 0) hi = i - 1;
        if(mid - i >= 0) lo = i + 1;
    }

    /*
    while(true)
    {
        while(l != r)
        {
            while(l != r && cmp((*pivot)->box, (*r)->box, type)) r --;
            while(l != r && cmp((*l)->box, (*pivot)->box, type)) l ++;
            if(l != r) std::swap((*l), (*r));
        }
        std::swap((*pivot), (*l));
        int size2 = l - Begin;
        printf("size2: %d\n", size2);
        if(size2 == size / 2) break;
        if(size2 < size / 2)
        {
            L = l;
            pivot = R;
            r = R - 1;
        }
        else
        {
            R = r;
            pivot = L;
            l = L + 1;
        }
    }
    */
    return;
}

std::vector<object*> kdtree::query(Ray ray, kdnode* node) const
{
    std::vector<object*> obj;
    vec3 intersect, normal;
    if(node->type == 3)
    {
        if(node->box.intersect(&ray, intersect, normal)) obj.push_back(node->obj);
    }
    else
    {
        if(node->left)
        {
            if(node->left->box.intersect(&ray, intersect, normal))
            {
                std::vector<object*> left_obj = query(ray, node->left);
                if(!left_obj.empty())
                {
                    for(auto iter = left_obj.begin(); iter != left_obj.end(); iter ++)
                    obj.push_back(*iter);
                }
            }
        }
        if(node->right)
        {
            if(node->right->box.intersect(&ray, intersect, normal))
            {
                std::vector<object*> right_obj = query(ray, node->right);
                if(!right_obj.empty())
                {
                    for(auto iter = right_obj.begin(); iter != right_obj.end(); iter ++)
                    obj.push_back(*iter);
                }
            }
        }
    }
    return obj;
}