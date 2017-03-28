//
//

#include "SphereTool.h"



SphereTool::SphereTool(double r)
{
  radius = r;
  sphere = new cShapeSphere(radius);
  sphere->setLocalPos(cVector3d(0,0.03, 0.01));
}

void SphereTool::addToWorld(cWorld *world)
{
  world->addChild(sphere);
}

cVector3d SphereTool::getPosition()
{
  return sphere->getLocalPos();
}

void SphereTool::setActor(PxRigidDynamic* s)
{
  physXSphere = s;
}

void SphereTool::update()
{
  PxVec3 pos = physXSphere->getGlobalPose().p;
  
  sphere->setLocalPos(cVector3d(pos.x, pos.y, pos.z));
  
}

void SphereTool::setUp()
{
  // display reference frame
  sphere->setShowFrame(true);
  
  // set the size of the reference frame
  sphere->setFrameSize(0.05);
}



void SphereTool::applyForce(cVector3d force)
{
  PxVec3 f = PxVec3(force.x(), force.y(), force.z());
  physXSphere->addForce(f);
}



