//
//

#include "GripperTool.h"


GripperTool::GripperTool(cGenericHapticDevicePtr d, double s) : Tool(d, s)
{
  root = new cMesh();
  
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
  cVector3d position;
  device->getPosition(position);
  position *= scale;
  
  cVector3d force = position - getPosition();
  force *= springConstant;
  
  physXRoot->addForce(PxVec3(force.x(), force.y(), force.z()));
  
  bool grip;
  device->getUserSwitch(0, grip);
  
  //Turning
  bool turn;
  device->getUserSwitch(1, turn);
  
  if (turn)
    rotation += M_PI / 180.0;
  
  device->getUserSwitch(2, turn);
  if (turn)
    rotation -= M_PI / 180.0;
  
  
  // Springs
  for (Spring* s : springs)
  {
    grip ? s->restLength *= 0.9 : s->restLength = max(s->maxRestLength, s->restLength * 1.1);
    s->applyForce();
  }
  
  // Tethers
  for (int i = 0; i < physXTools.size(); ++i)
  {
    PxVec3 pos = physXTools[i]->getGlobalPose().p;
    PxVec3 d = springs[i]->restLength * PxVec3(0, 1.0, 0);
    PxQuat r = PxQuat(i * M_PI + rotation, PxVec3(0, 0, 1.0));
    
    PxVec3 f = physXRoot->getGlobalPose().p + r.rotate(d) - pos;
    f *= springConstant;
    
    physXTools[i]->addForce(f);
    physXRoot->addForce(-f);
  }
  
}





