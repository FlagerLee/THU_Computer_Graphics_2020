#pragma once

//load mesh from .obj file or .ply file
#include "objects/Mesh.hpp"
#include <ctype.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <iostream>

void Loadobj(Mesh* M, std::string Path)
{
    //does not support vt now
    std::fstream obj_stream;
    obj_stream.open(Path, std::ios::in);
    if(!obj_stream.good())
    {
        std::cerr << "Error Occured When Open File '" << Path << "': No Such File\n";
        obj_stream.close();
        return;
    }
    std::string line;
    std::vector<vec3> v;
    std::vector<vec3> vn;
    std::vector<vec3> vt;
    while(!obj_stream.eof())
    {
        getline(obj_stream, line);
        if(line.empty()) continue;
        std::stringstream linestream(line);
        std::string first; linestream >> first;
        if(first[0] == '#') continue;
        if(first == "v") //vertex node
        {
            vec3 vertex;
            linestream >> vertex.x >> vertex.y >> vertex.z;
            v.push_back(vertex);
        }
        else if(first == "vn") //vertex normal
        {
            vec3 vertex_n;
            linestream >> vertex_n.x >> vertex_n.y >> vertex_n.z;
            vn.push_back(vertex_n);
        }
        else if(first == "vt") //texture coordinate
        {
            vec3 vertex_t;
            linestream >> vertex_t.x >> vertex_t.y >> vertex_t.z;
            vt.push_back(vertex_t);
        }
        else if(first == "f") //face; need to consider four or more points in a face
        {
            std::string X, Y, Z;
            linestream >> X >> Y >> Z;
            vec3 vertex[3], vertex_n[3];
            int ID = 0;
            int count = 0;
            for(auto iter = X.begin(); iter != X.end(); iter ++)
            {
                if(isdigit(*iter)) ID = ID * 10 + ((*iter) - '0');
                else if((*iter) == '/')
                {
                    if(count == 0)
                    {
                        vertex[0] = M->co.inv_trans_point(v[ID - 1] * M->scale);
                    }
                    ID = 0;
                    count ++;
                }
            }
            vertex_n[0] = M->co.inv_trans_vector(vn[ID - 1].normalize());
            ID = 0;
            count = 0;
            for(auto iter = Y.begin(); iter != Y.end(); iter ++)
            {
                if(isdigit(*iter)) ID = ID * 10 + ((*iter) - '0');
                else if((*iter) == '/')
                {
                    if(count == 0)
                    {
                        vertex[1] = M->co.inv_trans_point(v[ID - 1] * M->scale);
                    }
                    count ++;
                    ID = 0;
                }
            }
            vertex_n[1] = M->co.inv_trans_vector(vn[ID - 1].normalize());
            count = 0;
            ID = 0;
            for(auto iter = Z.begin(); iter != Z.end(); iter ++)
            {
                if(isdigit(*iter)) ID = ID * 10 + ((*iter) - '0');
                else if((*iter) == '/')
                {
                    if(count == 0)
                    {
                        vertex[2] = M->co.inv_trans_point(v[ID - 1] * M->scale);
                    }
                    count ++;
                    ID = 0;
                }
            }
            vertex_n[2] = M->co.inv_trans_vector(vn[ID - 1].normalize());
            assert(vertex_n[0] != vec3() || vertex_n[1] != vec3() || vertex_n[2] != vec3());
            Triangle* Tri;
            if(isnan(vertex_n[0].x) || isnan(vertex_n[1].x) || isnan(vertex_n[2].x))
            Tri = new Triangle(vertex, M->color, M->emission, M->ratio, M->type);
            else Tri = new Triangle(vertex, vertex_n, M->color, M->emission, M->ratio, M->type);
            M->T.push_back(Tri);
        }
    }
    M->tree.build(M->T);
    obj_stream.close();
}