// Copyright (C) 2022, Francis LaBounty, All rights reserved.

#pragma once

#include <math.h>

using namespace std;

inline float clamp(float val, float min, float max)
{
	return fmin(fmax(val, min), max);
}

inline void swap(int& a, int& b)
{
	int t = a;
	a = b;
	b = t;
}

inline void swap(float& a, float& b)
{
	float t = a;
	a = b;
	b = t;
}

void run_cpu(float* height_array, float* h_mask_out, int width, int height, int raylength, int raycount, bool tiled);