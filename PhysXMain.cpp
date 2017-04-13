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
  boxMaterial = gPhysics->createMaterial(0.3f, 0.25f, 0.6f);
  planeMaterial = gPhysics->createMaterial(0.5f, 0.4f, 0.6f);
  cursorMaterial = gPhysics->createMaterial(0.7f, 0.7f, 0.6f);
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


void PhysXMain::stepPhysics(double t)
{
  gScene->simulate(t);
  gScene->fetchResults(true);
}
