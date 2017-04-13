//
//

#ifndef SphereTool_h
#define SphereTool_h

#include <stdio.h>
#include "chai3d.h"
#include "PhysXUtilities.h"
#include "Tool.h"

using namespace chai3d;

class SphereTool : public Tool
{
private:
  
public:
  SphereTool(cGenericHapticDevicePtr d, double scale);
  ~SphereTool();
  
  double radius;
  
  void update();
  void setUp();
  
  void enableInteraction(bool);
  void applyForce(cCamera*);
};

#endif /* SphereTool_h */
