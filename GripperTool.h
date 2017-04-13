//
//

#ifndef GripperTool_h
#define GripperTool_h

#include <stdio.h>
#include "PhysXUtilities.h"
#include "Tool.h"


class GripperTool : public Tool
{
private:
  
  
public:
  vector<cMesh*> tools;
  vector<PxRigidDynamic*> physXTools;
  vector<PxDistanceJoint*> joints;

  bool gripMode = false;
  double maxDistance = 0.04;
  double minDistance = 0.02;
  double distance = 0.05;
  double rotation = 0.0;
  
  GripperTool(cGenericHapticDevicePtr, double s);
  ~GripperTool();

  void update();
  void enableInteraction(bool);
  
  // Applies a the force to all the physxtools
  void applyForce(cCamera* camera);
};


#endif /* GripperTool_h */
