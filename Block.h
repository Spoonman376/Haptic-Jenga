
#ifndef Block_h
#define Block_h

#include "chai3d.h"
#include "PhysXUtilities.h"

using namespace chai3d;

class Block
{
private:
  cMesh* createWall(double width, double height, cVector3d pos, cVector3d axis, double angle);
  
  
public:
  cGenericObject* blockFrame;
  PxRigidDynamic* physXBlock;

  Block(double s);
  ~Block();
  
  static double scale;
  static double dimX;
  static double dimY;
  static double dimZ;
  
  cVector3d getPosition();
  cMatrix3d getRotation();

  void setPosition(cVector3d);
  void setRotation(cMatrix3d);
  
  void enableInteraction(bool);
  
  void addToWorld(cWorld* world);
  void setActor(PxRigidDynamic* b);
  
  void update();
};



#endif /* Block_h */
