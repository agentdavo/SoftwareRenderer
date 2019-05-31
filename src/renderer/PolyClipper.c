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

#include "PolyClipper.h"

static inline int sgn(float val)
{
    return (0.f < val) - (val < 0.f);
}

void PolyClipper_init(PolyClipper *pc, Vector *vertices, int i1, int i2, int i3, int attribCount)
{
	pc->m_attribCount = attribCount;
	pc->m_vertices = vertices;

    Vector_clear(&pc->m_indicesIn);
    Vector_clear(&pc->m_indicesOut);
	
	Vector_append(&pc->m_indicesIn, i1, int);
    Vector_append(&pc->m_indicesIn, i2, int);
    Vector_append(&pc->m_indicesIn, i3, int);
}

void PolyClipper_clipToPlane(PolyClipper *pc, float a, float b, float c, float d)
{
	if (PolyClipper_fullyClipped(pc))
		return;

    Vector_clear(&pc->m_indicesOut);

	int idxPrev = Vector_element(&pc->m_indicesIn, 0, int);
	Vector_append(&pc->m_indicesIn, idxPrev, int);

	VertexShaderOutput *vPrev = &Vector_element(pc->m_vertices, idxPrev, VertexShaderOutput);
	float dpPrev = a * vPrev->x + b * vPrev->y + c * vPrev->z + d * vPrev->w;

    unsigned long indicesInSize = Vector_size(&pc->m_indicesIn);
	
    for (unsigned long i = 1; i < indicesInSize; ++i)
	{
		int idx = Vector_element(&pc->m_indicesIn, i, int);
		VertexShaderOutput *v = &Vector_element(pc->m_vertices, idx, VertexShaderOutput);
		float dp = a * v->x + b * v->y + c * v->z + d * v->w;

		if (dpPrev >= 0)
			Vector_append(&pc->m_indicesOut, idxPrev, int);

		if (sgn(dp) != sgn(dpPrev))
		{
			float t = dp < 0 ? dpPrev / (dpPrev - dp) : -dpPrev / (dp - dpPrev);

            VertexShaderOutput *v0, *v1;
            v0 = &Vector_element(pc->m_vertices, idxPrev, VertexShaderOutput);
            v1 = &Vector_element(pc->m_vertices, idx, VertexShaderOutput);
			VertexShaderOutput vOut = interpolateVertex(v0, v1, t, pc->m_attribCount);
			Vector_append(pc->m_vertices, vOut, VertexShaderOutput);
            int idxOut = Vector_size(pc->m_vertices) - 1;
			Vector_append(&pc->m_indicesOut, idxOut, int);
		}

		idxPrev = idx;
		dpPrev = dp;
	}

	Vector_swap(&pc->m_indicesIn, &pc->m_indicesOut);
}
