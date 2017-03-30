
#ifndef Block_h
#define Block_h

#include "chai3d.h"
#include "PhysXUtilities.h"

using namespace chai3d;

class Block
{
private:
  // chai3d Object
  cGenericObject* blockFrame;

  // PhysX Object
  PxRigidDynamic* physXBlock;
  
  cMesh* createWall(double width, double height, cVector3d pos, cVector3d axis, double angle);
  
public:
  Block();
  ~Block();
  
  const double scale = 1.0;
  
  const double dimX = 0.025 * scale;
  const double dimY = 0.075 * scale;
  const double dimZ = 0.015 * scale;
  
  cVector3d getPosition();
  cMatrix3d getRotation();

  void setPosition(cVector3d);
  void setRotation(cMatrix3d);
  
  void enableInteraction();
  void disableInteraction();
  
  void addToWorld(cWorld* world);
  void setActor(PxRigidDynamic* b);
  
  void update();
};



#endif /* Block_h */
