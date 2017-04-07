//
//

#include "GripperTool.h"


GripperTool::GripperTool(cGenericHapticDevicePtr d, double s) : Tool(d, s)
{
  
}

GripperTool::~GripperTool()
{
  
}

void GripperTool::grip()
{
  for (PxRigidDynamic* pxTool : physXTools)
  {    
    pxTool->addForce(PxVec3());
  }
}
