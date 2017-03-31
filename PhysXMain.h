//
//

#ifndef PhysXMain_h
#define PhysXMain_h

#include <stdio.h>
#include "PhysxUtilities.h"

#include "JengaFilterShader.h"

#include "Block.h"
#include "SphereTool.h"
#include "Wall.h"



class PhysXMain
{
private:
  map<PxRigidActor*, Block*> objectMap;
  map<PxRigidActor*, SphereTool*> toolMap;
  
  PxDefaultAllocator		      gAllocator;
  PxDefaultErrorCallback    	gErrorCallback;
  PxFoundation*			          gFoundation = NULL;
  PxPhysics*				          gPhysics = NULL;
  PxDefaultCpuDispatcher*   	gDispatcher = NULL;
  PxScene*				            gScene = NULL;
  PxMaterial*				          gMaterial = NULL;
  PxBatchQuery*			          gBatchQuery = NULL;
  
  
public:
  PhysXMain();
  ~PhysXMain();
  
  const double gravity = -9.81 * 1.0;
  
  void collisionFunction(PxContactModifyPair* const, PxU32);
  
  void initBlock(Block* b);
  void initSphere(SphereTool* s);
  void initWall(Wall* w);
  
  void stepPhysics(double t);
  void cleanupPhysics(bool interactive);
  
  cMatrix3d convertMat(PxVec3 x, PxVec3 y, PxVec3 z, PxVec3 w);
};



#endif /* PhysXMain_h */
