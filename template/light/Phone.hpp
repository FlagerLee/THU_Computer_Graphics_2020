#pragma once

#include "../utils.hpp"

//using normalize BRDF
//cosa: angle between out ray and camera ray
//n: controls highlight
double Phone_BRDF(double K_d, double K_s, double cosa, int n)
{
    double d1 = K_d / PI;
    double d2 = (n + 2) * pow(cosa, n) / (2 * PI);
    return d1 + d2;
}