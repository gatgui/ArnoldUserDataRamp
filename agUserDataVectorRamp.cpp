#include "agUserDataRamp.h"

AI_SHADER_NODE_EXPORT_METHODS(agUserDataVectorRampMtd);

enum FloatRampParams
{
   p_input = 0,
   p_positions,
   p_values,
   p_interpolations,
   p_default_interpolation,
   p_default_value,
   p_abort_on_error
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterStr("positions", "");
   AiParameterStr("values", "");
   AiParameterStr("interpolations", "");
   AiParameterEnum("default_interpolation", 1, InterpolationTypeNames);
   AiParameterVec("default_value", 1.0f, 0.0f, 1.0f);
   AiParameterBool("abort_on_error", false);
};

node_initialize
{
}

node_update
{
}

node_finish
{
}

static void Failed(AtShaderGlobals *sg, AtNode *node, const char *errMsg)
{
   if (AiShaderEvalParamBool(p_abort_on_error))
   {
      if (errMsg)
      {
         AiMsgError("[userDataVectorRamp] %s", errMsg);
      }
      else
      {
         AiMsgError("[userDataVectorRamp] Failed");
      }
   }
   else
   {
      if (errMsg)
      {
         AiMsgWarning("[userDataVectorRamp] %s. Use default value", errMsg);
      }
      else
      {
         AiMsgWarning("[userDataVectorRamp] Failed. Use default value");
      }
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
   }
}

shader_evaluate
{
   const char *pname = AiShaderEvalParamStr(p_positions);
   const char *vname = AiShaderEvalParamStr(p_values);
   const char *iname = AiShaderEvalParamStr(p_interpolations);
   
   AtArray *p, *v, *i=0;
   
   if (!AiUDataGetArray(pname, &p))
   {
      Failed(sg, node, "Positions user attribute not found or not an array");
      return;
   }
   
   if (p->type != AI_TYPE_FLOAT)
   {
      Failed(sg, node, "Positions user attribute must be an array of floats");
      return;
   }
   
   if (!AiUDataGetArray(vname, &v))
   {
      Failed(sg, node, "Values user attribute not found or not an array");
      return;
   }
   
   if (v->type != AI_TYPE_VECTOR && v->type != AI_TYPE_POINT)
   {
      Failed(sg, node, "Values user attribute must be an array of vectors/points");
      return;
   }
   
   if (v->nelements != p->nelements)
   {
      Failed(sg, node, "Positions and values array size mismatch");
      return;
   }
   
   if (AiUDataGetArray(iname, &i))
   {
      if (i->type != AI_TYPE_ENUM && i->type != AI_TYPE_INT)
      {
         Failed(sg, node, "Interpolations user attribute must be an array of enums or ints");
         return;
      }
      
      if (i->nelements != p->nelements)
      {
         Failed(sg, node, "Positions and interpolations array size mismatch");
         return;
      }
   }
   
   InterpolationType defi = (InterpolationType) AiShaderEvalParamInt(p_default_interpolation);
   
   unsigned int *s = (unsigned int*) AiShaderGlobalsQuickAlloc(sg, p->nelements * sizeof(unsigned int));
   
   SortPositions(p, s);
   
   EvalVectorRamp(p, v, i, defi, s, AiShaderEvalParamFlt(p_input), sg->out.VEC);
}
