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
  SphereTool();
  ~SphereTool();
  
  const double radius = 0.003;
    
  cVector3d getPosition();
  void addToWorld(cWorld* world);
  void setActor(PxRigidDynamic* s);
  void update();
  void setUp();
  
  void enableInteraction();
  void disableInteraction();
  
  void setPosition(cVector3d pos);
  
  void applyForce(cVector3d force);
};

#endif /* SphereTool_h */
