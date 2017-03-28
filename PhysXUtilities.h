//
//

#ifndef PhysxUtilities_h
#define PhysxUtilities_h

#include <stdio.h>
#include "chai3d.h"
#include "PxScene.h"
#include "PxPhysics.h"
#include "PxPhysicsAPI.h"

using namespace physx;
using namespace std;
using namespace chai3d;


cTransform convertMatrix(PxTransform trans);

#endif /* PhysxUtilities_h */
