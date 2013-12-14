#ifndef __agUserDataRamp_h__
#define __agUserDataRamp_h__

#include <ai.h>

enum InterpolationType
{
   Interp_None = 0,
   Interp_Linear,
   Interp_Smooth,
   Interp_Spline
};

extern const char* InterpolationTypeNames[];

bool SortPositions(AtArray *a, unsigned int *shuffle);

void EvalFloatRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, unsigned int *s, float t, float &out);
void EvalVectorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, unsigned int *s, float t, AtVector &out);
void EvalColorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, unsigned int *s, float t, AtRGB &out);

#endif
