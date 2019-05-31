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
#include "Rasterizer.h"
#include "VertexProcessor.h"
#include "Vector.h"

#include <stdlib.h>

static Vector g_object_ptrs;
// We need to store VertexProcessor pointers separately as they
// require a call to the "destruct" method as well
static Vector g_vertex_processor_ptrs;

void SoftwareRenderer_init()
{
    Vector_init(&g_object_ptrs, sizeof(void*));
    Vector_init(&g_vertex_processor_ptrs, sizeof(void*));
}

void SoftwareRenderer_destroy()
{
    // Call destructors for all vertex processor objects
    for (int i = 0; i < Vector_size(&g_vertex_processor_ptrs); i++)
    {
        void *ptr = Vector_element(&g_vertex_processor_ptrs, i, void*);
        VertexProcessor_destruct(ptr);
    }

    // Free memory for all allocated objects
    for (int i = 0; i < Vector_size(&g_object_ptrs); i++)
    {
        void *ptr = Vector_element(&g_object_ptrs, i, void*);
        free(ptr);
    }
}

VertexProcessor* SoftwareRenderer_createVertexProcessor(Rasterizer *r)
{
    VertexProcessor *ptr = malloc(sizeof(VertexProcessor));
    VertexProcessor_construct(ptr, r);
    Vector_append(&g_object_ptrs, ptr, void*);
    Vector_append(&g_vertex_processor_ptrs, ptr, void*);
    return ptr;
}

Rasterizer* SoftwareRenderer_createRasterizer()
{
    Rasterizer *ptr = malloc(sizeof(Rasterizer));
    Rasterizer_construct(ptr);
    Vector_append(&g_object_ptrs, ptr, void*);
    return ptr;
}

VertexShader* SoftwareRenderer_createVertexShader(int attribCount, ProcessVertexCallback callback)
{
    VertexShader *ptr = malloc(sizeof(VertexShader));
    *ptr = VertexShader_default;
    VertexShader_init(ptr, attribCount, callback);
    Vector_append(&g_object_ptrs, ptr, void*);
    return ptr;
}

PixelShader* SoftwareRenderer_createPixelShader(bool interpZ, bool interpW, int affineCount, int perspCount, DrawPixelCallback callback)
{
    PixelShader *ptr = malloc(sizeof(PixelShader));
    *ptr = PixelShader_default;
    PixelShader_init(ptr, interpZ, interpW, affineCount, perspCount, callback);
    Vector_append(&g_object_ptrs, ptr, void*);
    return ptr;
}