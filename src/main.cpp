#include <ai.h>
#include <cstring>

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
