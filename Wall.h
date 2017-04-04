//
//

#ifndef Wall_h
#define Wall_h

#include <stdio.h>
#include "chai3d.h"
#include "PhysxUtilities.h"

using namespace chai3d;
using namespace std;

class Wall
{
private:
  cMesh *wall;
  double width;
  double height;
  cVector3d normal;
  
  PxRigidStatic* physXWall;
  
public:
  Wall(double width, double height, cVector3d pos, cVector3d axis, double angle, string texFile);
  ~Wall();
  
  cVector3d getPosition();
  cMatrix3d getRotation();
  
  cVector3d getNormal();
  double getWidth();
  double getHeight();
  
  void addToWorld(cWorld* world);
  void setActor(PxRigidStatic*);
};


#endif /* Wall_h */
