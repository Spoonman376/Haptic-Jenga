//==============================================================================
/*
    \author    Erik Spooner
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "chai3d.h"
#include "Game.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
//------------------------------------------------- -----------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

// stereo Mode
/*
    C_STEREO_DISABLED:            Stereo is disabled 
    C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
    C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
    C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = true;

// mirrored display
bool mirroredDisplay = false;

//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

double mouseOldX;
double mouseOldY;

Game* game;

// a haptic device handler
cHapticDeviceHandler* handler;

// a label to display the rates [Hz] at which the simulation is running
cLabel* labelRates;

// flag to indicate if the haptic simulation currently running
bool simulationRunning = false;

// flag to indicate if the haptic simulation has terminated
bool simulationFinished = false;

// a frequency counter to measure the simulation graphic rate
cFrequencyCounter freqCounterGraphics;

// a frequency counter to measure the simulation haptic rate
cFrequencyCounter freqCounterHaptics;

// haptic thread
cThread* hapticsThread;

// a handle to window display context
GLFWwindow* window = NULL;

// current width of window
int width  = 0;

// current height of window
int height = 0;

// swap interval for the display context (vertical synchronization)
int swapInterval = 1;


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);
void motion(GLFWwindow* window, double x, double y);
void mouse(GLFWwindow* window, int button, int action, int mods);
void scrollCallback(GLFWwindow* window, double x, double y);

// this function renders the scene
void updateGraphics(void);

// this function contains the main haptics simulation loop
void updateHaptics(void);

// this function closes the application
void close(void);


//==============================================================================
/*
    TEMPLATE:    application.cpp

    Description of your application.
*/
//==============================================================================

int main(int argc, char* argv[])
{
  //--------------------------------------------------------------------------
  // INITIALIZATION
  //--------------------------------------------------------------------------

  cout << endl;
  cout << "-----------------------------------" << endl;
  cout << "CHAI3D" << endl;
  cout << "-----------------------------------" << endl << endl << endl;
  cout << "Keyboard Options:" << endl << endl;
  cout << "[f] - Enable/Disable full screen mode" << endl;
  cout << "[m] - Enable/Disable vertical mirroring" << endl;
  cout << "[q] - Exit application" << endl;
  cout << endl << endl;


  //--------------------------------------------------------------------------
  // OPENGL - WINDOW DISPLAY
  //--------------------------------------------------------------------------

  // initialize GLFW library
  if (!glfwInit())
  {
      cout << "failed initialization" << endl;
      cSleepMs(1000);
      return 1;
  }

  // set error callback
  glfwSetErrorCallback(errorCallback);

  // compute desired size of window
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int w = 0.8 * mode->height;
  int h = 0.5 * mode->height;
  int x = 0.5 * (mode->width - w);
  int y = 0.5 * (mode->height - h);

  // set OpenGL version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  // set active stereo mode
  if (stereoMode == C_STEREO_ACTIVE)
  {
      glfwWindowHint(GLFW_STEREO, GL_TRUE);
  }
  else
  {
      glfwWindowHint(GLFW_STEREO, GL_FALSE);
  }

  // create display context
  window = glfwCreateWindow(w, h, "CHAI3D", NULL, NULL);
  if (!window)
  {
      cout << "failed to create window" << endl;
      cSleepMs(1000);
      glfwTerminate();
      return 1;
  }

  // get width and height of window
  glfwGetWindowSize(window, &width, &height);

  // set position of window
  glfwSetWindowPos(window, x, y);

  // set key callback
  glfwSetKeyCallback(window, keyCallback);
  
  glfwSetMouseButtonCallback(window, mouse);
  
  glfwSetCursorPosCallback(window, motion);
  
  glfwSetScrollCallback(window, scrollCallback);
  
  // set resize callback
  glfwSetWindowSizeCallback(window, windowSizeCallback);

  // set current display context
  glfwMakeContextCurrent(window);

  // sets the swap interval for the current display context
  glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
  glfwSwapInterval(swapInterval);

#ifdef GLEW_VERSION
  // initialize GLEW library
  if (glewInit() != GLEW_OK)
  {
      cout << "failed to initialize GLEW library" << endl;
      glfwTerminate();
      return 1;
  }
#endif

  //--------------------------------------------------------------------------
  // HAPTIC DEVICE
  //--------------------------------------------------------------------------

  vector<cGenericHapticDevicePtr> hands;
  
  // create a haptic device handler
  handler = new cHapticDeviceHandler();

  cGenericHapticDevicePtr rightHand;
  cGenericHapticDevicePtr leftHand;
  
  // get a handle to the haptic devices
  //handler->getDevice(rightHand, 0);
  if(handler->getDevice(rightHand, 0))
    hands.push_back(rightHand);
  
  //handler->getDevice(leftHand, 1);
  if(handler->getDevice(leftHand, 1))
    hands.push_back(leftHand);
  
  game = new Game(hands);

  //--------------------------------------------------------------------------
  // WIDGETS
  //--------------------------------------------------------------------------

  // create a font
  cFontPtr font = NEW_CFONTCALIBRI20();
  
  // create a label to display the haptic and graphic rates of the simulation
  labelRates = new cLabel(font);
  labelRates->m_fontColor.setWhite();
  game->camera->m_frontLayer->addChild(labelRates);

  //--------------------------------------------------------------------------
  // START SIMULATION
  //--------------------------------------------------------------------------

  // create a thread which starts the main haptics rendering loop
  hapticsThread = new cThread();
  hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

  // setup callback when application exits
  atexit(close);


  //--------------------------------------------------------------------------
  // MAIN GRAPHIC LOOP
  //--------------------------------------------------------------------------

  // call window size callback at initialization
  windowSizeCallback(window, width, height);

  // main graphic loop
  while (!glfwWindowShouldClose(window))
  {
    // get width and height of window
    glfwGetWindowSize(window, &width, &height);

    // render graphics
    updateGraphics();

    // swap buffers
    glfwSwapBuffers(window);

    // process events
    glfwPollEvents();

    // signal frequency counter
    freqCounterGraphics.signal(1);
  }

  // close window
  glfwDestroyWindow(window);

  // terminate GLFW library
  glfwTerminate();

  // exit
  return 0;
}

//------------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
  // update window size
  width  = a_width;
  height = a_height;
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char* a_description)
{
  cout << "Error: " << a_description << endl;
}

//------------------------------------------------------------------------------


void scrollCallback(GLFWwindow* window, double x, double y)
{
  double azu = game->camera->getSphericalAzimuthRad();
  double alt = game->camera->getSphericalPolarRad();
  double r = game->camera->getSphericalRadius();
  
  r = (y > 0) ? r - 0.05 : r + 0.05;
  r = (r < 0.05) ? 0.05 : r;
  game->camera->setSphericalRadius(r);
  game->camera->setSphericalAzimuthRad(azu);
  game->camera->setSphericalPolarRad(alt);
}


void mouse(GLFWwindow* window, int button, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    mouseOldX = x;
    mouseOldY = y;
  }
}

void motion(GLFWwindow* window, double x, double y)
{
  double dx, dy;
  dx = x - mouseOldX;
  dy = y - mouseOldY;
  
  mouseOldX = x;
  mouseOldY = y;
  
  if (game->interactionEnabled)
    game->enableInteraction(false);
  
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
  {
    double azu = game->camera->getSphericalAzimuthRad();
    double alt = game->camera->getSphericalPolarRad();
    double r = game->camera->getSphericalRadius();
    
    game->camera->setSphericalAzimuthRad(azu - dx * 0.0025);
    game->camera->setSphericalPolarRad(alt - dy * 0.0025);
    game->camera->setSphericalRadius(r);
  }
}



void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
  // filter calls that only include a key press
  if (a_action != GLFW_PRESS)
  {
      return;
  }

  // option - exit
  else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q))
  {
      glfwSetWindowShouldClose(a_window, GLFW_TRUE);
  }

  // option - toggle fullscreen
  else if (a_key == GLFW_KEY_F)
  {
    // toggle state variable
    fullscreen = !fullscreen;

    // get handle to monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    // get information about monitor
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // set fullscreen or window mode
    if (fullscreen)
    {
      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
      glfwSwapInterval(swapInterval);
    }
    else
    {
      int w = 0.8 * mode->height;
      int h = 0.5 * mode->height;
      int x = 0.5 * (mode->width - w);
      int y = 0.5 * (mode->height - h);
      glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
      glfwSwapInterval(swapInterval);
    }
  }
  // option - toggle vertical mirroring
  else if (a_key == GLFW_KEY_M)
  {
      mirroredDisplay = !mirroredDisplay;
      game->camera->setMirrorVertical(mirroredDisplay);
  }
  else if (a_key == GLFW_KEY_R)
  {
    game->reset();
  }
  
}

//------------------------------------------------------------------------------

void close(void)
{
  // stop the simulation
  simulationRunning = false;

  // wait for graphics and haptics loops to terminate
  while (!simulationFinished) { cSleepMs(100); }

  delete game;
  
  // delete resources
  delete hapticsThread;
  delete handler;
}

//------------------------------------------------------------------------------

void updateGraphics(void)
{
  /////////////////////////////////////////////////////////////////////
  // UPDATE WIDGETS
  /////////////////////////////////////////////////////////////////////

  // update haptic and graphic rate data
  labelRates->setText(cStr(freqCounterGraphics.getFrequency(), 0) + " Hz / " +
      cStr(freqCounterHaptics.getFrequency(), 0) + " Hz");

  // update position of label
  labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 15);

  /////////////////////////////////////////////////////////////////////
  // RENDER SCENE
  /////////////////////////////////////////////////////////////////////

  game->renderLoop(width, height);
  
  // wait until all GL commands are completed
  glFinish();

  // check for any OpenGL errors
  GLenum err;
  err = glGetError();
  if (err != GL_NO_ERROR) cout << "Error:  %s\n" << gluErrorString(err);
}

//------------------------------------------------------------------------------

void updateHaptics(void)
{
  // simulation in now running
  simulationRunning  = true;
  simulationFinished = false;
  
  game->start();
    
  // main haptic simulation loop
  while(simulationRunning)
  {    
    game->gameLoop();
    
    // signal frequency counter
    freqCounterHaptics.signal(1);
  }
  
  // exit haptics thread
  simulationFinished = true;
}

//------------------------------------------------------------------------------
