 //
//

#include "Game.h"


Game::Game(vector<cGenericHapticDevicePtr> devicePtrs)
{
  
  // Initial chai3d world set up
  world = new cWorld();
  camera = new cCamera(world);
  world->addChild(camera);
  
  camera->setSphericalRad(0.5, M_PI / 3.0, M_PI / 2.0);
  
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
  
  // define direction and position of light
  light->setLocalPos(cVector3d(0.5, 0.5, 3.0) * 2.5);
  light->setDir(-0.5, -0.5, -3.0);
  light->setSpotExponent(0.001);
  light->setCutOffAngleDeg(4);

  hands = devicePtrs;
  cursor = nullptr;
  gripper = nullptr;
    
  for (int i = 0; i < hands.size(); ++i)
  {
    cGenericHapticDevicePtr hand = hands[i];
    
    hand->open();
    hand->calibrate();
    
    // create the cursor to go along with the device
    if (i == 0) {
      gripper = new GripperTool(hand, scale);
      gripper->addToWorld(world);
      physics.initGripper(gripper);
    }
    else {
      cursor = new SphereTool(hand, scale);
      cursor->addToWorld(world);
      physics.initSphere(cursor);
    }
  }
  
  //create 9 blocks
  Block::dimX *= scale;
  Block::dimY *= scale;
  Block::dimZ *= scale;
  
  for (int i = 0; i < (levels * 3); ++i)
//  for (int i = 0; i < 2; ++ i)
  {
    Block* b = new Block(scale);
    blocks.push_back(b);
    b->addToWorld(world);
  }
  
  physics.initBlock(blocks);

  
  Wall* floor = new Wall(0.5, 0.5, cVector3d(0.0, 0.0, 0.0), cVector3d(0.0, 0.0, 1.0), 0.0, "tiles.jpg");
  floor->addToWorld(world);
  physics.initWall(floor);
}

Game::~Game()
{
  for (cGenericHapticDevicePtr hand : hands)
    hand->close();
  
  if (cursor != nullptr)
    delete cursor;
  
  if (gripper != nullptr)
    delete gripper;
  
  for (Block* block : blocks)
    delete block;
  
  for (Wall* wall : room)
    delete wall;
  
  world->deleteAllChildren();
  delete world;
}


void Game::start()
{
  reset();
}


void Game::testScene()
{
  Block* b1 = blocks[0];
  Block* b2 = blocks[1];
  cMatrix3d rotation = cMatrix3d(cVector3d(0, 0, 1),  M_PI_2);

  b1->setPosition(cVector3d(0.02, 0, 0.075));
  b2->setPosition(cVector3d(-0.02, 0, 0.025));

  b1->setRotation(rotation);
  b2->setRotation(cMatrix3d(cVector3d(0, 0, 1),  0));
  
//  b2->physXBlock->setMassSpaceInertiaTensor(PxVec3(0,0,0));
}

void Game::reset()
{
  lock.lock();

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

  blocks[0]->physXBlock->addForce(PxVec3(0,0,20000));
  
  enableInteraction(false);
  
  timer.start();
  lock.unlock();
}

void Game::enableInteraction(bool b)
{
  if (cursor != nullptr)
    cursor->enableInteraction(b);

  if (gripper != nullptr)
    gripper->enableInteraction(b);

  for (Block* block : blocks)
    block->enableInteraction(b);
  
  interactionEnabled = b;
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
    enableInteraction(true);
}

void Game::gameLoop()
{
  lock.lock();

  if (!interactionEnabled)
    checkEnableInteraction();
    
  
  /* Read the positions from the devices
   * Apply forces to the physx cursors
   */
  
  if (cursor != nullptr)
    cursor->applyForce(camera);
  
  if (gripper != nullptr)
    gripper->applyForce(camera);
  
  if (hands.size() > 0)
  {
    int cameraMove = 0;
    for (cGenericHapticDevicePtr hand : hands)
    {
      cVector3d pos;
      hand->getPosition(pos);
      pos.normalize();
      double result = pos * cVector3d(0,0,1);
      if (result > 0.8)
        ++cameraMove;
      if (result < -0.7)
        --cameraMove;
    }
    
    if (abs(cameraMove) == 2)
    {
      cVector3d pos = camera->getSphericalOriginReference();
      (cameraMove == 2) ? pos.z(pos.z() + 0.000075) : pos.z(pos.z() - 0.000075);
      camera->setSphericalOriginReference(pos);
    }
      
  }
  
//  while(timer.getCurrentTimeSeconds() < 0.001);
//  timer.reset();

  physics.stepPhysics(0.001);
  
  /* update position and orienation of the scene objects
   * apply forces back to the devices
   */
  for (Block* block : blocks)
    block->update();
  
  if (cursor != nullptr) {
    cursor->update();
    cursor->applyForceToDevice(camera);
  }

  if (gripper != nullptr) {
    gripper->update();
    gripper->applyForceToDevice(camera);
  }

  lock.unlock();
}

void Game::renderLoop(int width, int height)
{
  // update shadow maps (if any)
  world->updateShadowMaps(false, camera->getMirrorVertical());
  
  // render world
  camera->renderView(width, height);
}










