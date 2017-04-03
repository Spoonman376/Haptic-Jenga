//
//

#ifndef ContactModifyCallback_h
#define ContactModifyCallback_h

#include <stdio.h>

#include "PhysXMain.h"

class PhysXMain;

class ContactModifyCallback : public PxContactModifyCallback
{
private:
  PhysXMain* physX;
  
public:
  ContactModifyCallback(PhysXMain*);
  
  void onContactModify(PxContactModifyPair* const pairs, PxU32 count);
  
  
};

#endif /* ContactModifyCallback_h */
