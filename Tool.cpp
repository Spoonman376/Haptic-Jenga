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
  
  for (Spring* spring : springs)
    delete spring;
}


cVector3d Tool::getPosition()
{
  return root->getLocalPos();
}

void Tool::setPosition(cVector3d pos)
{
  root->setLocalPos(pos);
  
  if (physXRoot != nullptr)
  {
    PxTransform trans = physXRoot->getGlobalPose();
    trans.p = PxVec3(pos.x(), pos.y(), pos.z());
    physXRoot->setGlobalPose(trans);
    
    physXRoot->clearForce();
    physXRoot->clearTorque();
    physXRoot->setLinearVelocity(PxVec3(0,0,0));
    physXRoot->setAngularVelocity(PxVec3(0,0,0));
  }
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

void Tool::addActor(PxRigidDynamic* actor)
{
  physXTools.push_back(actor);
}

void Tool::setRootActor(PxRigidDynamic* actor)
{
  physXRoot = actor;
}

void Tool::enableInteraction(bool b)
{
  for (PxRigidDynamic* physXTool : physXTools)
  {
    PxU32 numShapes = physXTool->getNbShapes();
    
    PxShape** shapes;
    shapes = new PxShape*[numShapes];
    
    physXTool->getShapes(shapes, numShapes);
    
    for (int i = 0; i < numShapes; ++i)
    {
      PxShape* shape = shapes[i];
      PxFilterData filterData;
      filterData.word0 = CURSOR; // word0 = own ID
      filterData.word1 = b ? CURSOR | PLANE | BLOCK : CURSOR | PLANE;
      
      shape->setSimulationFilterData(filterData);
    }
  }
}

void Tool::applyForceToDevice()
{
  cVector3d force(0, 0, 0);
  
  cVector3d position;
  device->getPosition(position);
  position *= scale;
  
  force = getPosition() - position;
  force *= springConstant;
  
  device->setForceAndTorqueAndGripperForce(force, cVector3d(), 0.0);
}

void Tool::applyForce()
{
  cVector3d position;
  device->getPosition(position);
  position *= scale;
  
  cVector3d force = position - getPosition();
  force *= springConstant;
  
  PxVec3 f = PxVec3(force.x(), force.y(), force.z());
  physXRoot->addForce(f);
}
















