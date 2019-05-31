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
#include "Rasterizer.h"
#include "Renderer.h"
#include "LineClipper.h"
#include "PolyClipper.h"
#include "VertexShader.h"
#include "VertexCache.h"
#include "Vector.h"

enum {
    ClipMask_PosX = 0x01,
    ClipMask_NegX = 0x02,
    ClipMask_PosY = 0x04,
    ClipMask_NegY = 0x08,
    ClipMask_PosZ = 0x10,
    ClipMask_NegZ = 0x20
};

/// Process vertices and pass them to a rasterizer.
typedef struct VertexProcessor_s {

	struct {
		int x, y, width, height;
		float px, py, ox, oy;
	} m_viewport;

	struct {
		float n, f;
	} m_depthRange;

	CullMode m_cullMode;
	Rasterizer *m_rasterizer;

    VertexShader *m_vertexShader;
	
	void (*m_processVertexFunc)(VertexShaderInput, VertexShaderOutput*);
	int m_attribCount;

	struct Attribute {
		const void *buffer;
		int stride;
	} m_attributes[MaxVertexAttribs];

	// Some temporary variables for speed
	PolyClipper m_polyClipper;

	//std::vector<VertexShaderOutput> m_verticesOut;
    Vector m_verticesOut;

	//std::vector<int> m_indicesOut;
    Vector m_indicesOut;

	//std::vector<int> m_clipMask;
    Vector m_clipMask;

	//std::vector<bool> m_alreadyProcessed;
    Vector m_alreadyProcessed;
} VertexProcessor;

/// Constructor.
void VertexProcessor_construct(VertexProcessor *vp, Rasterizer *rasterizer);

/// Destructor
void VertexProcessor_destruct(VertexProcessor *vp);

/// Change the rasterizer where the primitives are sent.
void VertexProcessor_setRasterizer(VertexProcessor *vp, Rasterizer *rasterizer);

/// Set the viewport.
/** Top-Left is (0, 0) */
void VertexProcessor_setViewport(VertexProcessor *vp, int x, int y, int width, int height);

/// Set the depth range.
/** Default is (0, 1) */
void VertexProcessor_setDepthRange(VertexProcessor *vp, float n, float f);

/// Set the cull mode.
/** Default is CullMode::CW to cull clockwise triangles. */
void VertexProcessor_setCullMode(VertexProcessor *vp, CullMode mode);

/// Set the vertex shader.
void VertexProcessor_setVertexShader(VertexProcessor *vp, VertexShader *vs);

/// Set a vertex attrib pointer.
void VertexProcessor_setVertexAttribPointer(VertexProcessor *vp, int index, int stride, const void *buffer);

/// Draw a number of points, lines or triangles.
void VertexProcessor_drawElements(VertexProcessor *vp, DrawMode mode, unsigned long count, int *indices);

int VertexProcessor_clipMask(VertexShaderOutput *v);
const void *VertexProcessor_attribPointer(VertexProcessor *vp, int attribIndex, int elementIndex);
void VertexProcessor_processVertex(VertexProcessor *vp, VertexShaderInput in, VertexShaderOutput *out);
void VertexProcessor_initVertexInput(VertexProcessor *vp, VertexShaderInput in, int index);

void VertexProcessor_clipPoints(VertexProcessor *vp);
void VertexProcessor_clipLines(VertexProcessor *vp);
void VertexProcessor_clipTriangles(VertexProcessor *vp);

void VertexProcessor_clipPrimitives(VertexProcessor *vp, DrawMode mode);
void VertexProcessor_processPrimitives(VertexProcessor *vp, DrawMode mode);
int VertexProcessor_primitiveCount(VertexProcessor *vp, DrawMode mode);

void VertexProcessor_drawPrimitives(VertexProcessor *vp, DrawMode mode);
void VertexProcessor_cullTriangles(VertexProcessor *vp);
void VertexProcessor_transformVertices(VertexProcessor *vp);
