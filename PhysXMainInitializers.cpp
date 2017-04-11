//
//

#include "PhysXMain.h"

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
    
    PxRigidBodyExt::updateMassAndInertia(*body, 100.0);
    
    body->setLinearDamping(1.0);
    body->setAngularDamping(1.0);
    
    b->setActor(body);
    
    gScene->addActor(*body);
    objectMap.insert(make_pair(body, b));
  }
}

void PhysXMain::initSphere(SphereTool *s)
{
  PxShape* shape = gPhysics->createShape(PxConvexMeshGeometry(initConvexMesh(s->root)), *gMaterial);
  shape->setContactOffset(contactOffset);
  
  cVector3d pos = s->getPosition();
  
  PxRigidDynamic *body = gPhysics->createRigidDynamic(PxTransform(PxVec3(pos.x(), pos.y(), pos.z())));
  
  body->attachShape(*shape);
  
  PxRigidBodyExt::updateMassAndInertia(*body, 15000.0);
  body->setLinearDamping(2 * sqrt(s->springConstant * body->getMass()));
  body->setAngularDamping(2 * sqrt(body->getMass()));
  
//  body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
  
  s->setRootActor(body);
  
  gScene->addActor(*body);
  toolMap.insert(make_pair(body, s));
}

void PhysXMain::initGripper(GripperTool* g)
{
  PxShape* shape = gPhysics->createShape(PxConvexMeshGeometry(initConvexMesh(g->tools[0])), *gMaterial);
  
  cVector3d pos = g->getPosition();
  g->physXRoot = gPhysics->createRigidDynamic(PxTransform(PxVec3(pos.x(), pos.y(), pos.z())));
  g->physXRoot->attachShape(* gPhysics->createShape(PxSphereGeometry(0.01), *gMaterial));
  
  g->physXRoot->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
  g->physXRoot->setAngularDamping(3.0);
  
  PxRigidBodyExt::updateMassAndInertia(*g->physXRoot, 15000.0);
  
  gScene->addActor(* g->physXRoot);
  
  for (int i = 0; i < g->tools.size(); ++i) {
    PxVec3 offset = PxVec3(g->distance, 0, 0);
    PxQuat rotation = PxQuat((double)i * M_PI, PxVec3(0.0, 0.0, 1.0));
    
    PxTransform trans = PxTransform(PxVec3(pos.x(), pos.y(), pos.z()) + rotation.rotate(offset), PxQuat((double)(i * 2 + 1) * M_PI / 2.0, PxVec3(0.0, 1.0, 0.0)));
    
    PxRigidDynamic* body = gPhysics->createRigidDynamic(trans);
    body->attachShape(*shape);
    
    PxRigidBodyExt::updateMassAndInertia(*body, 150);
    body->setLinearDamping(2 * sqrt(g->springConstant * body->getMass()));
    body->setAngularDamping(2 * sqrt(body->getMass()));
    
    body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
    
    PxTransform t = PxTransform::createIdentity();
    t.p = PxVec3(0,0,0);
    
    PxD6Joint* joint = PxD6JointCreate(*gPhysics, g->physXRoot, t, body, t);
    joint->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
    joint->setMotion(PxD6Axis::eTWIST,  PxD6Motion::eFREE);
    joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
    joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
    
    PxDistanceJoint* dJoint = PxDistanceJointCreate(*gPhysics, body, t, g->physXRoot, t);
    dJoint->setMaxDistance(g->distance);
    dJoint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
    dJoint->setMinDistance(g->distance);
    dJoint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);
    dJoint->setStiffness(1.0);
    
//    g->springs.push_back(dJoint);
    
    g->physXTools.push_back(body);
    
    gScene->addActor(*body);
  }
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

