//
//

#include "ContactModifyCallback.h"

ContactModifyCallback::ContactModifyCallback(PhysXMain* px)
{
  physX = px;
}

void ContactModifyCallback::onContactModify(PxContactModifyPair* const pairs, PxU32 count)
{
  physX->collisionFunction(pairs, count);
}

void PhysXMain::collisionFunction(PxContactModifyPair* const pairs, PxU32 count)
{
  for (PxU32 i = 0; i < count; ++i)
  {
    const PxRigidDynamic* d0 = pairs[i].actor[0]->is<PxRigidDynamic>();
    const PxRigidDynamic* d1 = pairs[i].actor[1]->is<PxRigidDynamic>();
    
    PxU32 nbPoints = pairs[i].contacts.size();
    
    if(d0 != NULL && d1 != NULL)
    {
      Block* b0 = objectMap[(PxRigidBody*)d0];
      Block* b1 = objectMap[(PxRigidBody*)d1];
      
      if (b0 != nullptr && b1 != nullptr)
      {
        
      }
    }
  }
}

