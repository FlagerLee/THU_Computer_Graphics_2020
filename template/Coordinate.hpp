#pragma once
#pragma clang diagnostic ignored"-Wunused-value"

//define new coordinate system and provide transform between the old and the new

#include "vec3.hpp"

class Matrix3x3
{
    //used to solve three-variable liner equation
    public:
    vec3 l1, l2, l3; //each 3-dimention vector represents a line vector [x, y, z]^T,
                     //and the matrix equals to [l1, l2, l3]

    Matrix3x3(vec3 _l1, vec3 _l2, vec3 _l3): l1(_l1), l2(_l2), l3(_l3) {}
    double solve() //get its determinant's value
    {
        double t1 = l1.x * l2.y * l3.z;
        double t2 = l1.y * l2.z * l3.x;
        double t3 = l1.z * l2.x * l3.y;
        double t4 = l1.z * l2.y * l3.x;
        double t5 = l1.y * l2.x * l3.z;
        double t6 = l1.x * l2.z * l3.y;
        return t1 + t2 + t3 - t4 - t5 - t6;
    }
    vec3 solve(vec3 result) //solve function M * [x, y, z]^T = result^T
    {
        double value = this->solve();
        double x = Matrix3x3(result, this->l2, this->l3).solve() / value;
        double y = Matrix3x3(this->l1, result, this->l3).solve() / value;
        double z = Matrix3x3(this->l1, this->l2, result).solve() / value;
        return vec3(x, y, z);
    }

    Matrix3x3 inv()
    {
        double value = solve();
        if(fabs(value - 0.0) < eps) return Matrix3x3(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0));
        vec3 c1 = ((l2.y * l3.z) - (l2.z * l3.y), - l2.x * l3.z + l2.z * l3.x, l2.x * l3.y - l2.y * l3.x);
        vec3 c2 = (- l1.y * l3.z + l1.z * l3.y, l1.x * l3.z - l1.z * l3.x, - l1.x * l3.y + l1.y * l3.x);
        vec3 c3 = (l1.y * l2.z - l1.z * l2.y, - l1.x * l2.z + l1.z * l2.x, l1.x * l2.y - l1.y * l2.x);
        c1 = c1 / value;
        c2 = c2 / value;
        c3 = c3 / value;
        return Matrix3x3(c1, c2, c3);
    }
};

vec3 operator * (const Matrix3x3& M, const vec3& v)
{
    double x = M.l1.x * v.x + M.l2.x * v.y + M.l3.x * v.z;
    double y = M.l1.y * v.x + M.l2.y * v.y + M.l3.y * v.z;
    double z = M.l1.z * v.x + M.l2.z * v.y + M.l3.z * v.z;
    return vec3(x, y, z);
}

class Coordinate
{
    public:
    vec3 x_axis, y_axis, z_axis; //three axis of the coordinate system
    vec3 origin; //the origin point

    Coordinate(vec3 _x_axis, vec3 _y_axis, vec3 _z_axis, vec3 _origin):
                x_axis(_x_axis.normalize()), y_axis(_y_axis.normalize()), z_axis(_z_axis.normalize()), origin(_origin.normalize()) {}
    Coordinate(vec3 _z_axis, vec3 _origin)
    {
        z_axis = _z_axis.normalize();
        x_axis = (z_axis % (fabs(z_axis.x) < 0.1 ? vec3(1, 0, 0) : vec3(0, 1, 0))).normalize();
        y_axis = z_axis % x_axis;
        origin = _origin;
    }
    Coordinate(vec3 _origin)
    {
        origin = _origin;
        z_axis = vec3(0, 0, 1);
        y_axis = vec3(0, 1, 0);
        x_axis = vec3(1, 0, 0);
    }
    Coordinate()
    {
        z_axis = vec3(0, 0, 1);
        y_axis = vec3(0, 1, 0);
        x_axis = vec3(1, 0, 0);
        origin = vec3(0, 0, 0);
    }

    vec3 trans_point(vec3 v) const //transform old point to new point
    {
        Matrix3x3 M(this->x_axis, this->y_axis, this->z_axis);
        return M.solve(v - origin);
    }
    vec3 trans_vector(vec3 v) const //transform old vector to new vector
    {
        Matrix3x3 M(this->x_axis, this->y_axis, this->z_axis);
        return M.solve(v);
    }
    vec3 inv_trans_point(vec3 v) const //transform new point to old point
    {
        return origin + v.x * x_axis + v.y * y_axis + v.z * z_axis;
    }
    vec3 inv_trans_vector(vec3 v) const //transform new vector to old vector
    {
        return v.x * x_axis + v.y * y_axis + v.z * z_axis;
    }

    Coordinate& operator = (const Coordinate co)
    {
        x_axis = co.x_axis;
        y_axis = co.y_axis;
        z_axis = co.z_axis;
        origin = co.origin;
        return *this;
    }
};

bool operator == (const Coordinate co1, const Coordinate co2)
{
    return (co1.origin == co2.origin) && (co1.x_axis == co2.x_axis) && (co1.y_axis == co2.y_axis) && (co1.z_axis == co2.z_axis);
}