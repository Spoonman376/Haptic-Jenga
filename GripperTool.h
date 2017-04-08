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
  bool gripMode = false;
  double maxdistance = 1.0;
  double mindistance = 0.2;
  double distance = 1.0;
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
