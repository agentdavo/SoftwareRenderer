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

#include "PixelShader.h"
#include "EdgeData.h"

void PixelShader_init(PixelShader *ps, int interpZ, int interpW, int affineVarCount, int perspVarCount, DrawPixelCallback callback)
{
    ps->InterpolateZ = interpZ;
    ps->InterpolateW = interpW;
    ps->AVarCount = affineVarCount;
    ps->PVarCount = perspVarCount;
    ps->drawPixel = callback;
}

void PixelShader_drawBlock(PixelShader *ps, const TriangleEquations *eqn, int x, int y, bool testEdges)
{
    float xf = x + 0.5f;
    float yf = y + 0.5f;

    PixelData po;
    PixelData_init(&po, eqn, xf, yf, ps->AVarCount, ps->PVarCount, ps->InterpolateZ, ps->InterpolateW);

    EdgeData eo;
    if (testEdges)
        EdgeData_init(&eo, eqn, xf, yf);

    for (int yy = y; yy < y + BlockSize; yy++)
    {
        PixelData pi = PixelShader_copyPixelData(ps, &po);

        EdgeData ei;
        if (testEdges)
            ei = eo;

        for (int xx = x; xx < x + BlockSize; xx++)
        {
            if (!testEdges || EdgeData_test(&ei, eqn))
            {
                pi.x = xx;
                pi.y = yy;
                if (ps->drawPixel)
                    ps->drawPixel(&pi);
            }

            PixelData_stepX(&pi, eqn, ps->AVarCount, ps->PVarCount, ps->InterpolateZ, ps->InterpolateW);
            if (testEdges)
                EdgeData_stepX(&ei, eqn);
        }

        PixelData_stepY(&po, eqn, ps->AVarCount, ps->PVarCount, ps->InterpolateZ, ps->InterpolateW);
        if (testEdges)
            EdgeData_stepY(&eo, eqn);
    }
}

void PixelShader_drawSpan(PixelShader *ps, const TriangleEquations *eqn, int x, int y, int x2)
{
    float xf = x + 0.5f;
    float yf = y + 0.5f;

    PixelData p;
    p.y = y;
    PixelData_init(&p, eqn, xf, yf, ps->AVarCount, ps->PVarCount, ps->InterpolateZ, ps->InterpolateW);

    while (x < x2)
    {
        p.x = x;
        if (ps->drawPixel)
            ps->drawPixel(&p);
        PixelData_stepX(&p, eqn, ps->AVarCount, ps->PVarCount, ps->InterpolateZ, ps->InterpolateW);
        x++;
    }
}

PixelData PixelShader_copyPixelData(PixelShader *ps, PixelData *po)
{
    PixelData pi;
    if (ps->InterpolateZ) pi.z = po->z;
    if (ps->InterpolateW) pi.invw = po->invw;
    for (int i = 0; i < ps->AVarCount; ++i)
        pi.avar[i] = po->avar[i];
    return pi;
}

