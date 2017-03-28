
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
}

Block::~Block()
{
  
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

void Block::setPosition(cVector3d pos)
{
  blockFrame->setLocalPos(pos);
}

void Block::setRotation(cMatrix3d rot)
{
  blockFrame->setLocalRot(rot);
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
