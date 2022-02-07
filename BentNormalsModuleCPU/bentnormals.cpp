// Copyright (C) 2022, Francis LaBounty, All rights reserved.

#include "bentnormals.h"

float raycast(float* height_array, int y0, int x0, int width, float angle, int length)
{
	int baserow = y0;
	int basecol = x0;

	int x1 = x0 + cosf(angle) * length;
	int y1 = y0 + sinf(angle) * length;

	bool steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep)
	{
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	int error = deltax / 2;
	int y = y0;
	int ystep = (y0 < y1) ? 1 : -1;
	float maxelevation = 0.f;

	for (int x = x0; x <= x1; x++)
	{
		int row, col;
		float distance;
		if (steep)
		{
			row = x;
			col = y;
		}
		else
		{
			row = y;
			col = x;
		}

		distance = sqrtf((row - baserow) * (row - baserow) + (col - basecol) * (col - basecol));
		maxelevation = fmax(maxelevation, (height_array[row * width + col] - height_array[baserow * width + basecol]) / fmax(1.f, distance));

		error = error - deltay;
		if (error < 0.f)
		{
			y = y + ystep;
			error = error + deltax;
		}
	}

	return maxelevation;
}

void run_cpu(float* height_array, float* h_mask_out, int width, int height, int raylength, int raycount, bool tiled)
{
	float anglestep = 2.f * (float)M_PI / raycount;

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			float xsum = 0.f;
			float ysum = 0.f;

			float averagex = 0.f; // R
			float averagey = 0.f; // G

			for (float a = 0.f; a < 2.f * (float)M_PI; a += anglestep)
			{
				float xdir = cosf(a);
				float ydir = sinf(a);

				xsum += fabsf(xdir);
				ysum += fabsf(ydir);

				float ray = raycast(height_array, row, col, width, a, raylength);

				averagex += xdir * ray;
				averagey += ydir * ray;
			}

			averagex /= xsum;
			averagey /= ysum;

			h_mask_out[(row * width * 4) + (4 * col)] = averagex; // R
			h_mask_out[(row * width * 4) + (4 * col) + 1] = averagey; // G
			h_mask_out[(row * width * 4) + (4 * col) + 2] = averagey; // B store copy of averagey so future min-max scaling isn't affected
			h_mask_out[(row * width * 4) + (4 * col) + 3] = averagex; // A store copy of averagex so future min-max scaling isn't affected
		}
	}
}