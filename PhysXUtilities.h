//
//

#ifndef PhysxUtilities_h
#define PhysxUtilities_h

#include <stdio.h>
#include <map>
#include "chai3d.h"
#include "PxScene.h"
#include "PxPhysics.h"
#include "PxPhysicsAPI.h"

using namespace physx;
using namespace std;
using namespace chai3d;

enum filter
{
  BLOCK     = (1 << 0),
  CURSOR    = (1 << 1),
  PLANE     = (1 << 2),
  UNTOUCHABLE = (1 << 3),
};


cTransform convertMatrix(PxTransform trans);
PxTransform convertMatrix(cTransform trans);

#endif /* PhysxUtilities_h */
