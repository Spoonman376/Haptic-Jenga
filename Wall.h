//
//

#ifndef Wall_h
#define Wall_h

#include <stdio.h>
#include "chai3d.h"

using namespace chai3d;
using namespace std;

class Wall
{
private:
  cMesh *wall;
  
  
public:
  Wall(double width, double height, cVector3d pos, cVector3d axis, double angle, string textureFile);
  ~Wall();
  
  
  
};


#endif /* Wall_h */
