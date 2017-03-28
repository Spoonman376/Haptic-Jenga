//
//

#include "Game.h"


Game::Game(vector<cGenericHapticDevicePtr> devicePtrs, cWorld* world)
{
  hands = devicePtrs;
  
  for (cGenericHapticDevicePtr hand : hands)
  {
    hand->open();
    hand->calibrate();
    
    // create the cursor to go along with the device
    SphereTool* cursor = new SphereTool();
    cursor->addToWorld(world);
    //physics.initSphere(cursor);
    cursors.push_back(cursor);
    
    cHapticDeviceInfo info = hand->getSpecifications();
    
    if (info.m_sensedRotation == true)
      cursor->setUp();
    
    hand->setEnableGripperUserSwitch(true);
  }
  
  
  //create 9 blocks
  for (int i = 0; i < 9; ++i)
  {
    blocks.push_back(new Block());
  }
  
}

Game::~Game()
{
  
}





void Game::start()
{

}









