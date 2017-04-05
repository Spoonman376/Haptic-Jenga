//
//

#include "Game.h"


Game::Game(vector<cGenericHapticDevicePtr> devicePtrs)
{
  
  // Initial chai3d world set up
  world = new cWorld();
  camera = new cCamera(world);
  world->addChild(camera);
  
  camera->setSphericalRad(0.2, M_PI / 3.0, 0);
  
  // set the near and far clipping planes of the camera
  camera->setClippingPlanes(0.01, 10.0);
  
  // create a directional light source
  light = new cSpotLight(world);
  
  // insert light source inside world
  world->addChild(light);
  
  // enable light source
  light->setEnabled(true);
  light->setShadowMapEnabled(true);
  light->setShadowMapProperties(0.01, 10);
  light->m_shadowMap->setQualityVeryHigh();
  light->m_shadowMap->setCamera(camera);
  
  // define direction of light beam
  light->setLocalPos(cVector3d(0.5, 0.5, 0.5));
  light->setDir(-1.0, -0.5, -1.5);
  light->setSpotExponent(0.001);
  
  
  hands = devicePtrs;
    
  for (cGenericHapticDevicePtr hand : hands)
  {
    hand->open();
    hand->calibrate();
    
    // create the cursor to go along with the device
    SphereTool* cursor = new SphereTool();
    cursor->addToWorld(world);
    cursors.push_back(cursor);
    
    cHapticDeviceInfo info = hand->getSpecifications();
    
    if (info.m_sensedRotation == true)
      cursor->setUp();
    
    hand->setEnableGripperUserSwitch(true);
    
    cVector3d pos;
    hand->getPosition(pos);
    cursor->setPosition(pos);
    physics.initSphere(cursor);
  }
  
  //create 9 blocks
  for (int i = 0; i < (levels * 3); ++i)
//  for (int i = 0; i < 2; ++ i)
  {
    Block* b = new Block(scale);
    blocks.push_back(b);
    physics.initBlock(b);
    b->addToWorld(world);
  }
  
  Wall* floor = new Wall(1.0, 1.0, cVector3d(0.0, 0.0, 0.0), cVector3d(0.0, 0.0, 1.0), 0.0, "tiles.jpg");
  floor->addToWorld(world);
  physics.initWall(floor);
}

Game::~Game()
{
  for (cGenericHapticDevicePtr hand : hands)
    hand->close();
  
  for (SphereTool* cursor : cursors)
    delete cursor;
  
  for (Block* block : blocks)
    delete block;
  
  for (Wall* wall : room)
    delete wall;
  
  world->deleteAllChildren();
  delete world;
}


void Game::start()
{
  lock = false;
  reset();
}


void Game::testScene()
{
  Block* b1 = blocks[0];
  Block* b2 = blocks[1];
  cMatrix3d rotation = cMatrix3d(cVector3d(0, 0, 1),  M_PI_2);

  b1->setPosition(rotation * cVector3d(-b1->dimX * 1.02, 0, (2) * b1->dimZ * 1.1));
  b2->setPosition(           cVector3d( b1->dimX * 1.90, 0, (1) * b2->dimZ * 1.1));

  b1->setRotation(rotation);
  b2->setRotation(cMatrix3d(cVector3d(0, 0, 1),  0));
  
//  b2->physXBlock->setMassSpaceInertiaTensor(PxVec3(0,0,0));
}

void Game::reset()
{
  gameRunning = false;
  while (lock);
  
  timer.stop();
  timer.reset();
  
//  testScene();
  
  for (int i = 0; i < levels; ++i)
  {
    Block* b1 = blocks[(i * 3)];
    Block* b2 = blocks[(i * 3) + 1];
    Block* b3 = blocks[(i * 3) + 2];
    
    cMatrix3d rotation = cMatrix3d(cVector3d(0, 0, 1), (i % 4) * M_PI_2);
    
    b1->setPosition(rotation * cVector3d(-b1->dimX * 1.02, 0, (i + 1) * b1->dimZ * 1.1));
    b2->setPosition(           cVector3d(0,                0, (i + 1) * b2->dimZ * 1.1));
    b3->setPosition(rotation * cVector3d(b3->dimX * 1.02,  0, (i + 1) * b2->dimZ * 1.1));
    
    b1->setRotation(rotation);
    b2->setRotation(rotation);
    b3->setRotation(rotation);
  }
  
  disableInteraction();
  
  gameRunning = true;
  timer.start();
}

void Game::enableInteraction()
{
  for (SphereTool* cursor : cursors)
    cursor->enableInteraction();
  
  for (Block* block : blocks)
    block->enableInteraction();
  
  interactionEnabled = true;
}

void Game::disableInteraction()
{
  for (SphereTool* cursor : cursors)
    cursor->disableInteraction();
  
  for (Block* block : blocks)
    block->disableInteraction();
  
  interactionEnabled = false;
}

void Game::checkEnableInteraction()
{
  bool enable = true;
  
  for (cGenericHapticDevicePtr hand : hands)
  {
    bool b = false;
    hand->getUserSwitch(0, b);
    enable = enable && b;
  }
  
  if (enable)
    enableInteraction();
}

void Game::gameLoop()
{
  if(!gameRunning)
  {
    lock = false;
    while (!gameRunning);
    lock = true;
  }
    
  if (!interactionEnabled)
    checkEnableInteraction();
    
  
  /* Read the positions from the devices
   * Apply forces to the physx cursors
   */
  
  for (int i = 0; i < hands.size(); ++i)
  {
    cGenericHapticDevicePtr hand = hands[i];
    SphereTool* cursor = cursors[i];
    
    cVector3d position;
    hand->getPosition(position);
    position *= scale;
    
    cVector3d force = position - cursor->getPosition();
    force *= 100.0 / scale;
    
    cursor->applyForce(force);
  }
    
  while(timer.getCurrentTimeSeconds() < 0.001);
  timer.reset();
  
  physics.stepPhysics(0.001);
  
  /* update position and orienation of the scene objects
   * apply forces back to the devices
   */
  for (Block* block : blocks)
    block->update();
  
  for (int i = 0; i < hands.size(); ++i)
  {
    cGenericHapticDevicePtr hand = hands[i];
    SphereTool* cursor = cursors[i];
    
    cVector3d force(0, 0, 0);
    cVector3d torque(0, 0, 0);
    double gripperForce = 0.0;
    
    cursor->update();
    
    cVector3d position;
    hand->getPosition(position);
    position *= scale;
    
    force = cursor->getPosition() - position;
    force *= 100 / scale;
    
    hand->setForceAndTorqueAndGripperForce(force, torque, gripperForce);
  }
}

void Game::renderLoop(int width, int height)
{
  // update shadow maps (if any)
  world->updateShadowMaps(false, camera->getMirrorVertical());
  
  // render world
  camera->renderView(width, height);
}










