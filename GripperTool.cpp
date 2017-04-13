//
//

#include "GripperTool.h"


GripperTool::GripperTool(cGenericHapticDevicePtr d, double s) : Tool(d, s)
{
  root = new cMesh();
  cCreateSphere(root, 0.0075);

  cVector3d pos;
  d->getPosition(pos);
  root->setLocalPos(pos);
  
  cMesh* t1 = new cMesh();
  cMesh* t2 = new cMesh();
  
  cMultiMesh* mesh = new cMultiMesh();
  if(!mesh->loadFromFile("Tool.obj"))
    cout << "failed to load object file" << endl;
  
  mesh->scale(0.002 * s);
  
  mesh->convertToSingleMesh(t1);
  mesh->convertToSingleMesh(t2);
  
  tools.push_back(t1);
  tools.push_back(t2);
  
  root->addChild(t1);
  root->addChild(t2);
}

GripperTool::~GripperTool()
{
  
}

void GripperTool::enableInteraction(bool b)
{
  for (int i = 0; i < root->getNumChildren(); ++i)
    b ? root->getChild(i)->m_material->setRedFireBrick() : root->getChild(i)->m_material->setWhite();
  
  PxU32 numShapes = physXRoot->getNbShapes();
  
  PxShape** shapes;
  shapes = new PxShape*[numShapes];
  
  physXRoot->getShapes(shapes, numShapes);
  
  for (int i = 0; i < numShapes; ++i)
  {
    PxShape* shape = shapes[i];
    PxFilterData filterData;
    filterData.word0 = UNTOUCHABLE; // word0 = own ID
    filterData.word1 = PLANE;
    
    shape->setSimulationFilterData(filterData);
  }
  
  for (PxRigidDynamic* physXTool : physXTools)
  {
    numShapes = physXTool->getNbShapes();
    
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


void GripperTool::update()
{
//  root->setLocalTransform(convertMatrix(physXRoot->getGlobalPose()));
  PxVec3 p = physXRoot->getGlobalPose().p;
  root->setLocalPos(cVector3d(p.x, p.y, p.z));
  
  
  for (int i = 0; i < physXTools.size(); ++i)
  {
    PxTransform t = physXTools[i]->getGlobalPose();
    cTransform trans = convertMatrix(t);
    trans.setLocalPos(trans.getLocalPos() - root->getLocalPos());
    tools[i]->setLocalTransform(trans);
  }
}


void GripperTool::applyForce(cCamera* camera)
{  
  cVector3d position;
  device->getPosition(position);
  
  position = fixPosition(position, camera, scale);
  
  cVector3d force = position - getPosition();
  force *= springConstant;
  
  PxVec3 f = PxVec3(force.x(), force.y(), force.z());
  physXRoot->addForce(f);
  
  
  // Enable the gripper
  bool grip;
  device->getUserSwitch(0, grip);
  grip ? distance = max(minDistance, distance * 0.99) : distance = min(maxDistance, distance * 1.01);
  for (int i = 0; i < 2; ++i) {
    joints[i]->setMaxDistance(distance);
    joints[i]->setMinDistance(distance);
  }
  joints[2]->setMaxDistance(distance * 2.0);
  joints[2]->setMinDistance(distance * 2.0);

  
  
  // Turn the gripper
  bool turn;
  device->getUserSwitch(1, turn);
  if (turn)
    rotation += M_PI / 1800.0;
  
  device->getUserSwitch(3, turn);
  if (turn)
    rotation -= M_PI / 1800.0;
  
  /* Add torque to fix the orientation of the gripper
   */
  PxVec3 newPoint = PxQuat(rotation, PxVec3(0,0,1)).rotate(PxVec3(1,0,0));
  PxVec3 oldPoint = physXTools[1]->getGlobalPose().p - physXRoot->getGlobalPose().p;
  oldPoint.normalize();
  
  PxVec3 w = newPoint.cross(oldPoint);
  w *= asin(w.normalize());
  
  PxQuat q = physXRoot->getGlobalPose().q;
  PxVec3 tensors = physXRoot->getMassSpaceInertiaTensor();
  
  w = q.rotateInv(w);
//  w.x *= tensors.x;
//  w.y *= tensors.y;
//  w.z *= tensors.z;
  w = q.rotate(w);
  
  physXRoot->addTorque(w * springConstant);
}





