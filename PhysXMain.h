//
//

#ifndef PhysXMain_h
#define PhysXMain_h

#include <stdio.h>
#include "PxScene.h"
#include "PxPhysics.h"
#include "PxPhysicsAPI.h"

#include "JengaFilterShader.h"

#include "Block.h"

using namespace physx;
using namespace std;
using namespace chai3d;


class PhysXMain
{
private:
  map<PxRigidActor*, Block*> objectMap;
  
  PxDefaultAllocator		      gAllocator;
  PxDefaultErrorCallback    	gErrorCallback;
  PxFoundation*			          gFoundation = NULL;
  PxPhysics*				          gPhysics = NULL;
  PxDefaultCpuDispatcher*   	gDispatcher = NULL;
  PxScene*				            gScene = NULL;
  //PxCooking*				          gCooking = NULL;
  PxMaterial*				          gMaterial = NULL;
  //PxVisualDebuggerConnection* gConnection = NULL;
  //VehicleSceneQueryData*	    gVehicleSceneQueryData = NULL;
  PxBatchQuery*			          gBatchQuery = NULL;
  //PxRigidStatic*			        gGroundPlane = NULL;
  //PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;
  //bool					gIsVehicleInAir = true; // unused
  
  
public:
  PhysXMain();
  ~PhysXMain();
  
  //vector<GEO*> deletedObjects;

  void collisionFunction(PxContactModifyPair* const, PxU32);
  
  void initBlock(Block* b);
  void initScene();
  

  void stepPhysics(double t);
  void cleanupPhysics(bool interactive);
  
  cMatrix3d convertMat(PxVec3 x, PxVec3 y, PxVec3 z, PxVec3 w);
};



#endif /* PhysXMain_h */
