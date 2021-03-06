//
//

#ifndef PhysXMain_h
#define PhysXMain_h

#include <stdio.h>

#include "PhysxUtilities.h"

#include "JengaFilterShader.h"

#include "Block.h"
#include "SphereTool.h"
#include "GripperTool.h"
#include "Wall.h"
#include "ContactModifyCallback.h"



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
  PxBatchQuery*			          gBatchQuery = NULL;
  PxCooking*                  gCooking = NULL;

  PxMaterial*				          boxMaterial = NULL;
  PxMaterial*				          planeMaterial = NULL;
  PxMaterial*				          cursorMaterial = NULL;
  
  PxConvexMesh* initConvexMesh(cMesh* mesh);
  
public:
  PhysXMain();
  ~PhysXMain();
    
  const double gravity = -9.81 * 0.2;
  const double contactOffset = 0.002;
  
  void collisionFunction(PxContactModifyPair* const, PxU32);
  
  void initGripper(GripperTool* g);
  void initBlock(vector<Block*> &b);
  void initSphere(SphereTool* s);
  void initWall(Wall* w);
  
  void stepPhysics(double t);
    
  cMatrix3d convertMat(PxVec3 x, PxVec3 y, PxVec3 z, PxVec3 w);
};



#endif /* PhysXMain_h */
