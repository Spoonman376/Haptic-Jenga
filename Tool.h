//
//

#ifndef Tool_h
#define Tool_h

#include <stdio.h>
#include "PhysXUtilities.h"

class Tool
{
protected:
  cMesh* root;
  PxRigidDynamic* physXRoot;
  
  cGenericHapticDevicePtr device;
  
  // Might want to ensure that these vectors are the same length
  vector<cMesh*> tools;
  vector<PxRigidDynamic*> physXTools;
  
public:
  
  Tool(cGenericHapticDevicePtr, double scale);
  ~Tool();
  
  double mass;
  double springConstant;
  double scale;

  void addToWorld(cWorld*);
  void addReferenceFrames();
  
  void addActor(PxRigidDynamic*);
  void setRootActor(PxRigidDynamic*);
  
  // These set the postions of the
  void setPosition(cVector3d pos);
  void setRotation(cMatrix3d rot);
  
  virtual void update();
  virtual void enableInteraction(bool);
  
  cMesh* getMesh();

  // Applies a the force to all the physxtools
  void applyForce(cVector3d force);
};

#endif /* Tool_h */
