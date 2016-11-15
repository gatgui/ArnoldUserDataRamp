#include "common.h"
#include <algorithm>

namespace SSTR
{
   AtString positions("positions");
   AtString values("values");
   AtString interpolations("interpolations");
   AtString default_interpolation("default_interpolation");
   AtString abort_on_error("abort_on_error");
   AtString linkable("linkable");
   AtString sort_positions("sort_positions");
}

// ---

static void GetArrayElement(AtArray *a, unsigned int i, float &e)
{
   e = AiArrayGetFlt(a, i);
}

static void GetArrayElement(AtArray *a, unsigned int i, AtVector &e)
{
   e = AiArrayGetVec(a, i);
}

static void GetArrayElement(AtArray *a, unsigned int i, AtRGB &e)
{
   e = AiArrayGetRGB(a, i);
}

template <typename T, typename S>
static void EvalRampT(AtArray *p, AtArray *v, AtArray *it, InterpolationType defi, const S &shuffle, float t, T &result)
{
   unsigned int inext = p->nelements;

   for (unsigned int i = 0; (i < p->nelements); ++i)
   {
      if (t < AiArrayGetFlt(p, shuffle[i]))
      {
         inext = i;
         break;
      }
   }

   if (inext >= p->nelements)
   {
      GetArrayElement(v, shuffle[p->nelements - 1], result);
      return;
   }

   int iprev = inext - 1;
   
   if (iprev < 0)
   {
      GetArrayElement(v, shuffle[0], result);
      return;
   }

   float p1 = AiArrayGetFlt(p, shuffle[iprev]);
   float p2 = AiArrayGetFlt(p, shuffle[inext]);

   T v1, v2;
   GetArrayElement(v, shuffle[iprev], v1);
   GetArrayElement(v, shuffle[inext], v2);

   float dp = p2 - p1;
   float u = (t - p1) / dp;

   int itype = defi;
   if (it)
   {
      itype = AiArrayGetInt(it, shuffle[iprev]);
      if (itype < Interp_None || itype > Interp_Spline)
      {
         itype = defi;
      }
   }

   switch (itype)
   {
   case Interp_None:
      result = v1;
      break;

   case Interp_Linear:
      result = v1 + u * (v2 - v1);
      break;

   case Interp_Smooth:
      {
         float u2 = u * u;
         float u3 = u * u2;
         float a =  2.0f * u3 - 3.0f * u2 + 1.0f;
         float b = -2.0f * u3 + 3.0f * u2 ;
         result = a * v1 + b * v2;
      }
      break;

   case Interp_Spline:
      {
         T t1, t2;

         float u2 = u * u;
         float u3 = u * u2;

         float a =  2.0f * u3 - 3.0f * u2 + 1.0f; // p1
         float b = -2.0f * u3 + 3.0f * u2; // p2
         float c = u3 - 2.0f * u2 + u; // t1
         float d = u3 - u2; // t2
         
         // Catmull-rom type tangents:
         float p0, p3, sx;
         T v0, v3, sy;

         // Compute start tangent
         if (iprev == 0)
         {
            p0 = p1 - dp;
            v0 = v1;
         }
         else
         {
            p0 = AiArrayGetFlt(p, shuffle[iprev-1]);
            GetArrayElement(v, shuffle[iprev-1], v0);
         }
         sx = p2 - p0;
         if (sx < AI_EPSILON)
         {
            sx = AI_EPSILON;
         }
         sy = v2 - v0;
         t1 = sy / sx;
         
         // Compute end tangent
         if (inext+1 >= p->nelements)
         {
            p3 = p2 + dp;
            v3 = v2;
         }
         else
         {
            p3 = AiArrayGetFlt(p, shuffle[inext+1]);
            GetArrayElement(v, shuffle[inext+1], v3);
         }
         sx = p3 - p1;
         if (sx < AI_EPSILON)
         {
            sx = AI_EPSILON;
         }
         sy = v3 - v1;
         t2 = sy / sx;
         
         t1 *= dp;
         t2 *= dp;

         result = (a * v1 + b * v2 + c * t1 + d * t2);
      }
      break;

   default:
      result = v1;
   }
}

// ---

const char* InterpolationTypeNames[] =
{
   "None",
   "Linear",
   "Smooth",
   "Spline",
   NULL
};

class ComparePositions
{
public:
   
   inline ComparePositions(AtArray *a)
      : _a(a)
   {
   }
   
   inline bool operator()(unsigned int i0, unsigned int i1) const
   {
      return (AiArrayGetFlt(_a, i0) < AiArrayGetFlt(_a, i1));
   }
   
private:
   
   AtArray *_a;
};

void SortPositions(AtArray *a, unsigned int *shuffle)
{
   if (a && shuffle && a->nelements > 0)
   {
      unsigned int n = a->nelements;
      
      for (unsigned int i=0; i<n; ++i)
      {
         shuffle[i] = i;
      }
      
      ComparePositions cmp(a);
      
      std::sort(shuffle, shuffle + n, cmp);
   }
}

void EvalFloatRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, unsigned int *s, float t, float &out)
{
   EvalRampT(p, v, i, defi, s, t, out);
}

void EvalVectorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, unsigned int *s, float t, AtVector &out)
{
   EvalRampT(p, v, i, defi, s, t, out);
}

void EvalColorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, unsigned int *s, float t, AtRGB &out)
{
   EvalRampT(p, v, i, defi, s, t, out);
}

struct IdentitySort
{
   inline unsigned int operator[](unsigned int i) const
   {
      return i;
   }
   
   inline unsigned int operator[](int i) const
   {
      return (unsigned int)i;
   }
};

void EvalFloatRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, float t, float &out)
{
   IdentitySort s;
   EvalRampT(p, v, i, defi, s, t, out);
}

void EvalVectorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, float t, AtVector &out)
{
   IdentitySort s;
   EvalRampT(p, v, i, defi, s, t, out);
}

void EvalColorRamp(AtArray *p, AtArray *v, AtArray *i, InterpolationType defi, float t, AtRGB &out)
{
   IdentitySort s;
   EvalRampT(p, v, i, defi, s, t, out);
}
