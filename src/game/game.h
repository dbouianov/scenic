#ifndef _GAME_H_
#define _GAME_H_

#include "config.h"
#include "factory.h"
#include "timer.h"
#include "font.h"
#include "mymenu.h"
#include "mymenuitem.h"
#include "keymap.h"
#include "algebra.h"
#include "light.h"
#include "camera.h"
#include "skybox.h"
#include "displist.h"
#include "primitive.h"
#include "groupnode.h"

NAMESPACE_BEGIN

//menu size
#define GAME_MENU_SIZE    10

//game flags
enum GameFlags
{
  GAME_FLAGS_NONE           = 0x00000000,
  GAME_FLAGS_ALL            = 0xffffffff,
  GAME_FLAGS_RUNNING        = 0x80000000,
  GAME_FLAGS_MOUSE          = 0x00000001,
  GAME_FLAGS_NOKEYREPS      = 0x00000002,
  GAME_FLAGS_MENU           = 0x00000004,
  GAME_FLAGS_STENCIL        = 0x00000008,
  GAME_FLAGS_REFLECT        = 0x00000010,
  GAME_FLAGS_SHADOWS        = 0x00000020,
  GAME_FLAGS_SKYBOX         = 0x00000040,
  GAME_FLAGS_NOFREECAM      = 0x00000800,
  GAME_FLAGS_EFFECTS        = 0x00000080,
  GAME_FLAGS_COLLISIONS     = 0x00000100,
  GAME_FLAGS_ANIMATION      = 0x00000200,
  GAME_FLAGS_PHYSICS        = 0x00000400,
};

//game
class Game
{
public:
  Game();
 ~Game();

  //run game
  int Run(int argc, char *argv[]);

  //GLUT event handlers
  void OnDrawScene    ();
  void OnCloseWindow  ();
  void OnChangeVisible(bool visible);
  void OnChangeSize   (int width, int height);
  void OnKey          (uchar key, bool down, int x, int y);
  void OnSysKey       (int   key, bool down, int x, int y);
  void OnMouseButton  (int button, bool down, int x, int y);
  void OnMouseWheel   (int wheel, int dir, int x, int y);
  void OnMouseMotion  (int x, int y);
  void OnMouseMotion2 (int x, int y);

  //game menu callbacks
  void OnToggleStencil    ();
  void OnToggleReflections();
  void OnToggleShadows    ();
  void OnToggleSkyBox     ();
  void OnToggleFreeCamera ();
  void OnToggleEffects    ();
  void OnToggleCollisions ();
  void OnToggleAnimation  ();
  void OnTogglePhysics    ();
  void OnTriggerExit      ();

private:
  //game configuration
  Config m_config;

  //game factory
  Factory m_factory;

  //game update timer
  Timer m_timer;

  //game camera
  Camera m_camera;

  //last camera position
  Point m_lastPosition;

  //flash light
  Light m_flashLight;

  //cross-hair for aiming
  Color     m_crossColor;
  CrossHair m_cross;

  //game font
  Font    m_font;
  Texture m_fontTexture;

  //game menu
  Menu    m_menuMain;
  Texture m_menuTexture;
  DisplayList m_menuIcon;

  //display list for scene data
  DisplayList m_sceneCache;

  //display list for shadows
  DisplayList m_shadowCache;

  //display list for scene terrain
  DisplayList m_terrainCache;

  //display list for scene mirrors
  DisplayList m_mirrorCache;

  //display list for skybox
  DisplayList m_skyCache;

  //shadow projection matrix
  Matrix m_shadowMatrix;

  //gravity vectors
  Vector m_gravitySpace;
  Vector m_gravityEarth;
  Vector m_gravityGame;
  Vector m_gravityRandom;

  //horizontal clipping plane
  GLdouble m_clipPlane [DIM_PLANE];

  //key states
  bool m_aKeyStates [NUM_KEYS];

  //characters corresponding to each action
  static uchar m_aKeyMap [NUM_ACTIONS];

  //game flags
  ulong m_gameFlags;

  //scene node flags
  ulong m_nodeFlags;

  //current frame
  ulong m_frame;

  //window center coordinates
  int m_centerX;
  int m_centerY;

  //mouse sampling and average weights
  float m_sampleWeightX;
  float m_sampleWeightY;
  float m_averageWeightX;
  float m_averageWeightY;

  //average mouse displacement
  float m_averageDx;
  float m_averageDy;

  //last mouse position
  float m_lastMouseX;
  float m_lastMouseY;

  //static scene data
  SceneNode *m_pScene;

#ifdef _DEBUG
  //scene bounding volumes
  SceneNode *m_pBounds;
#endif

  //static shadow data
  SceneNode *m_pShadows;

  //scene light
  SceneNode *m_pLight;

  //scene terrain
  SceneNode *m_pTerrain;

  //scene mirrors
  SceneNode *m_pMirrors;

  //scene effects
  SceneNode *m_pEffects;

  //scene skybox
  SceneNode *m_pSkyBox;

#if 0
  //player model
  Model *_pModel;
#endif

  //shadow map data
  /*uint _shadowMapWidth;
  uint _shadowMapHeight;
  GLuint _shadowMapTexture;
  Matrix _cameraProjMatrix;
  Matrix _cameraViewMatrix;
  Matrix _lightProjMatrix;
  Matrix _lightViewMatrix;*/

  //game menu strings
  static cchar *m_aMenuStrings [GAME_MENU_SIZE];

  //initialize game state
  bool InitState();

  //load game fonts
  bool LoadFonts(cchar *szFolder);

  //load game menus
  bool LoadMenus(cchar *szFolder);

  //load game objects
  bool LoadObjects();

  //process game input
  void ProcessInput(float dt);
  
  //update game state
  void UpdateState(float dt);

  //perform collision detection
  bool CheckCollisions();

  //draw game effects
  void WalkEffects(ulong flags);

  //draw game menu
  void WalkMenu();

  //randomize gravity
  static void RandomGravity(Vector oldGravity, OUT Vector newGravity);
};

//GLUT event handlers
void DrawScene    ();
void CloseWindow  ();
void ChangeVisible(int state);
void ChangeSize   (int width,  int height);
void KeyDown      (uchar key,  int x, int y);
void KeyUp        (uchar key,  int x, int y);
void SysKeyDown   (int   key,  int x, int y);
void SysKeyUp     (int   key,  int x, int y);
void MouseButton  (int button, int state, int x, int y);
void MouseWheel   (int wheel,  int dir,   int x, int y);
void MouseMotion  (int x, int y);

//game menu callbacks
void ToggleStencil    ();
void ToggleReflections();
void ToggleShadows    ();
void ToggleSkyBox     ();
void ToggleFreeCamera ();
void ToggleEffects    ();
void ToggleCollisions ();
void ToggleAnimation  ();
void TogglePhysics    ();
void TriggerExit      ();

//program entry point
int main(int argc, char *argv[]);

NAMESPACE_END

#endif //_GAME_H_
