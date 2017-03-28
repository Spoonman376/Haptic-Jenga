//
//

#ifndef SphereTool_h
#define SphereTool_h

#include <stdio.h>
#include "chai3d.h"
#include "PhysXUtilities.h"

using namespace chai3d;

class SphereTool
{
private:
  cShapeSphere *sphere;
  
  // PhysX Object
  PxRigidDynamic* physXSphere;

  
public:
  SphereTool(double r = 0.005);
  ~SphereTool();
  
  double radius = 0.005;
  
  cVector3d getPosition();
  void addToWorld(cWorld* world);
  void setActor(PxRigidDynamic* s);
  void update();
  void setUp();
  
  void applyForce(cVector3d force);
};

#endif /* SphereTool_h */
