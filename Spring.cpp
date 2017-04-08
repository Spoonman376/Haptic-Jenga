//
//

#include "Spring.h"


Spring::Spring(PxRigidDynamic* a, PxRigidDynamic* b, double k, double l)
{
  objectA = a;
  objectB = b;
  springConstant = k;
  maxRestLength = l;
  restLength = l;
  damping = 1.0;
}

Spring::~Spring()
{
  
}


void Spring::applyForce()
{
  PxVec3 a = objectA->getGlobalPose().p;
  PxVec3 b = objectA->getGlobalPose().p;
  
  PxVec3 forceD = a - b;
  double forceL = forceD.normalize() - restLength;
  
  PxVec3 force = -springConstant * forceD * forceL;
  
  objectA->addForce(force - damping * objectA->getLinearVelocity());
  objectB->addForce(-force - damping * objectB->getLinearVelocity());
}
