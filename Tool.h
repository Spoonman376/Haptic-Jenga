//
//

#ifndef Tool_h
#define Tool_h

#include <stdio.h>
#include "PhysXUtilities.h"

class Tool
{
protected:
  cGenericObject* root;
  
  // Might want to ensure that these vectors are the same length
  vector<cShapeSphere*> tools;
  vector<PxRigidDynamic*> physXTools;
  
public:
  
  Tool();
  ~Tool();
  
  const double mass = 0.1;
  
  void addToWorld(cWorld*);
  void addReferenceFrames();
  
  void addActor(PxRigidDynamic*);
  
  // These set the postions of the
  void setPosition(cVector3d pos);
  void setRotation(cMatrix3d rot);
  
  virtual void update();
  
  void enableInteraction(bool);

  // Applies a the force to all the physxtools
  void applyForce(cVector3d force);
};

#endif /* Tool_h */
