// Copyright (c) 2018 SS

#pragma once

double rad(double angle_deg);
float rad(float angle_deg);

double deg(double angle_rad);
float deg(float angle_rad);

template<class T> T pow2(T x);
template<class T> T pow3(T x);
template<class T> T pow4(T x);
template<class T> T pow5(T x);
template<class T> T pow6(T x);

template<class T>
T pow2(T x)
{
	return x * x;
}

template<class T>
T pow3(T x)
{
	return x * x * x;
}

template<class T>
T pow4(T x)
{
	return x * x * x * x;
}

template<class T>
T pow5(T x)
{
	return x * x * x * x * x;
}

template<class T>
T pow6(T x)
{
	return x * x * x * x * x * x;
}
