//
//

#include "Tool.h"

Tool::Tool(cGenericHapticDevicePtr d, double s)
{
  device = d;
  
  mass = 0.1;
  springConstant = 1000.0 / s;
  scale = s;
}

Tool::~Tool()
{
  root->removeFromGraph();
  root->deleteAllChildren();
}


cVector3d Tool::getPosition()
{
  return root->getLocalPos();
}

void Tool::setPosition(cVector3d pos)
{
  root->setLocalPos(pos);
  
//  if (physXRoot != nullptr)
//  {
//    PxTransform trans = physXRoot->getGlobalPose();
//    trans.p = PxVec3(pos.x(), pos.y(), pos.z());
//    physXRoot->setGlobalPose(trans);
//    
//    physXRoot->clearForce();
//    physXRoot->clearTorque();
//    physXRoot->setLinearVelocity(PxVec3(0,0,0));
//    physXRoot->setAngularVelocity(PxVec3(0,0,0));
//  }
}

void Tool::setRotation(cMatrix3d rot)
{
  root->setLocalRot(rot);
}


void Tool::update()
{
  PxTransform t = physXRoot->getGlobalPose();
  cTransform m = convertMatrix(t);
  
  root->setLocalTransform(m);
}

void Tool::addToWorld(cWorld* world)
{
  world->addChild(root);
}

void Tool::addReferenceFrames()
{
  root->setShowFrame(true, true);
  root->setShowFrame(false, false);
}

void Tool::setRootActor(PxRigidDynamic* actor)
{
  physXRoot = actor;
}

void Tool::enableInteraction(bool b)
{
  PxU32 numShapes = physXRoot->getNbShapes();
  
  PxShape** shapes;
  shapes = new PxShape*[numShapes];
  
  physXRoot->getShapes(shapes, numShapes);
  
  for (int i = 0; i < numShapes; ++i)
  {
    PxShape* shape = shapes[i];
    PxFilterData filterData;
    filterData.word0 = CURSOR; // word0 = own ID
    filterData.word1 = b ? CURSOR | PLANE | BLOCK : CURSOR | PLANE;
    
    shape->setSimulationFilterData(filterData);
  }
}

void Tool::applyForceToDevice(cCamera* camera)
{
  cVector3d force(0, 0, 0);
  
  cVector3d position;
  device->getPosition(position);

  position = fixPosition(position, camera, scale);
  
  force = getPosition() - position;
  force *= springConstant;
  
  force = cMatrix3d(cVector3d(0.0, 0.0, 1.0), - camera->getSphericalAzimuthRad()) * force;
  
  device->setForce(force);
}

void Tool::applyForce(cCamera* camera)
{
  cVector3d position;
  device->getPosition(position);
 
  position = fixPosition(position, camera, scale);

  cVector3d force = position - getPosition();
  force *= springConstant;
  
  PxVec3 f = PxVec3(force.x(), force.y(), force.z());
  physXRoot->addForce(f);
}

cVector3d Tool::fixPosition(cVector3d pos, cCamera* camera, double scale)
{
  cMatrix3d rot = cMatrix3d(cVector3d(0.0, 0.0, 1.0), camera->getSphericalAzimuthRad());
  pos = rot * pos;
  
  pos *= scale;
  
  pos += camera->getSphericalOriginReference();
  
  return pos;
}














