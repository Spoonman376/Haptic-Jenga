//
//

#ifndef Spring_h
#define Spring_h

#include <stdio.h>
#include "physXUtilities.h"



class Spring
{
public:
  
  double springConstant;
  double restLength;
  double maxRestLength;
  double damping;

  Spring(PxRigidDynamic*, PxRigidDynamic*, double, double);
  ~Spring();
  
  PxRigidDynamic* objectA;
  PxRigidDynamic* objectB;
  
  void applyForce();
};

#endif /* Spring_h */
