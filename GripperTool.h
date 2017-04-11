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
  vector<PxD6Joint*> joints;

  bool gripMode = false;
  double maxdistance = 0.3;
  double mindistance = 0.05;
  double distance = 0.05;
  double rotation = 0.0;
  
  GripperTool(cGenericHapticDevicePtr, double s);
  ~GripperTool();

  void update();
  void enableInteraction(bool);
  
  // Applies a the force to all the physxtools
  //void applyForceToDevice();
  void applyForce();
};


#endif /* GripperTool_h */
