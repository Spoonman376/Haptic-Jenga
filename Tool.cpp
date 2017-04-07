//
//

#include "Tool.h"

Tool::Tool(cGenericHapticDevicePtr d, double s)
{
  device = d;
  
  mass = 0.1;
  springConstant = 1000.0 / s;
  scale = s;
}

Tool::~Tool()
{
  root->removeFromGraph();
  root->deleteAllChildren();
}


void Tool::setPosition(cVector3d pos)
{
  root->setLocalPos(pos);
}

void Tool::setRotation(cMatrix3d rot)
{
  root->setLocalRot(rot);
}


void Tool::update()
{
  for (int i = 0; i < physXTools.size(); ++i)
  {
    PxTransform t = physXTools[i]->getGlobalPose();
    cTransform m = convertMatrix(t);
    
    tools[i]->setLocalTransform(m);
  }
}

void Tool::addToWorld(cWorld* world)
{
  world->addChild(root);
}

cMesh* Tool::getMesh()
{
  return root;
}

void Tool::addReferenceFrames()
{
  root->setShowFrame(true, true);
  root->setShowFrame(false, false);
}

void Tool::addActor(PxRigidDynamic* actor)
{
  physXTools.push_back(actor);
}

void Tool::setRootActor(PxRigidDynamic* actor)
{
  physXRoot = actor;
}

void Tool::enableInteraction(bool b)
{
  for (PxRigidDynamic* physXTool : physXTools)
  {
    PxU32 numShapes = physXTool->getNbShapes();
    
    PxShape** shapes;
    shapes = new PxShape*[numShapes];
    
    physXTool->getShapes(shapes, numShapes);
    
    for (int i = 0; i < numShapes; ++i)
    {
      PxShape* shape = shapes[i];
      PxFilterData filterData;
      filterData.word0 = CURSOR; // word0 = own ID
      filterData.word1 = b ? CURSOR | PLANE | BLOCK : CURSOR | PLANE;
      
      shape->setSimulationFilterData(filterData);
    }
  }
}

void Tool::applyForce(cVector3d force)
{
  for (PxRigidDynamic* physXTool : physXTools)
    physXTool->addForce(PxVec3(force.x(), force.y(), force.z()));
}
















