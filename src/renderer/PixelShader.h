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
#include "PixelData.h"

#include <stdbool.h>

/// Pixel shader base class.
/** Derive your own pixel shaders from this class and redefine the static
  variables to match your pixel shader requirements. */
typedef struct PixelShader_s {
	/// Tells the rasterizer to interpolate the z component.
	int InterpolateZ;

	/// Tells the rasterizer to interpolate the w component.
	int InterpolateW;

	/// Tells the rasterizer how many affine vars to interpolate.
	int AVarCount;

	/// Tells the rasterizer how many perspective vars to interpolate.
    int PVarCount;

    /// This performs the coloring and will be called for each pixel.
    DrawPixelCallback drawPixel;
} PixelShader;

static const PixelShader PixelShader_default = { false, false, 0, 0, 0/*NULL*/};

void PixelShader_init(PixelShader *ps, int interpZ, int interpW, int affineCount, int perspCount, DrawPixelCallback callback);

void PixelShader_drawBlock(PixelShader *ps, const TriangleEquations *eqn, int x, int y, bool testEdges);
void PixelShader_drawSpan(PixelShader *ps, const TriangleEquations *eqn, int x, int y, int x2);
/// This is called per pixel. 
/** Implement this in your derived class to display single pixels. */
PixelData PixelShader_copyPixelData(PixelShader *ps, PixelData *po);