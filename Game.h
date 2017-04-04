//
//

#ifndef Game_h
#define Game_h

#include <stdio.h>
#include "Block.h"
#include "SphereTool.h"
#include "Wall.h"
#include "PhysXMain.h"
#include "Tool.h"

class Game {
private:
  
  vector<cGenericHapticDevicePtr> hands;
  vector<SphereTool*> cursors;

  vector<Block*> blocks;
  vector<Wall*> room;
  
  PhysXMain physics;
  
  cPrecisionClock timer;
  
  bool lock;
  bool gameRunning;
  bool interactionEnabled;
  int levels = 5;
  
  void checkEnableInteraction();
  void testScene();
  
public:
  cWorld* world;
  cCamera* camera;
  
  cVector3d cameraLookAt;
  
  Game(vector<cGenericHapticDevicePtr>, cWorld*);
  ~Game();
  
  void start();
  void reset();
  void gameLoop();
  
  void renderLoop(int width, int height);
  
  
  void enableInteraction();
  void disableInteraction();
};

#endif /* Game_h */
