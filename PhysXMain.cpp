//
//

#include "PhysXMain.h"


PhysXMain::PhysXMain()
{
  gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
  PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);
  gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, profileZoneManager);
  
  PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
  sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.01*-9.81f);
  gDispatcher = PxDefaultCpuDispatcherCreate(2);
  sceneDesc.cpuDispatcher = gDispatcher;
  
  sceneDesc.filterShader = JengaFilterShader;	
  
  //sceneDesc.contactModifyCallback = new ContactModifyCallback(this);
  gScene = gPhysics->createScene(sceneDesc);
  
  gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);	//static friction, dynamic friction, restitution
  
  //gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
  
}


void PhysXMain::initBlock(Block* b)
{
  PxShape* shape = gPhysics->createShape(PxBoxGeometry(b->dimX / 2.0, b->dimY / 2.0, b->dimZ / 2.0), *gMaterial);
  cVector3d pos = b->getPosition();
  
  PxFilterData filterData;
  filterData.word0 = filter::BLOCK; // word0 = own ID
  filterData.word1 = filter::BLOCK | filter::CURSOR | filter::PLANE;  // word1 = ID mask to filter pairs that trigger a contact callback
  
  shape->setSimulationFilterData(filterData);
  
  PxRigidDynamic *body = gPhysics->createRigidDynamic(PxTransform(PxVec3(pos.x(), pos.y(), pos.z())));
  
  body->setMass(0.0106875);
  
  body->attachShape(*shape);
  gScene->addActor(*body);
  objectMap.insert(make_pair(body, b));
}

void PhysXMain::initScene()
{
  
  //PxShape* shape = gPhysics->createShape(PxPlane(PxVec3(0,1,0), 0), *gMaterial);
  
  PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.5, 0.5, 0.05), *gMaterial);
  
  PxFilterData filterData;
  filterData.word0 = filter::BLOCK; // word0 = own ID
  filterData.word1 = filter::BLOCK | filter::CURSOR;  // word1 = ID mask to filter pairs that trigger a contact callback

  shape->setSimulationFilterData(filterData);

  PxRigidStatic* plane = gPhysics->createRigidStatic(PxTransform(PxVec3(0.0, 0.0, -0.05)));

  plane->attachShape(*shape);
  gScene->addActor(*plane);
}



void PhysXMain::stepPhysics(double t)
{
  
  gScene->simulate(t);
  gScene->fetchResults(true);

  map<PxRigidActor*, Block*>::iterator itr = objectMap.begin();
  while (itr != objectMap.end())
  {
    PxRigidActor* body = itr->first;
    if (body != nullptr && itr->second != nullptr)
    {
      PxVec3 pos = body->getGlobalPose().p;
      
      itr->second->setPosition(cVector3d(pos.x, pos.y, pos.z));
      
      PxReal angle = 0;
      PxVec3 axis;
      
      body->getGlobalPose().q.toRadiansAndUnitAxis(angle, axis);
      
      cMatrix3d M;
      M.setAxisAngleRotationRad(cVector3d(axis.x, axis.y, axis.z), (double)angle);
      
      itr->second->setRotation(M);
      
    }
    
    ++itr;
  }
}


void PhysXMain::cleanupPhysics(bool interactive)
{
  PX_UNUSED(interactive);
  gScene->release();
  gDispatcher->release();
  PxProfileZoneManager* profileZoneManager = gPhysics->getProfileZoneManager();
  gPhysics->release();
  profileZoneManager->release();
  gFoundation->release();
  
  printf("PhysX done.\n");
}
