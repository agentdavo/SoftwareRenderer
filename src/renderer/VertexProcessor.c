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

#include "VertexProcessor.h"

#include <assert.h>

static inline void swapIntegers(int *first, int *second)
{
    int tmp = *first;
    *first = *second;
    *second = tmp;
}

void VertexProcessor_construct(VertexProcessor *vp, Rasterizer *rasterizer)
{
    // Init vectors
    Vector_init(&vp->m_verticesOut, sizeof(VertexShaderOutput));
    Vector_init(&vp->m_indicesOut, sizeof(int));
    Vector_init(&vp->m_clipMask, sizeof(int));
    Vector_init(&vp->m_alreadyProcessed, sizeof(bool));

    PolyClipper_construct(&vp->m_polyClipper);

    VertexProcessor_setRasterizer(vp, rasterizer);
    VertexProcessor_setCullMode(vp, CM_CW);
    VertexProcessor_setDepthRange(vp, 0.0f, 1.0f);
    VertexProcessor_setVertexShader(vp, 0/*NULL*/);
}

void VertexProcessor_destruct(VertexProcessor *vp)
{
    Vector_free(&vp->m_verticesOut);
    Vector_free(&vp->m_indicesOut);
    Vector_free(&vp->m_clipMask);
    Vector_free(&vp->m_alreadyProcessed);

    PolyClipper_destruct(&vp->m_polyClipper);
}

void VertexProcessor_setRasterizer(VertexProcessor *vp, Rasterizer *rasterizer)
{
	assert(rasterizer != 0);
	vp->m_rasterizer = rasterizer;
}

void VertexProcessor_setViewport(VertexProcessor *vp, int x, int y, int width, int height)
{
	vp->m_viewport.x = x;
	vp->m_viewport.y = y;
	vp->m_viewport.width = width;
	vp->m_viewport.height = height;

	vp->m_viewport.px = width / 2.0f;
	vp->m_viewport.py = height / 2.0f;
	vp->m_viewport.ox = (x + vp->m_viewport.px);
	vp->m_viewport.oy = (y + vp->m_viewport.py);
}

void VertexProcessor_setDepthRange(VertexProcessor *vp, float n, float f)
{
	vp->m_depthRange.n = n;
	vp->m_depthRange.f = f;
}

void VertexProcessor_setCullMode(VertexProcessor *vp, CullMode mode)
{
	vp->m_cullMode = mode;
}

void VertexProcessor_setVertexShader(VertexProcessor *vp, VertexShader *vs)
{
    vp->m_vertexShader = vs;
    if (!vp->m_vertexShader)
        return;

    assert(vs->AttribCount <= MaxVertexAttribs);
    vp->m_attribCount = vp->m_vertexShader->AttribCount;
    vp->m_processVertexFunc = vs->processVertex;
}

void VertexProcessor_setVertexAttribPointer(VertexProcessor *vp, int index, int stride, const void *buffer)
{
	assert(index < MaxVertexAttribs);
	vp->m_attributes[index].buffer = buffer;
	vp->m_attributes[index].stride = stride;
}

void VertexProcessor_drawElements(VertexProcessor *vp, DrawMode mode, unsigned long count, int *indices)
{
	Vector_clear(&vp->m_verticesOut);
	Vector_clear(&vp->m_indicesOut);

	// TODO: Max 1024 primitives per batch.
	VertexCache vCache;
    VertexCache_construct(&vCache);

	for (unsigned long i = 0; i < count; i++)
	{
		int index = indices[i];
		int outputIndex = VertexCache_lookup(&vCache, index);
		
		if (outputIndex != -1)
		{
			Vector_append(&vp->m_indicesOut, outputIndex, int);
		}
		else
		{
            VertexShaderInput vIn;
            VertexProcessor_initVertexInput(vp, vIn, index);

			int outputIndex = Vector_size(&vp->m_verticesOut);
			Vector_append(&vp->m_indicesOut, outputIndex, int);
            Vector_set_size(&vp->m_verticesOut, Vector_size(&vp->m_verticesOut) + 1);
			VertexShaderOutput *vOut = &Vector_back(&vp->m_verticesOut, VertexShaderOutput);
			
			VertexProcessor_processVertex(vp, vIn, vOut);

			VertexCache_set(&vCache, index, outputIndex);
		}

		if (VertexProcessor_primitiveCount(vp, mode) >= 1024)
		{
			VertexProcessor_processPrimitives(vp, mode);
			Vector_clear(&vp->m_verticesOut);
			Vector_clear(&vp->m_indicesOut);
			VertexCache_clear(&vCache);
		}
	}

    VertexProcessor_processPrimitives(vp, mode);
}

int VertexProcessor_clipMask(VertexShaderOutput *v)
{
	int mask = 0;
	if (v->w - v->x < 0) mask |= ClipMask_PosX;
	if (v->x + v->w < 0) mask |= ClipMask_NegX;
	if (v->w - v->y < 0) mask |= ClipMask_PosY;
	if (v->y + v->w < 0) mask |= ClipMask_NegY;
	if (v->w - v->z < 0) mask |= ClipMask_PosZ;
	if (v->z + v->w < 0) mask |= ClipMask_NegZ;
	return mask;
}

const void *VertexProcessor_attribPointer(VertexProcessor *vp, int attribIndex, int elementIndex)
{
	const struct Attribute *attrib = &vp->m_attributes[attribIndex];
	return (char*)attrib->buffer + attrib->stride * elementIndex;
}

void VertexProcessor_processVertex(VertexProcessor *vp, VertexShaderInput in, VertexShaderOutput *out)
{
	(*vp->m_processVertexFunc)(in, out);
}

void VertexProcessor_initVertexInput(VertexProcessor *vp, VertexShaderInput in, int index)
{
	for (int i = 0; i < vp->m_attribCount; ++i)
		in[i] = VertexProcessor_attribPointer(vp, i, index);
}

void VertexProcessor_clipPoints(VertexProcessor *vp)
{
	Vector_clear(&vp->m_clipMask);

    int verticesSize = Vector_size(&vp->m_verticesOut);
    Vector_set_size(&vp->m_clipMask, verticesSize);
	
    for (unsigned long i = 0; i < verticesSize; i++)
    {
        VertexShaderOutput *v = &Vector_element(&vp->m_verticesOut, i, VertexShaderOutput);
        Vector_element(&vp->m_clipMask, i, int) = VertexProcessor_clipMask(v);
    }

    int indicesSize = Vector_size(&vp->m_indicesOut);
	for (unsigned long i = 0; i < indicesSize; i++)
	{
        int idx = Vector_element(&vp->m_indicesOut, i, int);
        int mask = Vector_element(&vp->m_clipMask, idx, int);
		if (mask)
			Vector_element(&vp->m_indicesOut, i, int) = -1;
	}
}

void VertexProcessor_clipLines(VertexProcessor *vp)
{
    Vector_clear(&vp->m_clipMask);
	
    int verticesSize = Vector_size(&vp->m_verticesOut);
    Vector_set_size(&vp->m_clipMask, verticesSize);

    for (unsigned long i = 0; i < verticesSize; i++)
    {
        VertexShaderOutput *v = &Vector_element(&vp->m_verticesOut, i, VertexShaderOutput);
        Vector_element(&vp->m_clipMask, i, int) = VertexProcessor_clipMask(v);
    }

    int indicesSize = Vector_size(&vp->m_indicesOut);
	for (unsigned long i = 0; i < indicesSize; i += 2)
	{
		int index0 = Vector_element(&vp->m_indicesOut, i, int);
		int index1 = Vector_element(&vp->m_indicesOut, i + 1, int);

		VertexShaderOutput v0 = Vector_element(&vp->m_verticesOut, index0, VertexShaderOutput);
		VertexShaderOutput v1 = Vector_element(&vp->m_verticesOut, index1, VertexShaderOutput);

        int mask0 = Vector_element(&vp->m_clipMask, index0, int);
        int mask1 = Vector_element(&vp->m_clipMask, index1, int);

		int clipMask = mask0 | mask1;

        LineClipper lineClipper;
        LineClipper_construct(&lineClipper, &v0, &v1);

		if (clipMask & ClipMask_PosX) LineClipper_clipToPlane(&lineClipper, -1, 0, 0, 1);
		if (clipMask & ClipMask_NegX) LineClipper_clipToPlane(&lineClipper, 1, 0, 0, 1);
		if (clipMask & ClipMask_PosY) LineClipper_clipToPlane(&lineClipper, 0,-1, 0, 1);
		if (clipMask & ClipMask_NegY) LineClipper_clipToPlane(&lineClipper, 0, 1, 0, 1);
		if (clipMask & ClipMask_PosZ) LineClipper_clipToPlane(&lineClipper, 0, 0,-1, 1);
		if (clipMask & ClipMask_NegZ) LineClipper_clipToPlane(&lineClipper, 0, 0, 1, 1);

		if (lineClipper.fullyClipped)
		{
			Vector_element(&vp->m_indicesOut, i, int) = -1;
            Vector_element(&vp->m_indicesOut, i + 1, int) = -1;
			continue;
		}

		if (mask0)
		{
			VertexShaderOutput newV = interpolateVertex(&v0, &v1, lineClipper.t0, vp->m_attribCount);
			Vector_append(&vp->m_verticesOut, newV, VertexShaderOutput);
			Vector_element(&vp->m_indicesOut, i, int) = Vector_size(&vp->m_verticesOut) - 1;
		}

		if (mask1)
		{
			VertexShaderOutput newV = interpolateVertex(&v0, &v1, lineClipper.t1, vp->m_attribCount);
            Vector_append(&vp->m_verticesOut, newV, VertexShaderOutput);
            Vector_element(&vp->m_indicesOut, i + 1, int) = Vector_size(&vp->m_verticesOut) - 1;
		}
	}
}

void VertexProcessor_clipTriangles(VertexProcessor *vp)
{
    Vector_clear(&vp->m_clipMask);
    int verticesSize = Vector_size(&vp->m_verticesOut);
	Vector_set_size(&vp->m_clipMask, verticesSize);

    for (unsigned long i = 0; i < verticesSize; i++)
    {
        VertexShaderOutput *v = &Vector_element(&vp->m_verticesOut, i, VertexShaderOutput);
        Vector_element(&vp->m_clipMask, i, int) = VertexProcessor_clipMask(v);
    }

	unsigned long n = Vector_size(&vp->m_indicesOut);

	for (unsigned long i = 0; i < n; i += 3)
	{
		int i0 = Vector_element(&vp->m_indicesOut, i, int);
		int i1 = Vector_element(&vp->m_indicesOut, i + 1, int);
		int i2 = Vector_element(&vp->m_indicesOut, i + 2, int);

        int mask0 = Vector_element(&vp->m_clipMask, i0, int);
        int mask1 = Vector_element(&vp->m_clipMask, i1, int);
        int mask2 = Vector_element(&vp->m_clipMask, i2, int);

		int clipMask = mask0 | mask1 | mask2;

        PolyClipper_init(&vp->m_polyClipper, &vp->m_verticesOut, i0, i1, i2, vp->m_attribCount);

		if (clipMask & ClipMask_PosX) PolyClipper_clipToPlane(&vp->m_polyClipper, -1, 0, 0, 1);
		if (clipMask & ClipMask_NegX) PolyClipper_clipToPlane(&vp->m_polyClipper, 1, 0, 0, 1);
		if (clipMask & ClipMask_PosY) PolyClipper_clipToPlane(&vp->m_polyClipper, 0, -1, 0, 1);
		if (clipMask & ClipMask_NegY) PolyClipper_clipToPlane(&vp->m_polyClipper, 0, 1, 0, 1);
		if (clipMask & ClipMask_PosZ) PolyClipper_clipToPlane(&vp->m_polyClipper, 0, 0,-1, 1);
		if (clipMask & ClipMask_NegZ) PolyClipper_clipToPlane(&vp->m_polyClipper, 0, 0, 1, 1);

		if (PolyClipper_fullyClipped(&vp->m_polyClipper))
		{
			Vector_element(&vp->m_indicesOut, i, int) = -1;
            Vector_element(&vp->m_indicesOut, i + 1, int) = -1;
            Vector_element(&vp->m_indicesOut, i + 2, int) = -1;
			continue;
		}

		Vector *indices = PolyClipper_indices(&vp->m_polyClipper);

		Vector_element(&vp->m_indicesOut, i, int) = Vector_element(indices, 0, int);
        Vector_element(&vp->m_indicesOut, i + 1, int) = Vector_element(indices, 1, int);
        Vector_element(&vp->m_indicesOut, i + 2, int) = Vector_element(indices, 2, int);
        int indicesSize = Vector_size(indices);
		for (unsigned long idx = 3; idx < indicesSize; ++idx) {
            int i0 = Vector_element(indices, 0, int);
            int i1 = Vector_element(indices, idx - 1, int);
            int i2 = Vector_element(indices, idx, int);
			Vector_append(&vp->m_indicesOut, i0, int);
            Vector_append(&vp->m_indicesOut, i1, int);
            Vector_append(&vp->m_indicesOut, i2, int);
		}
	}
}

void VertexProcessor_clipPrimitives(VertexProcessor *vp, DrawMode mode)
{
	switch (mode)
	{
		case DM_Point:
            VertexProcessor_clipPoints(vp);
			break;
		case DM_Line:
            VertexProcessor_clipLines(vp);
			break;
		case DM_Triangle:
			VertexProcessor_clipTriangles(vp);
			break;
	}
}

void VertexProcessor_processPrimitives(VertexProcessor *vp, DrawMode mode)
{
	VertexProcessor_clipPrimitives(vp, mode);
    VertexProcessor_transformVertices(vp);
    VertexProcessor_drawPrimitives(vp, mode);
}

int VertexProcessor_primitiveCount(VertexProcessor *vp, DrawMode mode)
{
	int factor;
	
	switch (mode)
	{
		case DM_Point: factor = 1; break;
		case DM_Line: factor = 2; break;
		case DM_Triangle: factor = 3; break;
	}

	return Vector_size(&vp->m_indicesOut) / factor;
}

void VertexProcessor_drawPrimitives(VertexProcessor *vp, DrawMode mode)
{
	switch (mode)
	{
		case DM_Triangle:
            VertexProcessor_cullTriangles(vp);
			Rasterizer_drawTriangleList(vp->m_rasterizer, vp->m_verticesOut.data, vp->m_indicesOut.data, Vector_size(&vp->m_indicesOut));
			break;
		case DM_Line:
            Rasterizer_drawLineList(vp->m_rasterizer, vp->m_verticesOut.data, vp->m_indicesOut.data, Vector_size(&vp->m_indicesOut));
			break;
		case DM_Point:
            Rasterizer_drawPointList(vp->m_rasterizer, vp->m_verticesOut.data, vp->m_indicesOut.data, Vector_size(&vp->m_indicesOut));
			break;
	}
}

void VertexProcessor_cullTriangles(VertexProcessor *vp)
{
    int indicesSize = Vector_size(&vp->m_indicesOut);
	for (unsigned long i = 0; i + 3 <= indicesSize; i += 3)
	{
        int idx0 = Vector_element(&vp->m_indicesOut, i, int);

		if (idx0 == -1)
			continue;

        int idx1 = Vector_element(&vp->m_indicesOut, i + 1, int);
        int idx2 = Vector_element(&vp->m_indicesOut, i + 2, int);

		VertexShaderOutput *v0 = &Vector_element(&vp->m_verticesOut, idx0, VertexShaderOutput);
		VertexShaderOutput *v1 = &Vector_element(&vp->m_verticesOut, idx1, VertexShaderOutput);
        VertexShaderOutput *v2 = &Vector_element(&vp->m_verticesOut, idx2, VertexShaderOutput);

		float facing = (v0->x - v1->x) * (v2->y - v1->y) - (v2->x - v1->x) * (v0->y - v1->y);

		if (facing < 0)
		{
            if (vp->m_cullMode == CM_CW)
            {
                Vector_element(&vp->m_indicesOut, i, int) = -1;
                Vector_element(&vp->m_indicesOut, i + 1, int) = -1;
                Vector_element(&vp->m_indicesOut, i + 2, int) = -1;
            }
		}
		else
		{
            if (vp->m_cullMode == CM_CCW)
            {
                Vector_element(&vp->m_indicesOut, i, int) = -1;
                Vector_element(&vp->m_indicesOut, i + 1, int) = -1;
                Vector_element(&vp->m_indicesOut, i + 2, int) = -1;
            }
			else
				swapIntegers(&Vector_element(&vp->m_indicesOut, i, int),
                    &Vector_element(&vp->m_indicesOut, i + 2, int));
		}
	}
}

void VertexProcessor_transformVertices(VertexProcessor *vp)
{
	Vector_clear(&vp->m_alreadyProcessed);
    Vector_set_size(&vp->m_alreadyProcessed, Vector_size(&vp->m_verticesOut));

    int indicesSize = Vector_size(&vp->m_indicesOut);
	for (unsigned long i = 0; i < indicesSize; i++)
	{
		int index = Vector_element(&vp->m_indicesOut, i, int);

		if (index == -1)
			continue;

		if (Vector_element(&vp->m_alreadyProcessed, index, bool))
			continue;

        VertexShaderOutput *vOut = &Vector_element(&vp->m_verticesOut, index, VertexShaderOutput);

		// Perspective divide
		float invW = 1.0f / vOut->w;
		vOut->x *= invW;
		vOut->y *= invW;
		vOut->z *= invW;

		// Viewport transform
		vOut->x = (vp->m_viewport.px * vOut->x + vp->m_viewport.ox);
		vOut->y = (vp->m_viewport.py * -vOut->y + vp->m_viewport.oy);
		vOut->z = 0.5f * (vp->m_depthRange.f - vp->m_depthRange.n) * vOut->z + 0.5f * (vp->m_depthRange.n + vp->m_depthRange.f);

		Vector_element(&vp->m_alreadyProcessed, index, bool) = true;
	}
}
