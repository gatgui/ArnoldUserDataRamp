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

#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UserDataRampC3Mtd);

enum UserDataRampC3Params
{
   p_input = 0,
   p_positions,
   p_sort_positions,
   p_values,
   p_interpolations,
   p_default_interpolation,
   p_default_value,
   p_abort_on_error
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterStr(SSTR::positions, "");
   AiParameterBool(SSTR::sort_positions, false);
   AiParameterStr(SSTR::values, "");
   AiParameterStr(SSTR::interpolations, "");
   AiParameterEnum(SSTR::default_interpolation, 1, InterpolationTypeNames);
   AiParameterRGB("default_value", 1.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::abort_on_error, false);
}

struct UserDataRampC3Data
{
   AtString positions;
   AtString values;
   AtString interpolations;
   InterpolationType defaultInterpolation;
   bool abortOnError;
   bool sortPositions;
};

node_initialize
{
   AiNodeSetLocalData(node, new UserDataRampC3Data());
}

node_update
{
   UserDataRampC3Data *data = (UserDataRampC3Data*) AiNodeGetLocalData(node);
   
   data->positions = AiNodeGetStr(node, SSTR::positions);
   data->values = AiNodeGetStr(node, SSTR::values);
   data->interpolations = AiNodeGetStr(node, SSTR::interpolations);
   data->defaultInterpolation = (InterpolationType) AiNodeGetInt(node, SSTR::default_interpolation);
   data->abortOnError = AiNodeGetBool(node, SSTR::abort_on_error);
   data->sortPositions = AiNodeGetBool(node, SSTR::sort_positions);
}

node_finish
{
   UserDataRampC3Data *data = (UserDataRampC3Data*) AiNodeGetLocalData(node);
   delete data;
}

static void Failed(AtShaderGlobals *sg, AtNode *node, const char *errMsg, bool abortOnError)
{
   if (abortOnError)
   {
      if (errMsg)
      {
         AiMsgError("[%suser_data_ramp_c3] %s", PREFIX, errMsg);
      }
      else
      {
         AiMsgError("[%suser_data_ramp_c3] Failed", PREFIX);
      }
   }
   else
   {
      if (errMsg)
      {
         AiMsgWarning("[%suser_data_ramp_c3] %s. Use default value", PREFIX, errMsg);
      }
      else
      {
         AiMsgWarning("[%suser_data_ramp_c3] Failed. Use default value", PREFIX);
      }
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
   }
}

shader_evaluate
{
   UserDataRampC3Data *data = (UserDataRampC3Data*) AiNodeGetLocalData(node);

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
   
   if (data->sortPositions)
   {
      unsigned int *s = (unsigned int*) AiShaderGlobalsQuickAlloc(sg, p->nelements * sizeof(unsigned int));
      SortPositions(p, s);
      EvalColorRamp(p, v, i, data->defaultInterpolation, s, AiShaderEvalParamFlt(p_input), sg->out.RGB);
   }
   else
   {
      EvalColorRamp(p, v, i, data->defaultInterpolation, AiShaderEvalParamFlt(p_input), sg->out.RGB);
   }
}
