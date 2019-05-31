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

/** @file */

#include "Renderer.h"
#include "TriangleEquations.h"

#include <stdbool.h>

// Initialize pixel data for the given pixel coordinates.
inline void PixelData_init(PixelData *pd, const TriangleEquations *eqn, float x, float y, int aVarCount, int pVarCount, bool interpolateZ, bool interpolateW)
{
    if (interpolateZ)
        pd->z = ParameterEquation_evaluate(&eqn->z, x, y);

    if (interpolateW || pVarCount > 0)
    {
        pd->invw = ParameterEquation_evaluate(&eqn->invw, x, y);
        pd->w = 1.0f / pd->invw;
    }

    for (int i = 0; i < aVarCount; ++i)
        pd->avar[i] = ParameterEquation_evaluate(&eqn->avar[i], x, y);

    for (int i = 0; i < pVarCount; ++i)
    {
        pd->pvarTemp[i] = ParameterEquation_evaluate(&eqn->pvar[i], x, y);
        pd->pvar[i] = pd->pvarTemp[i] * pd->w;
    }
}

// Step all the pixel data in the x direction.
inline void PixelData_stepX(PixelData *pd, const TriangleEquations *eqn, int aVarCount, int pVarCount, bool interpolateZ, bool interpolateW)
{
    if (interpolateZ)
        pd->z = ParameterEquation_stepX(&eqn->z, pd->z);

    if (interpolateW || pVarCount > 0)
    {
        pd->invw = ParameterEquation_stepX(&eqn->invw, pd->invw);
        pd->w = 1.0f / pd->invw;
    }

    for (int i = 0; i < aVarCount; ++i)
        pd->avar[i] = ParameterEquation_stepX(&eqn->avar[i], pd->avar[i]);

    for (int i = 0; i < pVarCount; ++i)
    {
        pd->pvarTemp[i] = ParameterEquation_stepX(&eqn->pvar[i], pd->pvarTemp[i]);
        pd->pvar[i] = pd->pvarTemp[i] * pd->w;
    }
}

// Step all the pixel data in the y direction.
inline void PixelData_stepY(PixelData *pd, const TriangleEquations *eqn, int aVarCount, int pVarCount, bool interpolateZ, bool interpolateW)
{
    if (interpolateZ)
        pd->z = ParameterEquation_stepY(&eqn->z, pd->z);

    if (interpolateW || pVarCount > 0)
    {
        pd->invw = ParameterEquation_stepY(&eqn->invw, pd->invw);
        pd->w = 1.0f / pd->invw;
    }

    for (int i = 0; i < aVarCount; ++i)
        pd->avar[i] = ParameterEquation_stepY(&eqn->avar[i], pd->avar[i]);

    for (int i = 0; i < pVarCount; ++i)
    {
        pd->pvarTemp[i] = ParameterEquation_stepY(&eqn->pvar[i], pd->pvarTemp[i]);
        pd->pvar[i] = pd->pvarTemp[i] * pd->w;
    }
}