// FontSDF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "lodepng.h"

#define INPUT_NAME "font.png"
#define OUTPUT_NAME "fontSDF"
#define OUTPUT_WIDTH 32		
#define OUTPUT_HEIGHT 32

#define MAX_DISTANCE 128

unsigned char* input;
unsigned input_width, input_height;
double max_distance = -MAX_DISTANCE;
double min_distance = MAX_DISTANCE;

bool checkPixel(int x, int y)
{
	int nIndex = (x + y * input_width) << 2;
	if ((input[nIndex] == 255))
	{
		return true;
	}
	return false;
}


void generateSDF(int searchDistance)
{
	std::vector<int> pixels;
	std::vector<float> targetPixels;
	pixels.resize(input_width * input_height);
	targetPixels.resize(OUTPUT_WIDTH * OUTPUT_HEIGHT);
	for (int y = 0; y < input_height; ++y)
	{
		for (int x = 0; x < input_width; ++x)
		{
			if (checkPixel(x, y))
			{
				pixels[x + y * input_width] = 1;
			}
			else
			{
				pixels[x + y * input_width] = 0;
			}
		}
	}
	int gapX = input_width / OUTPUT_WIDTH;
	int gapY = input_height / OUTPUT_HEIGHT;
	int minx, maxx, miny, maxy;
	int MAX_SERCH_DIST = searchDistance;
	float max_distance = -MAX_SERCH_DIST;
	float min_distance = MAX_SERCH_DIST;
	for (int x = 0; x < OUTPUT_WIDTH; ++x)
	{
		for (int y = 0; y < OUTPUT_HEIGHT; ++y)
		{
			int sourceX = x * gapX;
			int sourceY = y * gapY;
			int min = MAX_SERCH_DIST;
			minx = sourceX - MAX_SERCH_DIST;
			if (minx < 0)
			{
				minx = 0;
			}
			miny = sourceY - MAX_SERCH_DIST;
			if (miny < 0)
			{
				miny = 0;
			}
			maxx = sourceX + MAX_SERCH_DIST;
			if (maxx > input_width)
			{
				maxx = input_width;
			}
			maxy = sourceY + MAX_SERCH_DIST;
			if (maxy > input_height)
			{
				maxy = input_height;
			}
			int dx, dy, iy, ix, distance;
			bool sourceInside = pixels[sourceX + sourceY * input_width] == 1 ? true : false;
			if (sourceInside)
			{
				for (int iy = miny; iy < maxy; ++iy)
				{
					dy = iy - sourceY;
					dy *= dy;
					for (int ix = minx; ix < maxx; ++ix)
					{
						bool bTargetInside = pixels[ix + iy * input_width] == 1 ? true : false;
						if (bTargetInside)
						{
							continue;
						}
						dx = ix - sourceX;
						distance = (int)sqrt(dx * dx + dy);
						if (distance < min)
						{
							min = distance;
						}
					}
				}
				if (min > max_distance)
				{
					max_distance = min;
				}
				targetPixels[x + y * OUTPUT_WIDTH] = min;
			}
			else
			{
				for (int iy = miny; iy < maxy; ++iy)
				{
					dy = iy - sourceY;
					dy *= dy;
					for (int ix = minx; ix < maxx; ++ix)
					{
						bool bTargetInside = pixels[ix + iy * input_width] == 1 ? true : false;
						if (!bTargetInside)
						{
							continue;
						}
						dx = ix - sourceX;
						distance = (int)sqrt(dx * dx + dy);
						if (distance < min)
						{
							min = distance;
						}
					}
				}
				if (-min < min_distance)
				{
					min_distance = -min;
				}
				targetPixels[x + y * OUTPUT_WIDTH] = min;
			}
		}
	}
	float clampDist = max_distance - min_distance;
	for (int x = 0; x < OUTPUT_WIDTH; ++x)
	{
		for (int y = 0; y < OUTPUT_HEIGHT; ++y)
		{
			targetPixels[x + y * OUTPUT_WIDTH] -= min_distance;
			targetPixels[x + y * OUTPUT_WIDTH] = (targetPixels[x + y * OUTPUT_WIDTH] / clampDist) * 255;
		}
	}

	int png_i = 0;
	auto debug_png = (unsigned char*)malloc(OUTPUT_WIDTH * OUTPUT_HEIGHT * 4);
	for (int y = 0; y < OUTPUT_HEIGHT; y++)
	{
		for (int x = 0; x < OUTPUT_WIDTH; ++x)
		{
			int nIndex = x + y * OUTPUT_WIDTH;
			debug_png[png_i + 0] = 255;
			debug_png[png_i + 1] = 255;
			debug_png[png_i + 2] = 255;
			debug_png[png_i + 3] = targetPixels[nIndex];

			png_i += 4;
		}
	}
	lodepng_encode32_file(OUTPUT_NAME ".png", debug_png, OUTPUT_WIDTH, OUTPUT_HEIGHT);
	free(debug_png);
}


int main()
{
	unsigned error = lodepng_decode32_file(&input, &input_width, &input_height, INPUT_NAME);
	if (error) 
	{
		fprintf(stderr, "Error %u: %s\n", error, lodepng_error_text(error));
		exit(1);
	}
	generateSDF(MAX_DISTANCE);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
