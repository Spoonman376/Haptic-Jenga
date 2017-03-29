
#include "Wall.h"



Wall::Wall(double w, double h, cVector3d pos, cVector3d axis, double angle, string textureFile)
{
  wall = new cMesh();
  
  width = w;
  height = h;
  
  cMatrix3d rotation = cRotAxisAngleRad(axis.x(), axis.y(), axis.z(), angle);
  normal = rotation * cVector3d(0,0,1);
  
  cCreatePlane(wall, width, height);
  wall->m_texture = cTexture2d::create();
  
  if (!wall->m_texture->loadFromFile(textureFile))
    cout << "texture load failed" << endl;
  
  wall->setUseTexture(true);
  wall->setLocalRot(rotation);
  wall->setLocalPos(cVector3d(pos.x(), pos.y(), pos.z()));
}


cVector3d Wall::getPosition()
{
  return wall->getLocalPos();
}

cMatrix3d Wall::getRotation()
{
  return wall->getLocalRot();
}

cVector3d Wall::getNormal()
{
  return normal;
}

double Wall::getWidth()
{
  return width;
}

double Wall::getHeight()
{
  return height;
}


void Wall::addToWorld(cWorld *world)
{
  world->addChild(wall);
}

void Wall::setActor(PxRigidStatic* s)
{
  physXWall = s;
}
