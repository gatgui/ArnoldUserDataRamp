#include "common.h"

extern AtNodeMethods* UserDataRampFMtd;
extern AtNodeMethods* UserDataRampVMtd;
extern AtNodeMethods* UserDataRampC3Mtd;

node_loader
{
   if (i == 0)
   {
      node->name = PREFIX "user_data_ramp_v";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = UserDataRampVMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == 1)
   {
      node->name = PREFIX "user_data_ramp_f";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = UserDataRampFMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == 2)
   {
      node->name = PREFIX "user_data_ramp_c3";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = UserDataRampC3Mtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else
   {
      return false;
   }
}
