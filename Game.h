//
//

#ifndef Game_h
#define Game_h

#include <stdio.h>
#include "Block.h"
#include "SphereTool.h"
#include "Wall.h"
#include "PhysXMain.h"

class Game {
private:
  
  vector<cGenericHapticDevicePtr> hands;
  vector<SphereTool*> cursors;

  vector<Block*> blocks;
  vector<Wall*> room;
  
  PhysXMain physics;
  
  bool started;
  cPrecisionClock timer;
  
  int levels = 5;
  
public:
  
  bool simulationRunning;
  bool simulationFinished;
  
  Game(vector<cGenericHapticDevicePtr>, cWorld*);
  ~Game();
  
  void reset();
  void setup();
  void start();
  void gameLoop();
};

#endif /* Game_h */
