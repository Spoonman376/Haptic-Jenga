//
//

#include "PhysXMain.h"


PhysXMain::PhysXMain()
{
  gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
  PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);
  gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, profileZoneManager);
  
  PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
  sceneDesc.gravity = PxVec3(0.0f, 0.0f, gravity);
  gDispatcher = PxDefaultCpuDispatcherCreate(2);
  sceneDesc.cpuDispatcher = gDispatcher;
  
  sceneDesc.filterShader = JengaFilterShader;
  sceneDesc.contactModifyCallback = new ContactModifyCallback(this);
  
  gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
  
  gScene = gPhysics->createScene(sceneDesc);
  
  //static friction, dynamic friction, restitution
  gMaterial = gPhysics->createMaterial(1.0f, 1.0f, 0.6f);
}

PhysXMain::~PhysXMain()
{
  gScene->release();
  gDispatcher->release();
  gPhysics->getProfileZoneManager()->release();
  gPhysics->release();
  gCooking->release();
  gFoundation->release();
  
  printf("PhysX done.\n");
}


void PhysXMain::initBlock(vector<Block*> &blocks)
{
  PxShape* shape = gPhysics->createShape(PxBoxGeometry(Block::dimX / 2.0, Block::dimY / 2.0, Block::dimZ / 2.0), *gMaterial);
  
  shape->setContactOffset(contactOffset);
  
  for (Block* b : blocks)
  {
    cVector3d pos = b->getPosition();
    
    cVector3d axis;
    double angle;
    b->getRotation().toAxisAngle(axis, angle);
    
    PxTransform trans = PxTransform(PxVec3(pos.x(), pos.y(), pos.z()), PxQuat(angle, PxVec3(axis.x(), axis.y(), axis.z())));
    
    PxRigidDynamic *body = gPhysics->createRigidDynamic(trans);
    body->attachShape(*shape);
    
    PxRigidBodyExt::updateMassAndInertia(*body, 15000.0);
    
    body->setLinearDamping(1.0);
    body->setAngularDamping(1.0);
    
    b->setActor(body);
    
    gScene->addActor(*body);
    objectMap.insert(make_pair(body, b));
  }
}

PxConvexMesh* PhysXMain::initConvexMesh(cMesh* mesh)
{
  PxConvexMeshDesc meshDesc;
  
  vector<PxVec3> verts;
  vector<PxU32> indices;
  
  vector<PxHullPolygon> polys;
    
  for (int i = 0; i < mesh->m_triangles->m_vertices->getNumElements(); i++) {
    cVector3d p = mesh->m_triangles->m_vertices->m_localPos[i];
    verts.push_back(PxVec3(p.x(), p.y(), p.z()));
  }
  for (int i = 0; i < mesh->getNumTriangles(); i++)
  {
    indices.push_back(mesh->m_triangles->getVertexIndex0(i));
    indices.push_back(mesh->m_triangles->getVertexIndex1(i));
    indices.push_back(mesh->m_triangles->getVertexIndex2(i));
  }
  
  meshDesc.points.count = verts.size();
  meshDesc.points.stride = sizeof(PxVec3);
  meshDesc.points.data = &verts[0];
  meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

  
  PxDefaultMemoryOutputStream writeBuffer;
  if (!gCooking->cookConvexMesh(meshDesc, writeBuffer))
    return NULL;
  PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
  return gPhysics->createConvexMesh(readBuffer);
}

void PhysXMain::initSphere(SphereTool *s)
{
  PxShape* shape = gPhysics->createShape(PxConvexMeshGeometry(initConvexMesh(s->getMesh())), *gMaterial);
  shape->setContactOffset(contactOffset);
  
  cVector3d pos = s->getPosition();
  
  PxRigidDynamic *body = gPhysics->createRigidDynamic(PxTransform(PxVec3(pos.x(), pos.y(), pos.z())));
  
  
  body->attachShape(*shape);
  
  PxRigidBodyExt::updateMassAndInertia(*body, 15000.0);
  body->setLinearDamping(1.0);
  body->setAngularDamping(1.0);
  
  body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
  
  s->setRootActor(body);
  
  gScene->addActor(*body);
  toolMap.insert(make_pair(body, s));
}

void PhysXMain::initWall(Wall* w)
{
  // Create the shape
  PxShape* shape = gPhysics->createShape(PxBoxGeometry(w->getWidth() / 2.0, w->getHeight() / 2.0, 0.05), *gMaterial);
  
  shape->setContactOffset(contactOffset);
  
  PxFilterData filterData;
  filterData.word0 = filter::PLANE; // word0 = own ID
  filterData.word1 = filter::BLOCK | filter::CURSOR;  // word1 = ID mask to filter pairs that trigger a contact callback
  
  shape->setSimulationFilterData(filterData);
  
  
  // Create the Actor
  cVector3d n = w->getNormal(); //normal
  cVector3d p = w->getPosition(); // position
  cVector3d a; // axis of rotation
  double angle; // radians
  w->getRotation().toAxisAngle(a, angle);
  
  PxVec3 position = PxVec3(p.x(), p.y(), p.z()) - 0.05 * PxVec3(n.x(), n.y(), n.z());
  PxTransform trans = PxTransform(position, PxQuat(angle, PxVec3(a.x(), a.y(), a.z())));
  PxRigidStatic* plane = gPhysics->createRigidStatic(trans);
  
  plane->attachShape(*shape);
  
  // Add the actor to the scene
  w->setActor(plane);
  gScene->addActor(*plane);
}


void PhysXMain::stepPhysics(double t)
{
  gScene->simulate(t);
  gScene->fetchResults(true);
}
