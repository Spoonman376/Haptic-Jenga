//==============================================================================
/*
    \author    Your Name
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "PhysXMain.h"
#include "chai3d.h"
#include "Wall.h"
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
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;

//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;
double mouseOldX;
double mouseOldY;


// a light source to illuminate the objects in the world
cDirectionalLight *light;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device

vector<cGenericHapticDevicePtr> hands;
vector<SphereTool*> cursors;

SphereTool* cursorLeft;
SphereTool* cursorRight;

// a label to display the rates [Hz] at which the simulation is running
cLabel* labelRates;

// a small sphere (cursorRight) representing the haptic device 

Block* block;
Wall* floorWall;
PhysXMain* physics;

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
  // WORLD - CAMERA - LIGHTING
  //--------------------------------------------------------------------------

  // create a new world.
  world = new cWorld();

  // set the background color of the environment
  world->m_backgroundColor.setBlack();

  // create a camera and insert it into the virtual world
  camera = new cCamera(world);
  world->addChild(camera);

  // position and orient the camera
  camera->set( cVector3d (0.25, 0.0, 0.1),    // camera position (eye)
               cVector3d (0.0, 0.0, 0.0),    // look at position (target)
               cVector3d (0.0, 0.0, 1.0));   // direction of the (up) vector

  // set the near and far clipping planes of the camera
  camera->setClippingPlanes(0.01, 10.0);

  // set stereo mode
  camera->setStereoMode(stereoMode);

  // set stereo eye separation and focal length (applies only if stereo is enabled)
  camera->setStereoEyeSeparation(0.01);
  camera->setStereoFocalLength(0.5);

  // set vertical mirrored display mode
  camera->setMirrorVertical(mirroredDisplay);

  // create a directional light source
  light = new cDirectionalLight(world);
  
  // insert light source inside world
  world->addChild(light);

  // enable light source
  light->setEnabled(true);

  // define direction of light beam
  light->setDir(-1.0, -0.2, -0.2);
  
  block = new Block();
  block->setPosition(cVector3d(0,0,0.1));
  block->addToWorld(world);
  
  floorWall = new Wall(1.0, 1.0, cVector3d(0.0, 0.0, 0.0), cVector3d(0.0, 0.0, 1.0), 0.0, "tiles.jpg");
  floorWall->addToWorld(world);
  
  physics = new PhysXMain();
  physics->initScene();
  physics->initBlock(block);


  //--------------------------------------------------------------------------
  // HAPTIC DEVICE
  //--------------------------------------------------------------------------

  // create a haptic device handler
  handler = new cHapticDeviceHandler();

  cGenericHapticDevicePtr rightHand;
  cGenericHapticDevicePtr leftHand;
  
  // get a handle to the haptic devices
  if(handler->getDevice(rightHand, 0))
    hands.push_back(rightHand);
  
  if(handler->getDevice(leftHand, 1))
    hands.push_back(leftHand);
  
  for (cGenericHapticDevicePtr hand : hands)
  {
    hand->open();
    hand->calibrate();
    
    // create the cursor to go along with the device
    SphereTool* cursor = new SphereTool();
    cursor->addToWorld(world);
    physics->initSphere(cursor);
    cursors.push_back(cursor);
    
    cHapticDeviceInfo info = hand->getSpecifications();
    
    if (info.m_sensedRotation == true)
      cursor->setUp();
    
    hand->setEnableGripperUserSwitch(true);
  }
  

  //--------------------------------------------------------------------------
  // WIDGETS
  //--------------------------------------------------------------------------

  // create a font
  cFontPtr font = NEW_CFONTCALIBRI20();
  
  // create a label to display the haptic and graphic rates of the simulation
  labelRates = new cLabel(font);
  labelRates->m_fontColor.setWhite();
  camera->m_frontLayer->addChild(labelRates);


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
  double azu = camera->getSphericalAzimuthRad();
  double alt = camera->getSphericalPolarRad();
  double r = camera->getSphericalRadius();
  
  r = (y > 0) ? r - 0.05 : r + 0.05;
  r = (r < 0.05) ? 0.05 : r;
  camera->setSphericalRadius(r);
  camera->setSphericalAzimuthRad(azu);
  camera->setSphericalPolarRad(alt);
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
  
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
  {
    double azu = camera->getSphericalAzimuthRad();
    double alt = camera->getSphericalPolarRad();
    double r = camera->getSphericalRadius();
    
    camera->setSphericalAzimuthRad(azu - dx * 0.0025);
    camera->setSphericalPolarRad(alt - dy * 0.0025);
    camera->setSphericalRadius(r);
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
      camera->setMirrorVertical(mirroredDisplay);
  }
}

//------------------------------------------------------------------------------

void close(void)
{
  // stop the simulation
  simulationRunning = false;

  // wait for graphics and haptics loops to terminate
  while (!simulationFinished) { cSleepMs(100); }

  // close haptic device
  for (cGenericHapticDevicePtr hand : hands)
    hand->close();
  
  // delete resources
  delete hapticsThread;
  delete world;
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

  // update shadow maps (if any)
  world->updateShadowMaps(false, mirroredDisplay);

  // render world
  camera->renderView(width, height);

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
  
  cPrecisionClock timer;
  timer.start(true);

  // main haptic simulation loop
  while(simulationRunning)
  {

    /* Read the positions from the devices
     * Apply forces to the physx cursors
     */
    
    vector<cVector3d> positions;
    
    for (int i = 0; i < hands.size(); ++i)
    {
      cGenericHapticDevicePtr hand = hands[i];
      SphereTool* cursor = cursors[i];
      
      cVector3d position;
      hand->getPosition(position);
      positions.push_back(position);
      
      
      cVector3d force = position - cursor->getPosition();
      force *= 20;
      
      cursor->applyForce(force);
    }
    
    // read user-switch status (button 0)
    bool button = false;
    //rightHand->getUserSwitch(0, button);
  
    double t = timer.getCurrentTimeSeconds();
    timer.reset();
    
    physics->stepPhysics(t);
    
    /* update position and orienation of the scene objects
     * apply forces back to the devices
     */
    
    // for (Block* block : blocks)
    block->update();
    
    for (int i = 0; i < hands.size(); ++i)
    {
      cGenericHapticDevicePtr hand = hands[i];
      SphereTool* cursor = cursors[i];

      cVector3d force(0, 0, 0);
      cVector3d torque(0, 0, 0);
      double gripperForce = 0.0;
  
      cursor->update();
      
      force = cursor->getPosition() - positions[i];
      force *= 20;
      
      hand->setForceAndTorqueAndGripperForce(force, torque, gripperForce);
    }
    
    // signal frequency counter
    freqCounterHaptics.signal(1);
  }
  
  // exit haptics thread
  simulationFinished = true;
}

//------------------------------------------------------------------------------
