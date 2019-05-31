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

#include "SDL.h"
#include "Renderer.h"

#include <stdlib.h>

SDL_Surface* surface;

typedef struct {
	float x, y, z;
	float r, g, b;
} VertexData;

static void processVertex(VertexShaderInput in, VertexShaderOutput *out)
{
	const VertexData *data = (const VertexData*)(in[0]);
	out->x = data->x;
	out->y = data->y;
	out->z = data->z;
	out->w = 1.0f;
	out->avar[0] = data->r;
	out->avar[1] = data->g;
	out->avar[2] = data->b;
}

static void drawPixel(const PixelData *p)
{
    int rint = (int)(p->avar[0] * 255);
    int gint = (int)(p->avar[1] * 255);
    int bint = (int)(p->avar[2] * 255);

    Uint32 color = rint << 16 | gint << 8 | bint;

    Uint32 *buffer = (Uint32*)((Uint8 *)surface->pixels + (int)p->y * surface->pitch + (int)p->x * 4);
    *buffer = color;
}

void drawTriangles(SDL_Surface *s)
{
    SoftwareRenderer_init();

    VertexShader *vshader = SoftwareRenderer_createVertexShader(3, processVertex);
    PixelShader *pshader = SoftwareRenderer_createPixelShader(false, false, 3, 0, drawPixel);

    Rasterizer *r = SoftwareRenderer_createRasterizer();
    Rasterizer_setScissorRect(r, 0, 0, 640, 480);
    Rasterizer_setPixelShader(r, pshader);

    surface = s;

    VertexProcessor *vp = SoftwareRenderer_createVertexProcessor(r);
	VertexProcessor_setViewport(vp, 100, 100, 640 - 200, 480 - 200);
    VertexProcessor_setCullMode(vp, CM_None);
    VertexProcessor_setVertexShader(vp, vshader);

	VertexData vdata[3];

	vdata[0].x = 0.0f;
	vdata[0].y = 0.5f;
	vdata[0].z = 0.0f;
	vdata[0].r = 1.0f;
	vdata[0].g = 0.0f;
	vdata[0].b = 0.0f;

	vdata[1].x = -1.5f;
	vdata[1].y = -0.5f;
	vdata[1].z = 0.0f;
	vdata[1].r = 0.0f;
	vdata[1].g = 1.0f;
	vdata[1].b = 0.0f;
	
	vdata[2].x = 1.5f;
	vdata[2].y = -0.5f;
	vdata[2].z = 0.0f;
	vdata[2].r = 0.0f;
	vdata[2].g = 0.0f;
	vdata[2].b = 1.0f;

	int idata[3];
	idata[0] = 0;
	idata[1] = 1;
	idata[2] = 2;

	VertexProcessor_setVertexAttribPointer(vp, 0, sizeof(VertexData), vdata);
	VertexProcessor_drawElements(vp, DM_Triangle, 3, idata);

    SoftwareRenderer_destroy();
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow(
		"VertexProcessorTest",
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED,
		640, 
		480,
		0
	);

	SDL_Surface *screen = SDL_GetWindowSurface(window);
	srand(1234);

	drawTriangles(screen);

	SDL_UpdateWindowSurface(window);

	SDL_Event e;
	while (SDL_WaitEvent(&e) && e.type != SDL_QUIT);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}