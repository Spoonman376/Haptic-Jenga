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
  b ? root->m_material->setRedFireBrick() : root->m_material->setWhite();
  
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





