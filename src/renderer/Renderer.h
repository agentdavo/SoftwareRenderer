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

#include <stdbool.h>

#ifdef __cplusplus
#define SR_API extern "C"
#else
#define SR_API
#endif

/// Primitive draw mode.
typedef enum {
    DM_Point,
    DM_Line,
    DM_Triangle
} DrawMode;

/// Triangle culling mode.
typedef enum {
    CM_None,
    CM_CCW,
    CM_CW
} CullMode;

/// Rasterizer mode.
typedef enum {
    RM_Span,
    RM_Block,
    RM_Adaptive
} RasterMode;

typedef struct VertexProcessor_s VertexProcessor;
typedef struct Rasterizer_s Rasterizer;
typedef struct VertexShader_s VertexShader;
typedef struct PixelShader_s PixelShader;

enum {
    BlockSize = 8,
    /// Maximum affine variables used for interpolation across the triangle.
    MaxAVars = 16,
    /// Maximum perspective variables used for interpolation across the triangle.
    MaxPVars = 16
};

/// Maximum supported number of vertex attributes.
enum { MaxVertexAttribs = 8 };

/// Vertex shader input is an array of vertex attribute pointers.
typedef const void *VertexShaderInput[MaxVertexAttribs];

/// Vertex shader output.
typedef struct {
    float x; ///< The x component.
    float y; ///< The y component.
    float z; ///< The z component.
    float w; ///< The w component.

    /// Affine variables.
    float avar[MaxAVars];

    /// Perspective variables.
    float pvar[MaxPVars];
} VertexShaderOutput;

typedef void (*ProcessVertexCallback)(VertexShaderInput in, VertexShaderOutput *out);

/// PixelData passed to the pixel shader for display.
typedef struct {
    int x; ///< The x coordinate.
    int y; ///< The y coordinate.

    float z; ///< The interpolated z value.
    float w; ///< The interpolated w value.
    float invw; ///< The interpolated 1 / w value.

    /// Affine variables.
    float avar[MaxAVars];

    /// Perspective variables.
    float pvar[MaxPVars];

    // Used internally.
    float pvarTemp[MaxPVars];
} PixelData;

typedef void (*DrawPixelCallback)(const PixelData*);

/// Vertex input structure for the Rasterizer. Output from the VertexProcessor.
typedef struct {
    float x; ///< The x component.
    float y; ///< The y component.
    float z; ///< The z component.
    float w; ///< The w component.

    /// Affine variables.
    float avar[MaxAVars];

    /// Perspective variables.
    float pvar[MaxPVars];
} RasterizerVertex;


SR_API void SoftwareRenderer_init();
SR_API void SoftwareRenderer_destroy();
SR_API VertexProcessor* SoftwareRenderer_createVertexProcessor(Rasterizer *r);
SR_API Rasterizer* SoftwareRenderer_createRasterizer();
SR_API VertexShader* SoftwareRenderer_createVertexShader(int attribCount, ProcessVertexCallback callback);
SR_API PixelShader* SoftwareRenderer_createPixelShader(bool interpZ, bool interpW, int affineCount, int perspCount, DrawPixelCallback callback);

/// Change the rasterizer where the primitives are sent.
SR_API void VertexProcessor_setRasterizer(VertexProcessor *vp, Rasterizer *rasterizer);

/// Set the viewport.
/** Top-Left is (0, 0) */
SR_API void VertexProcessor_setViewport(VertexProcessor *vp, int x, int y, int width, int height);

/// Set the depth range.
/** Default is (0, 1) */
SR_API void VertexProcessor_setDepthRange(VertexProcessor *vp, float n, float f);

/// Set the cull mode.
/** Default is CM_CW to cull clockwise triangles. */
SR_API void VertexProcessor_setCullMode(VertexProcessor *vp, CullMode mode);

/// Set the vertex shader.
SR_API void VertexProcessor_setVertexShader(VertexProcessor *vp, VertexShader *vs);

/// Set a vertex attrib pointer.
SR_API void VertexProcessor_setVertexAttribPointer(VertexProcessor *vp, int index, int stride, const void *buffer);

/// Draw a number of points, lines or triangles.
SR_API void VertexProcessor_drawElements(VertexProcessor *vp, DrawMode mode, unsigned long count, int *indices);

SR_API void Rasterizer_setRasterMode(Rasterizer *r, RasterMode mode);
SR_API void Rasterizer_setScissorRect(Rasterizer *r, int x, int y, int width, int height);
SR_API void Rasterizer_setPixelShader(Rasterizer *r, PixelShader *ps);
SR_API void Rasterizer_drawPoint(Rasterizer *r, const RasterizerVertex *v);
SR_API void Rasterizer_drawLine(Rasterizer *r, const RasterizerVertex *v0, const RasterizerVertex *v1);
SR_API void Rasterizer_drawTriangle(Rasterizer *r, const RasterizerVertex *v0, const RasterizerVertex *v1, const RasterizerVertex *v2);
