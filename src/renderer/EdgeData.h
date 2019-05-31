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

#include "TriangleEquations.h"

typedef struct EdgeData {
	float ev0;
	float ev1;
	float ev2;
} EdgeData;

// Initialize the edge data values.
static void EdgeData_init(EdgeData *ed, const TriangleEquations *eqn, float x, float y)
{
    ed->ev0 = EdgeEquation_evaluate(&eqn->e0, x, y);
    ed->ev1 = EdgeEquation_evaluate(&eqn->e1, x, y);
    ed->ev2 = EdgeEquation_evaluate(&eqn->e2, x, y);
}

// Step the edge values in the x direction.
static void EdgeData_stepX(EdgeData *ed, const TriangleEquations *eqn)
{
    ed->ev0 = EdgeEquation_stepX(&eqn->e0, ed->ev0);
    ed->ev1 = EdgeEquation_stepX(&eqn->e1, ed->ev1);
    ed->ev2 = EdgeEquation_stepX(&eqn->e2, ed->ev2);
}

// Step the edge values in the x direction.
static void EdgeData_stepX2(EdgeData *ed, const TriangleEquations *eqn, float stepSize)
{
    ed->ev0 = EdgeEquation_stepX2(&eqn->e0, ed->ev0, stepSize);
    ed->ev1 = EdgeEquation_stepX2(&eqn->e1, ed->ev1, stepSize);
    ed->ev2 = EdgeEquation_stepX2(&eqn->e2, ed->ev2, stepSize);
}

// Step the edge values in the y direction.
static void EdgeData_stepY(EdgeData *ed, const TriangleEquations *eqn)
{
    ed->ev0 = EdgeEquation_stepY(&eqn->e0, ed->ev0);
    ed->ev1 = EdgeEquation_stepY(&eqn->e1, ed->ev1);
    ed->ev2 = EdgeEquation_stepY(&eqn->e2, ed->ev2);
}

// Step the edge values in the y direction.
static void EdgeData_stepY2(EdgeData *ed, const TriangleEquations *eqn, float stepSize)
{
    ed->ev0 = EdgeEquation_stepY2(&eqn->e0, ed->ev0, stepSize);
    ed->ev1 = EdgeEquation_stepY2(&eqn->e1, ed->ev1, stepSize);
    ed->ev2 = EdgeEquation_stepY2(&eqn->e2, ed->ev2, stepSize);
}

// Test for triangle containment.
static bool EdgeData_test(const EdgeData *ed, const TriangleEquations *eqn)
{
    return EdgeEquation_testValue(&eqn->e0, ed->ev0) &&
        EdgeEquation_testValue(&eqn->e1, ed->ev1) &&
        EdgeEquation_testValue(&eqn->e2, ed->ev2);
}