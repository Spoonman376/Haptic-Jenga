//
//

#ifndef JengaFilterShader_h
#define JengaFilterShader_h

#include <stdio.h>

#include "PxPhysics.h"
#include "PxPhysicsAPI.h"

using namespace physx;

PxFilterFlags JengaFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
 PxFilterObjectAttributes attributes1, PxFilterData filterData1,
 PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);


#endif /* JengaFilterShader_h */
