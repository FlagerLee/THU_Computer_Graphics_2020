#pragma once

//load mesh from .obj file or .ply file
#include "objects/Mesh.hpp"
#include <ctype.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

void Loadobj(Mesh* M, std::fstream& obj_stream)
{
    //does not support vt now
    std::string line;
    std::vector<vec3> v;
    std::vector<vec3> vn;
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
        else if(first == "f") //face; need to consider four or more points in a face
        {
            std::string X, Y, Z;
            linestream >> X >> Y >> Z;
            vec3 vertex[3], vertex_n[3];
            int ID = 0;
            for(auto iter = X.begin(); iter != X.end(); iter ++)
            {
                if(isdigit(*iter)) ID = ID * 10 + ((*iter) - '0');
                else if((*iter) == '/' && ID)
                {
                    vertex[0] = M->co.inv_trans_point(v[ID - 1] * M->times);
                    ID = 0;
                }
            }
            vertex_n[0] = M->co.inv_trans_vector(vn[ID - 1]).normalize();
            ID = 0;
            for(auto iter = Y.begin(); iter != Y.end(); iter ++)
            {
                if(isdigit(*iter)) ID = ID * 10 + ((*iter) - '0');
                else if((*iter) == '/' && ID)
                {
                    vertex[1] = M->co.inv_trans_point(v[ID - 1] * M->times);
                    ID = 0;
                }
            }
            vertex_n[1] = M->co.inv_trans_vector(vn[ID - 1]).normalize();
            ID = 0;
            for(auto iter = Z.begin(); iter != Z.end(); iter ++)
            {
                if(isdigit(*iter)) ID = ID * 10 + ((*iter) - '0');
                else if((*iter) == '/' && ID)
                {
                    vertex[2] = M->co.inv_trans_point(v[ID - 1] * M->times);
                    ID = 0;
                }
            }
            vertex_n[2] = M->co.inv_trans_vector(vn[ID - 1]).normalize();
            Triangle* Tri = new Triangle(vertex, vertex_n, M->color, M->emission, M->ratio, M->type);
            M->T.push_back(Tri);
        }
    }
    M->tree.build(M->T);
    //close fstream outside this function
}