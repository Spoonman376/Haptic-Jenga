//
//

#include "SphereTool.h"



SphereTool::SphereTool(cGenericHapticDevicePtr d, double scale) : Tool(d, scale)
{
  radius = 0.003 * scale;
  
  cMultiMesh* mesh = new cMultiMesh();
  if(!mesh->loadFromFile("Tool.obj"))
    cout << "failed to load object file" << endl;

  mesh->scale(radius);

  root = new cMesh();
  mesh->convertToSingleMesh(root);
}

SphereTool::~SphereTool()
{
  
}

void SphereTool::update()
{
  PxTransform t = physXRoot->getGlobalPose();
  cTransform m = convertMatrix(t);
  
  root->setLocalTransform(m);
}

void SphereTool::enableInteraction(bool b)
{
  PxU32 numShapes = physXRoot->getNbShapes();
  
  PxShape** shapes;
  shapes = new PxShape*[numShapes];
  
  physXRoot->getShapes(shapes, numShapes);
  
  for (int i = 0; i < numShapes; ++i)
  {
    PxShape* shape = shapes[i];
    PxFilterData filterData;
    filterData.word0 = CURSOR; // word0 = own ID
    filterData.word1 = b ? CURSOR | PLANE | BLOCK : CURSOR | PLANE;
    
    shape->setSimulationFilterData(filterData);
  }
}

cVector3d SphereTool::getPosition()
{
  return root->getLocalPos();
}

void SphereTool::setPosition(cVector3d pos)
{
  root->setLocalPos(pos);
  
  if (physXRoot != nullptr)
  {
    PxTransform trans = physXRoot->getGlobalPose();
    trans.p = PxVec3(pos.x(), pos.y(), pos.z());
    physXRoot->setGlobalPose(trans);
    
    physXRoot->clearForce();
    physXRoot->clearTorque();
    physXRoot->setLinearVelocity(PxVec3(0,0,0));
    physXRoot->setAngularVelocity(PxVec3(0,0,0));
  }
}


void SphereTool::applyForceToDevice(double scale)
{
  cVector3d force(0, 0, 0);
  
  cVector3d position;
  device->getPosition(position);
  position *= scale;
  
  force = getPosition() - position;
  force *= springConstant;
  
  device->setForceAndTorqueAndGripperForce(force, cVector3d(), 0.0);
}

void SphereTool::applyForce()
{
  cVector3d position;
  device->getPosition(position);
  position *= scale;
  
  cVector3d force = position - getPosition();
  force *= springConstant;
  
  PxVec3 f = PxVec3(force.x(), force.y(), force.z());
  physXRoot->addForce(f);
}







