#pragma once
// parse scene

//how to write a scene file
/*Scene
 *{
 *  Light
 *  {
 *      [
 *          Shape=/
 *          {Shape parameters: Only sphere and round and plane}
 *          Color=/
 *          {vec3}
 *          Coordinate=/
 *          {Coordinate parameters}
 *          Illumination=/
 *          {Illumination parameter: Vec}
 *          Ratio=/
 *          {float}
 *          Material=/
 *          {Material: DIFF, SPEC, REFR}
 *      ]
 *  }
 *  End Light
 *  Object
 *  {
 *      [
 *          same as Light without Illumination
 *      ]
 *  }
 *  End Object
 *  Camera
 *  {
 *      Coordinate=/
 *      x_angle=/
 *      y_angle=/
 *      x_pixel=/
 *      y_pixel=/
 *  }
 *  Algorithm
 *  {
 *      MonteCarlo/PPM
 *  }
 *  End Algorithm
 * }
*/

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include "vec3.hpp"
#include "objects/object.hpp"
#include "utils.hpp"
#include "objects/Plane.hpp"
#include "objects/Circle.hpp"
#include "objects/Sphere.hpp"
#include "objects/Square.hpp"
#include "objects/Cylinder.hpp"
#include "objects/r_Bezier.hpp"
#include "objects/Triangle.hpp"
#include "objects/Mesh.hpp"
#include "objects/Box.hpp"
#include "Camera.hpp"
#include "Loadobj.hpp"

class Scene
{
    public:
    std::vector<object*> Objects;
    int obj_size;
};

class Parser
{
    private:
    std::string file_content;
    Scene* light;
    Scene* objs;
    Camera* cam;
    Algo Algorithm;
    void Parse();
    std::string Str_From_File(std::string file_path);
    void Split(const std::string& s, std::vector<std::string>& sv, const char flag = ' ');
    std::string Erase_Blank(std::string s);
    vec3 Parse_Vec3(std::string vec);
    Coordinate Parse_Coordinate(std::string s);
    std::vector<vec3> Parse_Controll_Point(std::string Path);
    void Parse_Light(std::string s);
    void Parse_Object(std::string s);
    void Parse_Camera(std::string s);
    void Parse_Effect(std::string s);
    void Parse_Algorithm(std::string s);
    public:

    Parser(std::string path)
    {
        try
        {
            file_content = Str_From_File(path);
            file_content = Erase_Blank(file_content);
            light = new Scene;
            objs = new Scene;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            std::cerr << "Error Occured in Parser\n";
        }
        Parse();
    }

    Scene* Get_Light() {return light;}
    Scene* Get_Objects() {return objs;}
    Camera* Get_Camera() {return cam;}
    Algo Get_Algorithm() {return Algorithm;}
};

typedef std::smatch::iterator sm_iter;

void Parser::Parse()
{
    std::regex light_regex("Light(.+)EndLight");
    std::regex object_regex("Object(.+?)EndObject");
    std::regex camera_regex("Camera(.+?)EndCamera");
    std::regex effect_regex("Effect(.+?)EndEffect");
    std::regex algorithm_regex("Algorithm\\s*\\{\\s*([A-Za-z].+)\\s*\\}\\s*EndAlgorithm");
    std::smatch sm;
    if(std::regex_search(file_content, sm, light_regex)) //parse light
    {
        std::string light_content = sm[1];
        Parse_Light(light_content);
    }
    else
    {
        std::cerr << "Cannot Find Light\n";
    }
    if(std::regex_search(file_content, sm, object_regex))
    {
        std::string object_content = sm[1];
        Parse_Object(object_content);
    }
    else
    {
        std::cerr << "Cannot Find Object\n";
    }
    if(std::regex_search(file_content, sm, camera_regex))
    {
        std::string camera_content = sm[1];
        Parse_Camera(camera_content);
    }
    else
    {
        std::cerr << "Cannot Find Camera\n";
    }
    if(std::regex_search(file_content, sm, effect_regex))
    {
        std::string effect_content = sm[1];
        Parse_Effect(effect_content);
    }
    if(std::regex_search(file_content, sm, algorithm_regex))
    {
        std::string algorithm_content = sm[1];
        Parse_Algorithm(algorithm_content);
    }
    else
    {
        std::cerr << "Cannot Find Algorithm; Default PPM\n";
        Algorithm = PPM;
    }
}

std::string Parser::Str_From_File(std::string file_path)
{
    std::ifstream stream;
    stream.open(file_path, std::ios::in);
    if(!stream.good())
    {
        std::cerr << "Error Occured When Open File '" << file_path << "': No Such File\n";
        return "";
    }
    std::ostringstream buf;
    char c;
    while(buf && stream.get(c)) buf.put(c);
    stream.close();
    return buf.str();
}

void Parser::Split(const std::string& s, std::vector<std::string>& sv, const char flag)
{
    sv.clear();
    std::istringstream ss(s);
    std::string temp;

    while(getline(ss, temp, flag)) sv.push_back(temp);
    return;
}

std::string Parser::Erase_Blank(std::string s)
{
    std::string::iterator head = s.begin(), tail = s.end();
    while(head != tail)
    {
        if(*head == '\f' || *head == '\v' || *head == '\r' || *head == '\t' || *head == '\n' || *head == ' ')
            s.erase(head, head + 1);
        head ++;
    }
    return s;
}

vec3 Parser::Parse_Vec3(std::string vec)
{
    if(*(vec.begin()) != '(' || *(vec.end() - 1) != ')')
    {
        std::cerr << "Parsing Vector '" << vec << "' Error: Not A Vector\n";
    }
    std::vector<std::string> vecs;
    Split(std::string(vec.begin() + 1, vec.end() - 1), vecs, ',');
    return vec3(atof(vecs[0].c_str()), atof(vecs[1].c_str()), atof(vecs[2].c_str()));
}

Coordinate Parser::Parse_Coordinate(std::string s)
{
    std::regex vec3_regex("(\\(.+?\\))");
    auto head = std::sregex_iterator(s.begin(), s.end(), vec3_regex);
    auto tail = std::sregex_iterator();
    if(int size = std::distance(head, tail))
    {
        std::string center_vec3 = (*head).str();
        if(size == 1) return Coordinate(Parse_Vec3(center_vec3));
        head ++;
        std::string x_axis_vec3 = (*head).str();
        if(size == 2) return Coordinate(Parse_Vec3(x_axis_vec3).normalize(), Parse_Vec3(center_vec3));
        head ++;
        std::string y_axis_vec3 = (*head).str();
        head ++;
        std::string z_axis_vec3 = (*head).str();
        return Coordinate(Parse_Vec3(x_axis_vec3).normalize(), Parse_Vec3(y_axis_vec3).normalize(), Parse_Vec3(z_axis_vec3).normalize(), Parse_Vec3(center_vec3));
    }
    return Coordinate();
}

void Parser::Parse_Light(std::string s)
{
    std::regex light_instance_regex("\\[(.+?)\\]");
    auto head = std::sregex_iterator(s.begin(), s.end(), light_instance_regex);
    auto tail = std::sregex_iterator();
    if(std::distance(head, tail))
    {
        for(std::sregex_iterator re_iter = head; re_iter != tail; re_iter ++)
        {
            std::string light_instance = (*re_iter).str();
            std::regex shape_regex("Shape\\s*?=\\s*?\\{(.+?)\\}");
            std::regex color_regex("Color\\s*?=\\s*?\\{(.+?)\\}");
            std::regex coordinate_regex("Coordinate\\s*?=\\s*?\\{(.+?)\\}");
            std::regex illumination_regex("Illumination\\s*?=\\s*?\\{(.+?)\\}");
            std::regex ratio_regex("Ratio\\s*?=\\s*?\\{(.+?)\\}");
            std::regex material_regex("Material\\s*?=\\s*?\\{(.+?)\\}");
            std::regex texture_regex("Texture\\s*?=\\s*?\\{(.+?)\\}");
            std::smatch component_sm;
            std::vector<std::string> parameters;
            color Color;
            Coordinate co;
            vec3 illumination;
            double ratio;
            Refl_t material;
            std::string texture_content = "";
            if(std::regex_search(light_instance, component_sm, texture_regex))
            {
                texture_content = component_sm[1];
            }
            if(std::regex_search(light_instance, component_sm, color_regex))
            {
                std::string color_content = component_sm[1];
                try
                {
                    Color = Parse_Vec3(color_content);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
            if(std::regex_search(light_instance, component_sm, coordinate_regex))
            {
                std::string coordinate_content = component_sm[1];
                try
                {
                    co = Parse_Coordinate(coordinate_content);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
            if(std::regex_search(light_instance, component_sm, illumination_regex))
            {
                std::string illumination_content = component_sm[1];
                try
                {
                    illumination = Parse_Vec3(illumination_content);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
            if(std::regex_search(light_instance, component_sm, ratio_regex))
            {
                std::string ratio_content = component_sm[1];
                try
                {
                    ratio = atof(ratio_content.c_str());
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
            if(std::regex_search(light_instance, component_sm, material_regex))
            {
                std::string material_content = component_sm[1];
                if(material_content == "DIFF") material = DIFF;
                else if(material_content == "SPEC") material = SPEC;
                else if(material_content == "REFR") material = REFR;
            }
            if(std::regex_search(light_instance, component_sm, shape_regex))
            {
                std::string shape_content = component_sm[1];
                Split(Erase_Blank(shape_content), parameters, ';');
                std::string shape_type = parameters[0];
                if(shape_type == "Sphere")
                {
                    try
                    {
                        std::string radius = parameters[1];
                        std::cerr << parameters[1] << std::endl << parameters[2] << std::endl;
                        Sphere* new_light = new Sphere(Parse_Vec3(parameters[1]), atof(radius.c_str()), Color, illumination, ratio, co, material);
                        light->Objects.push_back(new_light);
                        light->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
                else if(shape_type == "Circle")
                {
                    try
                    {
                        std::string radius = parameters[1];
                        Circle* new_light = new Circle(atof(radius.c_str()), Color, illumination, ratio, co, material);
                        light->Objects.push_back(new_light);
                        light->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
                else if(shape_type == "Box")
                {
                    try
                    {
                        std::string length = parameters[1];
                        std::string width = parameters[2];
                        std::string height = parameters[3];
                        std::string texture[6];
                        if(texture_content != "")
                        {
                            std::vector<std::string> textures;
                            Split(texture_content, textures, ';');
                            if(textures.size() >= 6)
                            {
                                for(int k = 0; k < 6; k ++) texture[k] = textures[k];
                            }
                        }
                        Box* new_light = new Box(vec3(atof(length.c_str()), atof(width.c_str()), atof(height.c_str())), texture, Color, illumination, ratio, co, material);
                        light->Objects.push_back(new_light);
                        light->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                    
                }
            }
        }
    }
    else
    {
        std::cerr << "No Light Objects in Scene File\n";
    }
}

void Parser::Parse_Object(std::string s)
{
    std::regex object_instance_regex("\\[(.+?)\\]");
    auto head = std::sregex_iterator(s.begin(), s.end(), object_instance_regex);
    auto tail = std::sregex_iterator();
    if(std::distance(head, tail))
    {
        for(std::sregex_iterator re_iter = head; re_iter != tail; re_iter ++)
        {
            std::string object_instance = (*re_iter).str();
            std::regex shape_regex("Shape\\s*?=\\s*?\\{(.+?)\\}");
            std::regex color_regex("Color\\s*?=\\s*?\\{(.+?)\\}");
            std::regex coordinate_regex("Coordinate\\s*?=\\s*?\\{(.+?)\\}");
            std::regex ratio_regex("Ratio\\s*?=\\s*?\\{(.+?)\\}");
            std::regex material_regex("Material\\s*?=\\s*?\\{(.+?)\\}");
            std::regex texture_regex("Texture\\s*?=\\s*?\\{(.+?)\\}");
            std::smatch component_sm;
            std::vector<std::string> parameters;
            color Color;
            Coordinate co;
            double ratio;
            Refl_t material;
            std::string texture_content = "";
            if(std::regex_search(object_instance, component_sm, texture_regex))
            {
                texture_content = component_sm[1];
            }
            if(std::regex_search(object_instance, component_sm, color_regex))
            {
                std::string color_content = component_sm[1];
                try
                {
                    Color = Parse_Vec3(color_content);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
            if(std::regex_search(object_instance, component_sm, coordinate_regex))
            {
                std::string coordinate_content = component_sm[1];
                try
                {
                    co = Parse_Coordinate(coordinate_content);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
            if(std::regex_search(object_instance, component_sm, ratio_regex))
            {
                std::string ratio_content = component_sm[1];
                try
                {
                    ratio = atof(ratio_content.c_str());
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
            if(std::regex_search(object_instance, component_sm, material_regex))
            {
                std::string material_content = component_sm[1];
                if(material_content == "DIFF") material = DIFF;
                else if(material_content == "SPEC") material = SPEC;
                else if(material_content == "REFR") material = REFR;
            }
            if(std::regex_search(object_instance, component_sm, shape_regex))
            {
                std::string shape_content = component_sm[1];
                Split(Erase_Blank(shape_content), parameters, ';');
                std::string shape_type = parameters[0];
                if(shape_type == "Sphere")
                {
                    try
                    {
                        std::string radius = parameters[2];
                        Sphere* new_object = new Sphere(Parse_Vec3(parameters[1]), atof(radius.c_str()), Color, vec3(), ratio, co, material);
                        if(texture_content != "") new_object->texture.load_texture(texture_content.c_str());
                        objs->Objects.push_back(new_object);
                        objs->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
                else if(shape_type == "Triangle")
                {
                    try
                    {
                        std::string point1 = parameters[1];
                        std::string point2 = parameters[2];
                        std::string point3 = parameters[3];
                        vec3 points[3] = {Parse_Vec3(point1), Parse_Vec3(point2), Parse_Vec3(point3)};
                        Triangle* new_object = new Triangle(points, Color, vec3(), ratio, material);
                        if(texture_content != "") new_object->texture.load_texture(texture_content.c_str());
                        objs->Objects.push_back(new_object);
                        objs->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                    
                }
                else if(shape_type == "Square")
                {
                    try
                    {
                        std::string width = parameters[1];
                        std::string height = parameters[2];
                        Square* new_object = new Square(atof(width.c_str()), atof(height.c_str()), Color, vec3(), ratio, co, material);
                        if(texture_content != "") new_object->texture.load_texture(texture_content.c_str());
                        objs->Objects.push_back(new_object);
                        objs->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                    
                }
                else if(shape_type == "Plane")
                {
                    try
                    {
                        Plane* new_object = new Plane(Parse_Vec3(parameters[1]), Parse_Vec3(parameters[2]), Color, vec3(), ratio, co, material);
                        objs->Objects.push_back(new_object);
                        objs->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
                else if(shape_type == "Mesh")
                {
                    try
                    {
                        std::string mesh_path = parameters[1];
                        std::string scale = parameters[2];
                        Mesh* new_object = new Mesh(atof(scale.c_str()), Color, vec3(), ratio, co, material);
                        if(texture_content != "") new_object->texture.load_texture(texture_content.c_str());
                        Loadobj(new_object, mesh_path);
                        objs->Objects.push_back(new_object);
                        objs->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
                else if(shape_type == "Rotate_Bezier")
                {
                    try
                    {
                        std::string controll_point_path = parameters[1];
                        r_Bezier* new_object = new r_Bezier(Parse_Controll_Point(controll_point_path), Color, vec3(), ratio, co, material);
                        if(texture_content != "") new_object->texture.load_texture(texture_content.c_str());
                        objs->Objects.push_back(new_object);
                        objs->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
                else if(shape_type == "Box")
                {
                    try
                    {
                        std::string length = parameters[1];
                        std::string width = parameters[2];
                        std::string height = parameters[3];
                        std::string texture[6];
                        if(texture_content != "")
                        {
                            std::vector<std::string> textures;
                            Split(texture_content, textures, ';');
                            if(textures.size() >= 6)
                            {
                                for(int k = 0; k < 6; k ++) texture[k] = textures[k];
                            }
                        }
                        Box* new_object = new Box(vec3(atof(length.c_str()), atof(width.c_str()), atof(height.c_str())), texture, Color, vec3(), ratio, co, material);
                        objs->Objects.push_back(new_object);
                        objs->obj_size ++;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                    
                }
                else
                {
                    std::cerr << "Unsupported Object Type in Object" << shape_type << "\n";
                }
            }
        }
    }
    else
    {
        std::cerr << "No Objects in Scene File\n";
    }
    std::cerr << "End Parsing Object" << std::endl;
}

void Parser::Parse_Camera(std::string camera_content)
{
    std::regex coordinate_regex("Coordinate\\s*?=\\s*?\\{(.+?)\\}");
    std::regex x_angle_regex("x_angle\\s*?=\\s*?\\{(.+?)\\}");
    std::regex y_angle_regex("y_angle\\s*?=\\s*?\\{(.+?)\\}");
    std::regex x_pixel_regex("x_pixel\\s*?=\\s*?\\{(.+?)\\}");
    std::regex y_pixel_regex("y_pixel\\s*?=\\s*?\\{(.+?)\\}");
    std::regex radius_regex("apreture\\s*?=\\s*?\\{(.+?)\\}");
    std::regex f_regex("f\\s*?=\\s*?\\{(.+?)\\}");
    std::smatch sm;
    Coordinate co;
    double x_angle, y_angle, radius, f;
    int x_pixel, y_pixel;
    if(std::regex_search(camera_content, sm, coordinate_regex))
    {
        co = Parse_Coordinate(sm[1]);
    }
    else
    {
        std::cerr << "Cannot Find Camera Coordinate\n";
    }
    if(std::regex_search(camera_content, sm, x_angle_regex))
    {
        std::string x_angle_content = sm[1];
        x_angle = atof(x_angle_content.c_str());
    }
    else
    {
        std::cerr << "Cannot Find Camera X Angle\n";
    }
    if(std::regex_search(camera_content, sm, y_angle_regex))
    {
        std::string y_angle_content = sm[1];
        y_angle = atof(y_angle_content.c_str());
    }
    else
    {
        std::cerr << "Cannot Find Camera Y Angle\n";
    }
    if(std::regex_search(camera_content, sm, x_pixel_regex))
    {
        std::string x_pixel_content = sm[1];
        x_pixel = atoi(x_pixel_content.c_str());
    }
    else
    {
        std::cerr << "Cannot Find Camera X Pixel\n";
    }
    if(std::regex_search(camera_content, sm, y_pixel_regex))
    {
        std::string y_pixel_content = sm[1];
        y_pixel = atoi(y_pixel_content.c_str());
    }
    else
    {
        std::cerr << "Cannot Find Camera Y Pixel\n";
    }
    if(std::regex_search(camera_content, sm, radius_regex))
    {
        std::string radius_content = sm[1];
        radius = atof(radius_content.c_str());
    }
    else
    {
        std::cerr << "Cannot Find Apreture\n";
    }
    if(std::regex_search(camera_content, sm, f_regex))
    {
        std::string f_content = sm[1];
        f = atof(f_content.c_str());
    }
    else
    {
        std::cerr << "Cannot Find Focal Length\n";
        f = 0.5;
    }
    cam = new Camera(co, x_angle, y_angle, x_pixel, y_pixel, radius, f);
}

void Parser::Parse_Algorithm(std::string s)
{
    if(s == "PPM") Algorithm = PPM;
    else if(s == "MonteCarlo") Algorithm = MonteCarlo;
    else
    {
        std::cerr << "Unrecognized Algorithm '" << s << "'; Use PPM\n";
        Algorithm = PPM;
    }
}

void Parser::Parse_Effect(std::string s)
{
    return;
}

std::vector<vec3> Parser::Parse_Controll_Point(std::string Path) //s is bezier file path
{
    std::vector<vec3> controll_points;
    std::string points_str = Str_From_File(Path);
    if(points_str.empty()) return controll_points;
    points_str = Erase_Blank(points_str);
    if(points_str[0] != '{' || points_str[points_str.size() - 1] != '}')
    {
        std::cerr << "Bezier file content should start with '{' and end with '}'\n";
        return controll_points;
    }
    std::regex point_regex("\\(([0-9\\.,]+?)\\)");
    auto head = std::sregex_iterator(points_str.begin(), points_str.end(), point_regex);
    auto tail = std::sregex_iterator();
    if(std::distance(head, tail))
    {
        for(auto iter = head; iter != tail; iter ++)
        {
            std::vector<std::string> position;
            std::string point = (*iter).str();
            Split(std::string(point.begin() + 1, point.end() - 1), position, ',');
            controll_points.push_back(vec3(atof(position[0].c_str()), atof(position[1].c_str())));
        }
    }
    return controll_points;
}