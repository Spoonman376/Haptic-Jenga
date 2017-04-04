//
//

#include "GripperTool.h"


GripperTool::GripperTool()
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
