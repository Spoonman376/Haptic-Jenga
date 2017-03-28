//
//

#include <stdio.h>
#include "PhysXUtilities.h"


cTransform convertMatrix(PxTransform trans)
{
  
  PxVec3 x = trans.q.getBasisVector0();
  PxVec3 y = trans.q.getBasisVector1();
  PxVec3 z = trans.q.getBasisVector2();
  PxVec3 w = trans.p;
  
  cTransform m;
  
  m.set(x.x, x.y, x.z, 0.0,
        y.x, y.y, y.z, 0.0,
        z.x, z.y, z.z, 0.0,
        w.x, w.y, w.z, 1.0);
  
  return m;
}
