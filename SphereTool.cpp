//
//

#include "SphereTool.h"



SphereTool::SphereTool()
{
  sphere = new cShapeSphere(radius);
  sphere->setLocalPos(cVector3d(0.0, 0.0, 0.0));
}

SphereTool::~SphereTool()
{
  sphere->removeFromGraph();
  sphere->deleteAllChildren();
  delete sphere;
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
  if (physXSphere != nullptr)
  {
    PxVec3 pos = physXSphere->getGlobalPose().p;
    
    sphere->setLocalPos(cVector3d(pos.x, pos.y, pos.z));
  }
}

void SphereTool::setUp()
{
  // display reference frame
  sphere->setShowFrame(true);
  
  // set the size of the reference frame
  sphere->setFrameSize(0.05);
}


void SphereTool::enableInteraction()
{
  PxU32 numShapes = physXSphere->getNbShapes();
  
  PxShape** shapes;
  shapes = new PxShape*[numShapes];
  
  physXSphere->getShapes(shapes, numShapes);
  
  for (int i = 0; i < numShapes; ++i)
  {
    PxShape* shape = shapes[i];
    PxFilterData filterData;
    filterData.word0 = filter::CURSOR; // word0 = own ID
    filterData.word1 = filter::CURSOR | filter::PLANE | filter::BLOCK;
    
    shape->setSimulationFilterData(filterData);
  }
}

void SphereTool::disableInteraction()
{
  PxU32 numShapes = physXSphere->getNbShapes();
  
  PxShape** shapes;
  shapes = new PxShape*[numShapes];
  
  physXSphere->getShapes(shapes, numShapes);
  
  for (int i = 0; i < numShapes; ++i)
  {
    PxShape* shape = shapes[i];
    PxFilterData filterData;
    filterData.word0 = filter::CURSOR; // word0 = own ID
    filterData.word1 = filter::CURSOR | filter::PLANE | filter::BLOCK;
    
    shape->setSimulationFilterData(filterData);
  }
}



void SphereTool::setPosition(cVector3d pos)
{
  sphere->setLocalPos(pos);
  
//  if (physXSphere != nullptr)
//  {
//    PxTransform trans = physXSphere->getGlobalPose();
//    trans.p = PxVec3(pos.x(), pos.y(), pos.z());
//    physXSphere->setGlobalPose(trans);
//  }
}

void SphereTool::applyForce(cVector3d force)
{
  if (physXSphere != nullptr)
  {
    PxVec3 f = PxVec3(force.x(), force.y(), force.z());
    physXSphere->addForce(f);
  }
}







