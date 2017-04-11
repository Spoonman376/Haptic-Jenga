//
//

#include "GripperTool.h"


GripperTool::GripperTool(cGenericHapticDevicePtr d, double s) : Tool(d, s)
{
  root = new cMesh();
  cCreateSphere(root, 0.01);
  
  cMesh* t1 = new cMesh();
  cMesh* t2 = new cMesh();
  
  cMultiMesh* mesh = new cMultiMesh();
  if(!mesh->loadFromFile("Tool.obj"))
    cout << "failed to load object file" << endl;
  
  mesh->scale(0.003 * s);
  
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


void GripperTool::applyForce()
{  
//  cVector3d position;
//  device->getPosition(position);
//  position *= scale;
//  
//  cVector3d force = position - getPosition();
//  force *= springConstant;
//  
//  physXRoot->addForce(PxVec3(force.x(), force.y(), force.z()));
//  
//  bool grip;
//  device->getUserSwitch(0, grip);
//  
//  //Turning
//  bool turn;
//  device->getUserSwitch(1, turn);
//  
//  if (turn)
//    rotation += M_PI / 180.0;
//  
//  device->getUserSwitch(2, turn);
//  if (turn)
//    rotation -= M_PI / 180.0;
  
  
  PxVec3 newPoint = PxQuat(rotation, PxVec3(0,0,1)).rotate(PxVec3(1,0,0));
  PxVec3 oldPoint = physXTools[1]->getGlobalPose().p - physXRoot->getGlobalPose().p;
  oldPoint.normalize();
  
  PxVec3 w = newPoint.cross(oldPoint);
  w *= asin(w.normalize());
  
  PxQuat q = physXRoot->getGlobalPose().q;
  PxVec3 tensors = physXRoot->getMassSpaceInertiaTensor();
  
  w = q.rotateInv(w);
  w.x *= tensors.x;
  w.y *= tensors.y;
  w.z *= tensors.z;
  w = q.rotate(w);
  
  physXRoot->addTorque(w * springConstant);
  
//  //find rotation around y axis
//  PxVec3 newPoint = physXTools[1]->getGlobalPose().p - physXRoot->getGlobalPose().p;
//  newPoint.z = 0;
//  newPoint.normalize();
//  
//  PxVec3 oldPoint = physXTools[1]->getGlobalPose().p - physXRoot->getGlobalPose().p;
//  oldPoint.normalize();
// 
//  PxVec3 w = oldPoint.cross(newPoint);
//  w.normalize();
//  w *= acos(newPoint.dot(oldPoint));
//  
//  physXRoot->addTorque(w * springConstant);

  
//  for (int i = 0; i < joints.size(); ++i)
//  {
//  }
  
  
//  // Springs
//  for (Spring* s : springs)
//  {
//    grip ? s->restLength *= 0.9 : s->restLength = max(s->maxRestLength, s->restLength * 1.1);
//    s->applyForce();
//  }
//  
//  // Tethers
//  for (int i = 0; i < physXTools.size(); ++i)
//  {
//    PxVec3 pos = physXTools[i]->getGlobalPose().p;
//    PxVec3 d = springs[i]->restLength * PxVec3(0, 1.0, 0);
//    PxQuat r = PxQuat(i * M_PI + rotation, PxVec3(0, 0, 1.0));
//    
//    PxVec3 f = physXRoot->getGlobalPose().p + r.rotate(d) - pos;
//    f *= springConstant;
//    
//    physXTools[i]->addForce(f);
//    physXRoot->addForce(-f);
//  }
  
}





