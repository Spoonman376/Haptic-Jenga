
#ifndef Block_h
#define Block_h

#include "chai3d.h"

using namespace chai3d;

class Block
{
private:
  // chai3d Object
  cGenericObject* blockFrame;

  // PhysX Object
  
  
  cMesh* createWall(double width, double height, cVector3d pos, cVector3d axis, double angle);
  
public:
  Block();
  ~Block();
  
  const double scale = 1.0;
  
  const double dimX = 0.025 * scale;
  const double dimY = 0.075 * scale;
  const double dimZ = 0.015 * scale;
  
  cVector3d getPosition();
  void setPosition(cVector3d);
  void setRotation(cMatrix3d);
  
  void addToWorld(cWorld* world);
};



#endif /* Block_h */
