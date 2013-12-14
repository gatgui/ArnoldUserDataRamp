#include "agUserDataRamp.h"
#include <cstring>

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

template <typename T>
static void EvalRampT(AtArray *p, AtArray *v, AtArray *it, InterpolationType defi, unsigned int *shuffle, float t, T &result)
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
   "Spline"
};

bool SortPositions(AtArray *a, unsigned int *shuffle)
{
   bool modified = false;

   if (a && shuffle && a->nelements > 0)
   {
      float p0, p1;
      int tmp;

      bool swapped = true;
      unsigned int n = a->nelements;

      for (unsigned int i=0; i<n; ++i)
      {
         shuffle[i] = i;
      }

      while (swapped)
      {
         swapped = false;
         n -= 1;
         for (unsigned int i=0; i<n; ++i)
         {
            p0 = AiArrayGetFlt(a, shuffle[i]);
            p1 = AiArrayGetFlt(a, shuffle[i + 1]);
            if (p0 > p1)
            {
               swapped = true;
               modified = true;

               tmp = shuffle[i];
               shuffle[i] = shuffle[i + 1];
               shuffle[i + 1] = tmp;
            }
         }
      }
   }

   return modified;
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

// ---

extern AtNodeMethods* agUserDataFloatRampMtd;
extern AtNodeMethods* agUserDataVectorRampMtd;
extern AtNodeMethods* agUserDataColorRampMtd;

node_loader
{
   if (i == 0)
   {
      node->name = "userDataVectorRamp";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = agUserDataVectorRampMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == 1)
   {
      node->name = "userDataFloatRamp";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = agUserDataFloatRampMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == 2)
   {
      node->name = "userDataColorRamp";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = agUserDataColorRampMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else
   {
      return false;
   }
}
