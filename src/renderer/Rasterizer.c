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

#include "Rasterizer.h"
#include "EdgeEquation.h"
#include "EdgeData.h"

#include <stdlib.h>


static inline void swap_ptrs(const void **ptr1, const void **ptr2)
{
    const void *tmp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = tmp;
}

void Rasterizer_construct(Rasterizer *rs)
{
    Rasterizer_setRasterMode(rs, RM_Span);
    Rasterizer_setScissorRect(rs, 0, 0, 0, 0);
    Rasterizer_setPixelShader(rs, 0);
}

void Rasterizer_setRasterMode(Rasterizer *rs, RasterMode mode)
{
    rs->rasterMode = mode;
}

/// Set the scissor rectangle.
void Rasterizer_setScissorRect(Rasterizer *rs, int x, int y, int width, int height)
{
    rs->m_minX = x;
    rs->m_minY = y;
    rs->m_maxX = x + width;
    rs->m_maxY = y + height;
}

void Rasterizer_setPixelShader(Rasterizer *rs, PixelShader *ps)
{
    rs->m_pixelShader = ps;
    rs->m_triangleFunc = Rasterizer_drawTriangleModeTemplate;
    rs->m_lineFunc = Rasterizer_drawLineTemplate;
    rs->m_pointFunc = Rasterizer_drawPointTemplate;
}

void Rasterizer_drawPoint(Rasterizer *rs, const RasterizerVertex *v)
{
    (*rs->m_pointFunc)(rs, v);
}

void Rasterizer_drawLine(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1)
{
    (*rs->m_lineFunc)(rs, v0, v1);
}

void Rasterizer_drawTriangle(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2)
{
    (*rs->m_triangleFunc)(rs, v0, v1, v2);
}

void Rasterizer_drawPointList(Rasterizer *rs, const RasterizerVertex *vertices, const int *indices, unsigned long indexCount)
{
    for (unsigned long i = 0; i < indexCount; ++i) {
        if (indices[i] == -1)
            continue;
        Rasterizer_drawPoint(rs, &vertices[indices[i]]);
    }
}

void Rasterizer_drawLineList(Rasterizer *rs, const RasterizerVertex *vertices, const int *indices, unsigned long indexCount)
{
    for (unsigned long i = 0; i + 2 <= indexCount; i += 2) {
        if (indices[i] == -1)
            continue;
        Rasterizer_drawLine(rs, &vertices[indices[i]], &vertices[indices[i + 1]]);
    }
}

void Rasterizer_drawTriangleList(Rasterizer *rs, const RasterizerVertex *vertices, const int *indices, unsigned long indexCount)
{
    for (unsigned long i = 0; i + 3 <= indexCount; i += 3) {
        if (indices[i] == -1)
            continue;
        Rasterizer_drawTriangle(rs, &vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);
    }
}

bool Rasterizer_scissorTest(Rasterizer *rs, float x, float y)
{
    return (x >= rs->m_minX && x < rs->m_maxX && y >= rs->m_minY && y < rs->m_maxY);
}

void Rasterizer_drawPointTemplate(Rasterizer *rs, const RasterizerVertex *v)
{
    // Check scissor rect
    if (!Rasterizer_scissorTest(rs, v->x, v->y))
        return;

    PixelData p = Rasterizer_pixelDataFromVertex(rs, v);
    if (rs->m_pixelShader && rs->m_pixelShader->drawPixel)
        rs->m_pixelShader->drawPixel(&p);
}

PixelData Rasterizer_pixelDataFromVertex(Rasterizer *rs, const RasterizerVertex *v)
{
    PixelData p;
    p.x = (int)v->x;
    p.y = (int)v->y;
    if (rs->m_pixelShader->InterpolateZ) p.z = v->z;
    if (rs->m_pixelShader->InterpolateW) p.invw = 1.0f / v->w;
    for (int i = 0; i < rs->m_pixelShader->AVarCount; ++i)
        p.avar[i] = v->avar[i];
    return p;
}

void Rasterizer_drawLineTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1)
{
    int adx = abs((int)v1->x - (int)v0->x);
    int ady = abs((int)v1->y - (int)v0->y);
    int steps = max(adx, ady);

    RasterizerVertex step = Rasterizer_computeVertexStep(rs, v0, v1, steps);

    RasterizerVertex v = *v0;
    while (steps-- > 0)
    {
        PixelData p = Rasterizer_pixelDataFromVertex(rs, &v);

        if (Rasterizer_scissorTest(rs, v.x, v.y))
            if (rs->m_pixelShader && rs->m_pixelShader->drawPixel)
                rs->m_pixelShader->drawPixel(&p);

        Rasterizer_stepVertex(rs, &v, &step);
    }
}

void Rasterizer_stepVertex(Rasterizer *rs, RasterizerVertex *v, RasterizerVertex *step)
{
    v->x += step->x;
    v->y += step->y;
    if (rs->m_pixelShader->InterpolateZ) v->z += step->z;
    if (rs->m_pixelShader->InterpolateW) v->w += step->w;
    for (int i = 0; i < rs->m_pixelShader->AVarCount; ++i)
        v->avar[i] += step->avar[i];
}

RasterizerVertex Rasterizer_computeVertexStep(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, int adx)
{
    RasterizerVertex step;
    step.x = (v1->x - v0->x) / adx;
    step.y = (v1->y - v0->y) / adx;
    if (rs->m_pixelShader->InterpolateZ) step.z = (v1->z - v0->z) / adx;
    if (rs->m_pixelShader->InterpolateW) step.w = (v1->w - v0->w) / adx;
    for (int i = 0; i < rs->m_pixelShader->AVarCount; ++i)
        step.avar[i] = (v1->avar[i] - v0->avar[i]) / adx;
    return step;
}

void Rasterizer_drawTriangleBlockTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2)
{
    // Compute triangle equations.
    TriangleEquations eqn;
    TriangleEquations_construct(&eqn, v0, v1, v2, rs->m_pixelShader->AVarCount, rs->m_pixelShader->PVarCount);

    // Check if triangle is backfacing.
    if (eqn.area2 <= 0)
        return;

    // Compute triangle bounding box.
    int minX = (int)min(min(v0->x, v1->x), v2->x);
    int maxX = (int)max(max(v0->x, v1->x), v2->x);
    int minY = (int)min(min(v0->y, v1->y), v2->y);
    int maxY = (int)max(max(v0->y, v1->y), v2->y);

    // Clip to scissor rect.
    minX = max(minX, rs->m_minX);
    maxX = min(maxX, rs->m_maxX);
    minY = max(minY, rs->m_minY);
    maxY = min(maxY, rs->m_maxY);

    // Round to block grid.
    minX = minX & ~(BlockSize - 1);
    maxX = maxX & ~(BlockSize - 1);
    minY = minY & ~(BlockSize - 1);
    maxY = maxY & ~(BlockSize - 1);

    float s = BlockSize - 1;

    int stepsX = (maxX - minX) / BlockSize + 1;
    int stepsY = (maxY - minY) / BlockSize + 1;

    int i;
#pragma omp parallel for
    for (i = 0; i < stepsX * stepsY; ++i)
    {
        int sx = i % stepsX;
        int sy = i / stepsX;

        // Add 0.5 to sample at pixel centers.
        int x = minX + sx * BlockSize;
        int y = minY + sy * BlockSize;

        float xf = x + 0.5f;
        float yf = y + 0.5f;

        // Test if block is inside or outside triangle or touches it.
        EdgeData e00; EdgeData_init(&e00, &eqn, xf, yf);
        EdgeData e01 = e00; EdgeData_stepY2(&e01, &eqn, s);
        EdgeData e10 = e00; EdgeData_stepX2(&e10, &eqn, s);
        EdgeData e11 = e01; EdgeData_stepX2(&e11, &eqn, s);

        bool e00_0 = EdgeEquation_testValue(&eqn.e0, e00.ev0), e00_1 = EdgeEquation_testValue(&eqn.e1, e00.ev1), e00_2 = EdgeEquation_testValue(&eqn.e2, e00.ev2), e00_all = e00_0 && e00_1 && e00_2;
        bool e01_0 = EdgeEquation_testValue(&eqn.e0, e01.ev0), e01_1 = EdgeEquation_testValue(&eqn.e1, e01.ev1), e01_2 = EdgeEquation_testValue(&eqn.e2, e01.ev2), e01_all = e01_0 && e01_1 && e01_2;
        bool e10_0 = EdgeEquation_testValue(&eqn.e0, e10.ev0), e10_1 = EdgeEquation_testValue(&eqn.e1, e10.ev1), e10_2 = EdgeEquation_testValue(&eqn.e2, e10.ev2), e10_all = e10_0 && e10_1 && e10_2;
        bool e11_0 = EdgeEquation_testValue(&eqn.e0, e11.ev0), e11_1 = EdgeEquation_testValue(&eqn.e1, e11.ev1), e11_2 = EdgeEquation_testValue(&eqn.e2, e11.ev2), e11_all = e11_0 && e11_1 && e11_2;

        int result = e00_all + e01_all + e10_all + e11_all;

        // Potentially all out.
        if (result == 0)
        {
            // Test for special case.
            bool e00Same = e00_0 == e00_1 == e00_2;
            bool e01Same = e01_0 == e01_1 == e01_2;
            bool e10Same = e10_0 == e10_1 == e10_2;
            bool e11Same = e11_0 == e11_1 == e11_2;

            if (!e00Same || !e01Same || !e10Same || !e11Same)
                PixelShader_drawBlock(rs->m_pixelShader, &eqn, x, y, true);
        }
        else if (result == 4)
        {
            // Fully Covered.
            PixelShader_drawBlock(rs->m_pixelShader, &eqn, x, y, false);
        }
        else
        {
            // Partially Covered.
            PixelShader_drawBlock(rs->m_pixelShader, &eqn, x, y, true);
        }
    }
}

void Rasterizer_drawTriangleSpanTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2)
{
    // Compute triangle equations.
    TriangleEquations eqn;
    TriangleEquations_construct(&eqn, v0, v1, v2, rs->m_pixelShader->AVarCount, rs->m_pixelShader->PVarCount);

    // Check if triangle is backfacing.
    if (eqn.area2 <= 0)
        return;

    const RasterizerVertex *t = v0;
    const RasterizerVertex *m = v1;
    const RasterizerVertex *b = v2;

    // Sort vertices from top to bottom.
    if (t->y > m->y) swap_ptrs(&t, &m);
    if (m->y > b->y) swap_ptrs(&m, &b);
    if (t->y > m->y) swap_ptrs(&t, &m);

    float dy = (b->y - t->y);
    float iy = (m->y - t->y);

    if (m->y == t->y)
    {
        const RasterizerVertex *l = m, *r = t;
        if (l->x > r->x) swap_ptrs(&l, &r);
        Rasterizer_drawTopFlatTriangle(rs, &eqn, l, r, b);
    }
    else if (m->y == b->y)
    {
        const RasterizerVertex *l = m, *r = b;
        if (l->x > r->x) swap_ptrs(&l, &r);
        Rasterizer_drawBottomFlatTriangle(rs, &eqn, t, l, r);
    }
    else
    {
        RasterizerVertex v4;
        v4.y = m->y;
        v4.x = t->x + ((b->x - t->x) / dy) * iy;
        if (rs->m_pixelShader->InterpolateZ) v4.z = t->z + ((b->z - t->z) / dy) * iy;
        if (rs->m_pixelShader->InterpolateW) v4.w = t->w + ((b->w - t->w) / dy) * iy;
        for (int i = 0; i < rs->m_pixelShader->AVarCount; ++i)
            v4.avar[i] = t->avar[i] + ((b->avar[i] - t->avar[i]) / dy) * iy;

        const RasterizerVertex *l = m, *r = &v4;
        if (l->x > r->x) swap_ptrs(&l, &r);

        Rasterizer_drawBottomFlatTriangle(rs, &eqn, t, l, r);
        Rasterizer_drawTopFlatTriangle(rs, &eqn, l, r, b);
    }
}

void Rasterizer_drawBottomFlatTriangle(Rasterizer *rs, const TriangleEquations *eqn, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2)
{
    float invslope1 = (v1->x - v0->x) / (v1->y - v0->y);
    float invslope2 = (v2->x - v0->x) / (v2->y - v0->y);

    //float curx1 = v0.x;
    //float curx2 = v0.x;

    int scanlineY;
#pragma omp parallel for
    for (scanlineY = (int)(v0->y + 0.5f); scanlineY < (int)(v1->y + 0.5f); scanlineY++)
    {
        float dy = (scanlineY - v0->y) + 0.5f;
        float curx1 = v0->x + invslope1 * dy + 0.5f;
        float curx2 = v0->x + invslope2 * dy + 0.5f;

        // Clip to scissor rect
        int xl = max(rs->m_minX, (int)curx1);
        int xr = min(rs->m_maxX, (int)curx2);

        PixelShader_drawSpan(rs->m_pixelShader, eqn, xl, scanlineY, xr);

        // curx1 += invslope1;
        // curx2 += invslope2;
    }
}

void Rasterizer_drawTopFlatTriangle(Rasterizer *rs, const TriangleEquations *eqn, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2)
{
    float invslope1 = (v2->x - v0->x) / (v2->y - v0->y);
    float invslope2 = (v2->x - v1->x) / (v2->y - v1->y);

    // float curx1 = v2.x;
    // float curx2 = v2.x;

    int scanlineY;
#pragma omp parallel for
    for (scanlineY = (int)(v2->y - 0.5f); scanlineY > (int)(v0->y - 0.5f); scanlineY--)
    {
        float dy = (scanlineY - v2->y) + 0.5f;
        float curx1 = v2->x + invslope1 * dy + 0.5f;
        float curx2 = v2->x + invslope2 * dy + 0.5f;

        // Clip to scissor rect
        int xl = max(rs->m_minX, (int)curx1);
        int xr = min(rs->m_maxX, (int)curx2);

        PixelShader_drawSpan(rs->m_pixelShader, eqn, xl, scanlineY, xr);
        // curx1 -= invslope1;
        // curx2 -= invslope2;
    }
}

void Rasterizer_drawTriangleAdaptiveTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2)
{
    // Compute triangle bounding box.
    float minX = (float)min(min(v0->x, v1->x), v2->x);
    float maxX = (float)max(max(v0->x, v1->x), v2->x);
    float minY = (float)min(min(v0->y, v1->y), v2->y);
    float maxY = (float)max(max(v0->y, v1->y), v2->y);

    float orient = (maxX - minX) / (maxY - minY);

    if (orient > 0.4 && orient < 1.6)
        Rasterizer_drawTriangleBlockTemplate(rs, v0, v1, v2);
    else
        Rasterizer_drawTriangleSpanTemplate(rs, v0, v1, v2);
}

void Rasterizer_drawTriangleModeTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2)
{
    switch (rs->rasterMode)
    {
    case RM_Span:
        Rasterizer_drawTriangleSpanTemplate(rs, v0, v1, v2);
        break;
    case RM_Block:
        Rasterizer_drawTriangleBlockTemplate(rs, v0, v1, v2);
        break;
    case RM_Adaptive:
        Rasterizer_drawTriangleAdaptiveTemplate(rs, v0, v1, v2);
        break;
    }
}