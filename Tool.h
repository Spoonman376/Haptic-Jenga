//
//

#ifndef Tool_h
#define Tool_h

#include <stdio.h>
#include "PhysXUtilities.h"
#include "Spring.h"

class Tool
{
protected:
  cGenericHapticDevicePtr device;
  cVector3d fixPosition(cVector3d pos, cCamera* camera, double scale);
  
public:
  cMesh* root;
  PxRigidDynamic* physXRoot;

//  vector<cMesh*> tools;
//  vector<PxRigidDynamic*> physXTools;
//  vector<Spring*> springs;
  
  Tool(cGenericHapticDevicePtr, double scale);
  ~Tool();
  
  double mass;
  double springConstant;
  double scale;

  void addToWorld(cWorld*);
  void addReferenceFrames();
  
  void addActor(PxRigidDynamic*);
  void setRootActor(PxRigidDynamic*);
  
  cVector3d getPosition();
  
  // These set the postions of the
  void setPosition(cVector3d pos);
  void setRotation(cMatrix3d rot);
  
  virtual void update();
  virtual void enableInteraction(bool);
  
  // Applies a the force to all the physxtools
  virtual void applyForceToDevice(cCamera*);
  virtual void applyForce(cCamera*);
};

#endif /* Tool_h */
