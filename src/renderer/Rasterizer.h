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
#include "PixelShader.h"

#include <stdbool.h>


/// Rasterizer main class.
typedef struct Rasterizer_s
{
	int m_minX;
	int m_maxX;
	int m_minY;
	int m_maxY;

	RasterMode rasterMode;

    PixelShader *m_pixelShader;

	void (*m_triangleFunc)(struct Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2);
	void (*m_lineFunc)(struct Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1);
	void (*m_pointFunc)(struct Rasterizer *rs, const RasterizerVertex *v);
} Rasterizer;

/// Constructor.
void Rasterizer_construct(Rasterizer *rs);
/// Set the raster mode. The default is RasterMode::Span.
void Rasterizer_setRasterMode(Rasterizer *rs, RasterMode mode);
/// Set the scissor rectangle.
void Rasterizer_setScissorRect(Rasterizer *rs, int x, int y, int width, int height);
/// Set the pixel shader.
void Rasterizer_setPixelShader(Rasterizer *rs, PixelShader *ps);
/// Draw a single point.
void Rasterizer_drawPoint(Rasterizer *rs, const RasterizerVertex *v);
/// Draw a single line.
void Rasterizer_drawLine(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1);
/// Draw a single triangle.
void Rasterizer_drawTriangle(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2);
void Rasterizer_drawPointList(Rasterizer *rs, const RasterizerVertex *vertices, const int *indices, unsigned long indexCount);
void Rasterizer_drawLineList(Rasterizer *rs, const RasterizerVertex *vertices, const int *indices, unsigned long indexCount);
void Rasterizer_drawTriangleList(Rasterizer *rs, const RasterizerVertex *vertices, const int *indices, unsigned long indexCount);
bool Rasterizer_scissorTest(Rasterizer *rs, float x, float y);
void Rasterizer_drawPointTemplate(Rasterizer *rs, const RasterizerVertex *v);
PixelData Rasterizer_pixelDataFromVertex(Rasterizer *rs, const RasterizerVertex *v);
void Rasterizer_drawLineTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1);
void Rasterizer_stepVertex(Rasterizer *rs, RasterizerVertex *v, RasterizerVertex *step);
RasterizerVertex Rasterizer_computeVertexStep(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, int adx);
void Rasterizer_drawTriangleBlockTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2);
void Rasterizer_drawTriangleSpanTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2);
void Rasterizer_drawBottomFlatTriangle(Rasterizer *rs, const TriangleEquations *eqn, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2);
void Rasterizer_drawTopFlatTriangle(Rasterizer *rs, const TriangleEquations *eqn, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2);
void Rasterizer_drawTriangleAdaptiveTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2);
void Rasterizer_drawTriangleModeTemplate(Rasterizer *rs, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2);