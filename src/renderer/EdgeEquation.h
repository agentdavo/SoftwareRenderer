/*
MIT License

Copyright (c) 2017 trenki2

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Renderer.h"

#include <stdbool.h>

typedef struct EdgeEquation {
	float a;
	float b;
	float c;
	bool tie; 
} EdgeEquation;

static inline void EdgeEquation_init(EdgeEquation *ee, const RasterizerVertex *v0, const RasterizerVertex *v1)
{
    ee->a = v0->y - v1->y;
    ee->b = v1->x - v0->x;
    ee->c = -(ee->a * (v0->x + v1->x) + ee->b * (v0->y + v1->y)) / 2;
    ee->tie = ee->a != 0 ? ee->a > 0 : ee->b > 0;
}

// Evaluate the edge equation for the given point.
static inline float EdgeEquation_evaluate(const EdgeEquation *ee, float x, float y)
{
    return ee->a * x + ee->b * y + ee->c;
}

// Test for a given evaluated value.
static inline bool EdgeEquation_testValue(const EdgeEquation *ee, float v)
{
    return (v > 0 || (v == 0 && ee->tie));
}

// Test if the given point is inside the edge.
static inline bool EdgeEquation_testPoint(const EdgeEquation *ee, float x, float y)
{
    return EdgeEquation_testValue(ee, EdgeEquation_evaluate(ee, x, y));
}

// Step the equation value v to the x direction
static inline float EdgeEquation_stepX(const EdgeEquation *ee, float v)
{
    return v + ee->a;
}

// Step the equation value v to the x direction
static inline float EdgeEquation_stepX2(const EdgeEquation *ee, float v, float stepSize)
{
    return v + ee->a * stepSize;
}

// Step the equation value v to the y direction
static inline float EdgeEquation_stepY(const EdgeEquation *ee, float v)
{
    return v + ee->b;
}

// Step the equation value vto the y direction
static inline float EdgeEquation_stepY2(const EdgeEquation *ee, float v, float stepSize)
{
    return v + ee->b * stepSize;
}