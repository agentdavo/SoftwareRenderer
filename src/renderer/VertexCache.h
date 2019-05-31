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

enum { VertexCacheSize = 16 };
typedef struct VertexCache {
	int inputIndex[VertexCacheSize];
	int outputIndex[VertexCacheSize];
} VertexCache;

static inline void VertexCache_clear(VertexCache *vc)
{
    for (int i = 0; i < VertexCacheSize; i++)
        vc->inputIndex[i] = -1;
}

static inline void VertexCache_construct(VertexCache *vc)
{
    VertexCache_clear(vc);
}

static inline void VertexCache_set(VertexCache *vc, int inIndex, int outIndex)
{
    int cacheIndex = inIndex % VertexCacheSize;
    vc->inputIndex[cacheIndex] = inIndex;
    vc->outputIndex[cacheIndex] = outIndex;
}

static inline int VertexCache_lookup(VertexCache *vc, int inIndex)
{
    int cacheIndex = inIndex % VertexCacheSize;
    if (vc->inputIndex[cacheIndex] == inIndex)
        return vc->outputIndex[cacheIndex];
    else
        return -1;
}
