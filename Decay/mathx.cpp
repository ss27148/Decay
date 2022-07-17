// Copyright (c) 2018 SS

#include "stdafx.h"
#include "mathx.h"

#define _USE_MATH_DEFINES
#include <math.h>


double rad(double angle_deg)
{
	return angle_deg * M_PI / 180.;
}

float rad(float angle_deg)
{
	return (float) (angle_deg * M_PI / 180.f);
}

double deg(double angle_rad)
{
	return angle_rad * 180. / M_PI;
}

float deg(float angle_rad)
{
	return (float) (angle_rad * 180.f / M_PI);
}
