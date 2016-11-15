/*
MIT License

Copyright (c) 2016 Gaetan Guidet

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

#ifndef __userdataramp_common_h__
#define __userdataramp_common_h__

#include <ai.h>
#include <cstring>

#ifndef PREFIX
#  define PREFIX ""
#endif

namespace SSTR
{
   extern AtString positions;
   extern AtString values;
   extern AtString interpolations;
   extern AtString default_interpolation;
   extern AtString abort_on_error;
   extern AtString linkable;
   extern AtString sort_positions;
}

enum InterpolationType
{
   Interp_None = 0,
   Interp_Linear,
   Interp_Smooth,
   Interp_Spline
};

extern const char* InterpolationTypeNames[];

void SortPositions(AtArray *a, unsigned int *shuffle);

void EvalFloatRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, float t, float &out);
void EvalFloatRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, unsigned int *s, float t, float &out);

void EvalVectorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, float t, AtVector &out);
void EvalVectorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, unsigned int *s, float t, AtVector &out);

void EvalColorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, float t, AtRGB &out);
void EvalColorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, unsigned int *s, float t, AtRGB &out);

#endif
