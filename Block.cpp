
#include <stdio.h>
#include "Block.h"

Block::Block()
{
  blockFrame = new cGenericObject();
  
  blockFrame->addChild(createWall(dimX, dimY, cVector3d(0.0, 0.0, dimZ / 2.0), cVector3d(0.0, 0.0, 1.0), 0.0));
  blockFrame->addChild(createWall(dimX, dimY, cVector3d(0.0, 0.0, -dimZ / 2.0), cVector3d(0.0, 0.0, 1.0), 0.0));

  blockFrame->addChild(createWall(dimZ, dimY, cVector3d(dimX / 2.0, 0.0, 0.0), cVector3d(0.0, 1.0, 0.0), M_PI/2));
  blockFrame->addChild(createWall(dimZ, dimY, cVector3d(-dimX / 2.0, 0.0, 0.0), cVector3d(0.0, 1.0, 0.0), -M_PI/2));

  blockFrame->addChild(createWall(dimX, dimZ, cVector3d(0.0, dimY / 2.0, 0.0), cVector3d(1.0, 0.0, 0.0), M_PI/2));
  blockFrame->addChild(createWall(dimX, dimZ, cVector3d(0.0, -dimY / 2.0, 0.0), cVector3d(1.0, 0.0, 0.0), -M_PI/2));
  
  
  physXBlock = nullptr;
}

Block::~Block()
{
  blockFrame->removeFromGraph();
  blockFrame->deleteAllChildren();
  delete blockFrame;
}

cMesh* Block::createWall(double width, double height, cVector3d pos, cVector3d axis, double angle)
{
  cMesh* wall = new cMesh();
  
  cCreatePlane(wall, width, height);
  wall->m_texture = cTexture2d::create();
  wall->m_texture->loadFromFile("woodTexture.jpg");
  wall->setUseTexture(true);
  wall->setLocalRot(cRotAxisAngleRad(axis.x(), axis.y(), axis.z(), angle));
  wall->setLocalPos(cVector3d(pos.x(), pos.y(), pos.z()));
  
  return wall;
}

cVector3d Block::getPosition()
{
  return blockFrame->getLocalPos();
}

cMatrix3d Block::getRotation()
{
  return blockFrame->getLocalRot();
}

void Block::setPosition(cVector3d pos)
{
  blockFrame->setLocalPos(pos);
  
  if (physXBlock != nullptr)
  {
    PxTransform trans = physXBlock->getGlobalPose();
    trans.p = PxVec3(pos.x(), pos.y(), pos.z());
    physXBlock->setGlobalPose(trans);
    
    physXBlock->clearForce();
    physXBlock->clearTorque();
    physXBlock->setLinearVelocity(PxVec3(0,0,0));
    physXBlock->setAngularVelocity(PxVec3(0,0,0));
  }
}



void Block::setRotation(cMatrix3d rot)
{
  blockFrame->setLocalRot(rot);
  
  if (physXBlock != nullptr)
  {
    cVector3d axis;
    double angle;
    
    rot.toAxisAngle(axis, angle);
    
    PxTransform trans = physXBlock->getGlobalPose();
    trans.q = PxQuat(angle, PxVec3(axis.x(), axis.y(), axis.z()));
    physXBlock->setGlobalPose(trans);
  }
}


void Block::enableInteraction()
{
  PxU32 numShapes = physXBlock->getNbShapes();
  
  PxShape** shapes;
  shapes = new PxShape*[numShapes];
  
  physXBlock->getShapes(shapes, numShapes);
  
  for (int i = 0; i < numShapes; ++i)
  {
    PxShape* shape = shapes[i];
    PxFilterData filterData;
    filterData.word0 = filter::BLOCK; // word0 = own ID
    filterData.word1 = filter::BLOCK | filter::PLANE | filter::CURSOR;
    
    shape->setSimulationFilterData(filterData);
  }
}

void Block::disableInteraction()
{
  PxU32 numShapes = physXBlock->getNbShapes();
  
  PxShape** shapes;
  shapes = new PxShape*[numShapes];
  
  physXBlock->getShapes(shapes, numShapes);
  
  for (int i = 0; i < numShapes; ++i)
  {
    PxShape* shape = shapes[i];
    PxFilterData filterData;
    filterData.word0 = filter::BLOCK; // word0 = own ID
    filterData.word1 = filter::BLOCK | filter::PLANE | filter::CURSOR;
    
    shape->setSimulationFilterData(filterData);
  }
}



void Block::addToWorld(cWorld* world)
{
  world->addChild(blockFrame);
}

void Block::setActor(PxRigidDynamic* b)
{
  physXBlock = b;
}

void Block::update()
{
  PxTransform t = physXBlock->getGlobalPose();
  cTransform m = convertMatrix(t);
  
  blockFrame->setLocalTransform(m);
}



