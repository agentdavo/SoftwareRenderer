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

#include "LineClipper.h"

#include <math.h>

void LineClipper_clipToPlane(LineClipper *lc, float a, float b, float c, float d)
{
	if (lc->fullyClipped)
		return;

	float dp0 = a * lc->m_v0->x + b * lc->m_v0->y + c * lc->m_v0->z + d * lc->m_v0->w;
	float dp1 = a * lc->m_v1->x + b * lc->m_v1->y + c * lc->m_v1->z + d * lc->m_v1->w;

	bool dp0neg = dp0 < 0;
	bool dp1neg = dp1 < 0;

	if (dp0neg && dp1neg) {
        lc->fullyClipped = true;
		return;
	} 
	
	if (dp0neg)
	{
		float t = -dp0 / (dp1 - dp0);
        lc->t0 = fmaxf(lc->t0, t);
	}
	else
	{
		float t = dp0 / (dp0 - dp1);
        lc->t1 = fminf(lc->t1, t);
	}
}
