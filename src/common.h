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
