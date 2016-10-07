#include "agUserDataRampCommon.h"

AI_SHADER_NODE_EXPORT_METHODS(agUserDataColorRampMtd);

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

namespace SSTR
{
   extern AtString positions;
   extern AtString values;
   extern AtString interpolations;
   extern AtString default_interpolation;
   extern AtString abort_on_error;
   extern AtString linkable;
}

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterStr(SSTR::positions, "");
   AiParameterStr(SSTR::values, "");
   AiParameterStr(SSTR::interpolations, "");
   AiParameterEnum(SSTR::default_interpolation, 1, InterpolationTypeNames);
   AiParameterRGB("default_value", 1.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::abort_on_error, false);
}

struct NodeData
{
   AtString positions;
   AtString values;
   AtString interpolations;
   InterpolationType defaultInterpolation;
   bool abortOnError;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->positions = AiNodeGetStr(node, SSTR::positions);
   data->values = AiNodeGetStr(node, SSTR::values);
   data->interpolations = AiNodeGetStr(node, SSTR::interpolations);
   data->defaultInterpolation = (InterpolationType) AiNodeGetInt(node, SSTR::default_interpolation);
   data->abortOnError = AiNodeGetBool(node, SSTR::abort_on_error);
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
}

static void Failed(AtShaderGlobals *sg, AtNode *node, const char *errMsg, bool abortOnError)
{
   if (abortOnError)
   {
      if (errMsg)
      {
         AiMsgError("[userDataColorRamp] %s", errMsg);
      }
      else
      {
         AiMsgError("[userDataColorRamp] Failed");
      }
   }
   else
   {
      if (errMsg)
      {
         AiMsgWarning("[userDataColorRamp] %s. Use default value", errMsg);
      }
      else
      {
         AiMsgWarning("[userDataColorRamp] Failed. Use default value");
      }
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
   }
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   AtArray *p, *v, *i=0;
   
   if (!AiUDataGetArray(data->positions, &p))
   {
      Failed(sg, node, "Positions user attribute not found or not an array", data->abortOnError);
      return;
   }
   
   if (p->type != AI_TYPE_FLOAT)
   {
      Failed(sg, node, "Positions user attribute must be an array of floats", data->abortOnError);
      return;
   }
   
   if (!AiUDataGetArray(data->values, &v))
   {
      Failed(sg, node, "Values user attribute not found or not an array", data->abortOnError);
      return;
   }
   
   if (v->type != AI_TYPE_RGB)
   {
      Failed(sg, node, "Values user attribute must be an array of colors", data->abortOnError);
      return;
   }
   
   if (v->nelements != p->nelements)
   {
      Failed(sg, node, "Positions and values array size mismatch", data->abortOnError);
      return;
   }
   
   if (AiUDataGetArray(data->interpolations, &i))
   {
      if (i->type != AI_TYPE_ENUM && i->type != AI_TYPE_INT)
      {
         Failed(sg, node, "Interpolations user attribute must be an array of enums or ints", data->abortOnError);
         return;
      }
      
      if (i->nelements != p->nelements)
      {
         Failed(sg, node, "Positions and interpolations array size mismatch", data->abortOnError);
         return;
      }
   }
   
   unsigned int *s = (unsigned int*) AiShaderGlobalsQuickAlloc(sg, p->nelements * sizeof(unsigned int));
   
   SortPositions(p, s);
   
   EvalColorRamp(p, v, i, data->defaultInterpolation, s, AiShaderEvalParamFlt(p_input), sg->out.RGB);
}
