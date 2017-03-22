
#include "Wall.h"



Wall::Wall(double width, double height, cVector3d pos, cVector3d axis, double angle, string textureFile)
{
  wall = new cMesh();
  
  cCreatePlane(wall, width, height);
  wall->m_texture = cTexture2d::create();
  wall->m_texture->loadFromFile(textureFile);
  wall->setUseTexture(true);
  wall->setLocalRot(cRotAxisAngleRad(axis.x(), axis.y(), axis.z(), angle));
  wall->setLocalPos(cVector3d(pos.x(), pos.y(), pos.z()));
}
