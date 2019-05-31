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
#include "Vector.h"

#include <stdbool.h>

typedef struct {
	int m_attribCount;
	Vector m_indicesIn;   // array
    Vector m_indicesOut;  // array
	Vector *m_vertices; // array
} PolyClipper;

static inline void PolyClipper_construct(PolyClipper *pc)
{
    Vector_init(&pc->m_indicesIn, sizeof(int));
    Vector_init(&pc->m_indicesOut, sizeof(int));
}

static inline void PolyClipper_destruct(PolyClipper *pc)
{
    Vector_free(&pc->m_indicesIn);
    Vector_free(&pc->m_indicesOut);
}

void PolyClipper_init(PolyClipper *pc, Vector *vertices, int i1, int i2, int i3, int attribCount);

// Clip the poly to the plane given by the formula a * x + b * y + c * z + d * w.
void PolyClipper_clipToPlane(PolyClipper *pc, float a, float b, float c, float d);

static inline Vector* PolyClipper_indices(PolyClipper *pc)
{
    return &pc->m_indicesIn;
}

static inline bool PolyClipper_fullyClipped(PolyClipper *pc)
{
    return Vector_size(&pc->m_indicesIn) < 3;
}

static inline VertexShaderOutput interpolateVertex(const VertexShaderOutput *v0, const VertexShaderOutput *v1, float t, int attribCount)
{
    VertexShaderOutput result;

    result.x = v0->x * (1.0f - t) + v1->x * t;
    result.y = v0->y * (1.0f - t) + v1->y * t;
    result.z = v0->z * (1.0f - t) + v1->z * t;
    result.w = v0->w * (1.0f - t) + v1->w * t;
    for (int i = 0; i < attribCount; ++i)
        result.avar[i] = v0->avar[i] * (1.0f - t) + v1->avar[i] * t;

    return result;
}
