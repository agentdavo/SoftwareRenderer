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

#include "EdgeEquation.h"

typedef struct ParameterEquation {
	float a;
	float b;
	float c;
} ParameterEquation;

static inline void ParameterEquation_init(
    ParameterEquation *pe,
    float p0,
    float p1,
    float p2,
    const EdgeEquation *e0,
    const EdgeEquation *e1,
    const EdgeEquation *e2,
    float factor)
{
    pe->a = factor * (p0 * e0->a + p1 * e1->a + p2 * e2->a);
    pe->b = factor * (p0 * e0->b + p1 * e1->b + p2 * e2->b);
    pe->c = factor * (p0 * e0->c + p1 * e1->c + p2 * e2->c);
}

// Evaluate the parameter equation for the given point.
static inline float ParameterEquation_evaluate(const ParameterEquation *pe, float x, float y)
{
    return pe->a * x + pe->b * y + pe->c;
}

// Step parameter value v in x direction.
static inline float ParameterEquation_stepX(const ParameterEquation *pe, float v)
{
    return v + pe->a;
}

// Step parameter value v in x direction.
static inline float ParameterEquation_stepX2(const ParameterEquation *pe, float v, float stepSize)
{
    return v + pe->a * stepSize;
}

// Step parameter value v in y direction.
static inline float ParameterEquation_stepY(const ParameterEquation *pe, float v)
{
    return v + pe->b;
}

// Step parameter value v in y direction.
static inline float ParameterEquation_stepY2(const ParameterEquation *pe, float v, float stepSize)
{
    return v + pe->b * stepSize;
}
