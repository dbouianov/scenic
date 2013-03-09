#include "game.h"
#include "loader.h"

#if defined(__APPLE__) || defined(MACOSX)
//mac glut
#include <GLUT/glut.h>
#define glutCloseFunc   glutWMCloseFunc
#define glutMouseWheelFunc(x)
#else
//free/open glut
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#endif

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_MAIN, "Game",__FILE__);

//single game instance
static Game g_game;

Game::Game()
: m_gameFlags      (GAME_FLAGS_MOUSE
                  | GAME_FLAGS_RUNNING
#if 1
                  | GAME_FLAGS_NOKEYREPS
#endif
                  | GAME_FLAGS_STENCIL
                  | GAME_FLAGS_REFLECT
                  | GAME_FLAGS_SHADOWS
                  | GAME_FLAGS_SKYBOX
                  | GAME_FLAGS_EFFECTS
                  | GAME_FLAGS_COLLISIONS
                  | GAME_FLAGS_ANIMATION
                  | GAME_FLAGS_PHYSICS
#ifndef _DEBUG
                  | GAME_FLAGS_NOFREECAM
#endif
                   ),
  m_nodeFlags      (NODE_FLAGS_ALL
#ifdef _DEBUG
                  | NODE_FLAGS_HALFLOD
                  | NODE_FLAGS_BOUNDS
#endif
                   ),
  m_frame          (0),
  m_centerX        (0),
  m_centerY        (0),
  m_sampleWeightX  (0.0f),
  m_sampleWeightY  (0.0f),
  m_averageWeightX (0.0f),
  m_averageWeightY (0.0f),
  m_averageDx      (0.0f),
  m_averageDy      (0.0f),
  m_lastMouseX     (0.0f),
  m_lastMouseY     (0.0f),
  m_pScene         (NULL),
#ifdef _DEBUG
  m_pBounds        (NULL),
#endif
  m_pShadows       (NULL),
  m_pLight         (NULL),
  m_pMirrors       (NULL),
  m_pEffects       (NULL),
  m_pSkyBox        (NULL)
{
  //initialize configuration
  ConfigInit(&m_config);

  //initialize cross-hair color
  ColorInit(m_crossColor, DEFAULT_CROSSHAIR_COLOR);

  //initialize gravity vectors
  VectorZero(m_gravitySpace);
  VectorInit(m_gravityEarth, 0.0f, -DEFAULT_EARTH_GRAVITY, 0.0f);

  //zero out key states
  MemZero(m_aKeyStates, sizeof(m_aKeyStates));

  //initialize clipping plane
  m_clipPlane[CX] = 0.0;
  m_clipPlane[CY] = 1.0;
  m_clipPlane[CZ] = 0.0;
  m_clipPlane[CW] = 0.0;

  //seed random number generator
  RandomSeed();
}

Game::~Game()
{}

int Game::Run(int argc, char *argv[])
{
  //form config file name
  strncpy(m_config.szConfigFile, argv[0], MAX_FILE_NAME_SIZE);
  strncat(m_config.szConfigFile, ".cfg",
    MAX_FILE_NAME_SIZE - strlen(m_config.szConfigFile));

  //load configuration
  ShowMessage("Loading %s", m_config.szConfigFile);
  ConfigLoad(&m_config, m_config.szConfigFile);

  //command-line arguments supersede settings in config file
  ConfigParse(&m_config, argc, argv);
  ConfigPrint(&m_config, stdout);

  //initialize GLUT
  glutInit(&argc, argv);

  //initialize display mode
  //glutInitDisplayString("stencil>=1 rgba depth double");
  glutInitDisplayMode(GLUT_STENCIL | GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

  //initialize window parameters
  glutInitWindowSize(m_config.width, m_config.height);
  glutInitWindowPosition(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y);

  //create GLUT window
  glutCreateWindow(DEFAULT_WINDOW_TITLE);

  //set event handlers
  glutDisplayFunc      (DrawScene);
  glutCloseFunc        (CloseWindow);
  glutReshapeFunc      (ChangeSize);
  glutVisibilityFunc   (ChangeVisible);
  glutKeyboardFunc     (KeyDown);
  glutKeyboardUpFunc   (KeyUp);
  glutSpecialFunc      (SysKeyDown);
  glutSpecialUpFunc    (SysKeyUp);
  glutMouseFunc        (MouseButton);
  glutMouseWheelFunc   (MouseWheel);
  glutMotionFunc       (MouseMotion);
  glutPassiveMotionFunc(MouseMotion);

  //initialize game state
  InitState();

  //load game objects
  LoadObjects();

  if (m_gameFlags & GAME_FLAGS_NOKEYREPS)
  {
    //turn off key repeats
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
  }

  //turn off cursor
  glutSetCursor(GLUT_CURSOR_NONE);

  //enable full screen mode if necessary
  if (m_config.flags & CONFIG_FLAGS_FULLSCREEN)
  {
    //turn on full screen mode
    glutFullScreen();
  }
  else
  {
    //set window title to scene name
    glutSetWindowTitle(m_pScene->GetName());
  }

  //start game timer
  m_timer.Start();

  //start game loop
  glutMainLoop();
  /*while (m_gameFlags & GAME_FLAGS_RUNNING)
  {
    glutCheckLoop();
  }*/

  //restore window manager state
  //OnCloseWindow();

  return 0;
}

bool Game::InitState()
{
  //specify depth and stencil buffer clear values
  glClearDepth(DEFAULT_CLEAR_DEPTH);
  glClearStencil(DEFAULT_CLEAR_STENCIL);

  //specify point size and line width
  glPointSize(DEFAULT_POINT_SIZE);
  glLineWidth(DEFAULT_LINE_WIDTH);

  //enable back face culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  //specify polygon mode
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  //specify shading model
  glShadeModel(GL_SMOOTH);

  //enable anti-aliasing for lines
  glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
  glEnable(GL_LINE_SMOOTH);

#if 0
  //enable anti-aliasing for polylgons
  glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  glEnable(GL_POLYGON_SMOOTH);
#endif

  //specify background color
  glClearColor(DEFAULT_CLEAR_COLOR);

  //enable current color tracking material color
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  //specify lighting model
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

  //add ambient color everywhere in the scene
  Color ambient = {DEFAULT_AMBIENT_COLOR};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

  //enable alpha blending
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  //enable automatic texture coordinate generation
  //glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  //glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  //glEnable(GL_TEXTURE_GEN_S);
  //glEnable(GL_TEXTURE_GEN_T);

  //perspective correction
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

#ifndef _DEBUG
  //enable exponential fog
  Color fog = {DEFAULT_FOG_COLOR};
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_START, DEFAULT_FOG_START);
  glFogf(GL_FOG_END, DEFAULT_FOG_END);
  glFogf(GL_FOG_DENSITY, DEFAULT_FOG_DENSITY);
  glFogfv(GL_FOG_COLOR, fog);
  glHint(GL_FOG_HINT, GL_NICEST);
#endif

  //enable re-normalization of scaled normals
  glEnable(GL_NORMALIZE);

  //set pixel pack/unpack alignment
  glPixelStorei(GL_UNPACK_ALIGNMENT, DEFAULT_PIXEL_ALIGNMENT);
  glPixelStorei(GL_PACK_ALIGNMENT, DEFAULT_PIXEL_ALIGNMENT);

  //initialize projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glPushMatrix();

  //initialize modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return true;
}

bool Game::LoadObjects()
{
  //load skybox
  ShowMessage("Loading skybox");
  m_pSkyBox = LoadSkyBox(m_config.szSkyFolder, m_factory, m_factory);
  if (!m_pSkyBox)
  {
    ShowMessage("Unable to load skybox");
    //replace skybox with null node
    m_pSkyBox = m_factory.GetNullNode();
  }
  //cache skybox
  else if (m_skyCache.Create(m_pSkyBox))
  {
    //replace skybox with display list
    m_pSkyBox = &m_skyCache;
  }

  //load light
  ShowMessage("Loading light");
  m_pLight = LoadLuaFile(m_config.szSkyFolder, DEFAULT_LIGHT_FILE, m_factory, m_factory);
  if (!m_pLight)
  {
    ShowMessage("Unable to load light");
    //replace light with null node
    m_pLight = m_factory.GetNullNode();
    m_gameFlags &= ~GAME_FLAGS_SHADOWS;
  }
  else
  {
    //apply light
    m_pLight->Walk();
    Light *pLight = dynamic_cast<Light *>(m_pLight);
    if (pLight)
    {
      //calculate shadow projection matrix
      Plane shadowPlane = PLANE_ZX;
      MatrixShadow(m_shadowMatrix, shadowPlane, pLight->GetPosition());
    }
    else
    {
      //turn off shadows
      m_gameFlags &= ~GAME_FLAGS_SHADOWS;
      MatrixIdentity(m_shadowMatrix);
    }
  }

  //load scene
  ShowMessage("Loading scene");
  m_pScene = LoadLuaFile(m_config.szSceneFolder, DEFAULT_SCENE_FILE, m_factory, m_factory);
  if (!m_pScene)
  {
    ShowMessage("Unable to load scene");
    //replace scene with null node
    m_pScene = m_factory.GetNullNode();
    //replace shadows with null node
    m_pShadows = m_factory.GetNullNode();
#ifdef _DEBUG
    //replace bounding volumes with null node
    m_pBounds = m_factory.GetNullNode();
#endif
  }
  //cache scene data
  else if (m_sceneCache.Create(m_pScene))
  {
    if (m_gameFlags & GAME_FLAGS_SHADOWS)
    {
      //cache shadows
      if (m_shadowCache.Create(m_pScene, NODE_FLAGS_PRIMITIVE))
      {
        //replace shadows with shadow display list
        m_pShadows = &m_shadowCache;
      }
      else
      {
        //replace shadows with scene display list
        m_pShadows = &m_sceneCache;
      }
    }
#ifdef _DEBUG
    //find scene bounding volumes
    Bounds bounds;
    m_pScene->GetBounds(bounds);
    m_pBounds = m_pScene;
#endif
    //replace scene with display list
    m_sceneCache.SetName(m_pScene->GetName());
    m_pScene = &m_sceneCache;
  }
  else
  {
    //replace shadows with scene
    m_pShadows = m_pScene;
  }

  //load terrain
  ShowMessage("Loading terrain");
  m_pTerrain = LoadLuaFile(m_config.szSceneFolder, DEFAULT_TERRAIN_FILE, m_factory, m_factory);
  if (!m_pTerrain)
  {
    ShowMessage("Unable to load terrain");
    //replace terrain with null node
    m_pTerrain = m_factory.GetNullNode();
  }
  //cache terrain data
  else if (m_terrainCache.Create(m_pTerrain))
  {
    //replace terrain with display list
    m_pTerrain = &m_terrainCache;
  }

  //load mirrors
  ShowMessage("Loading mirrors");
  m_pMirrors = LoadLuaFile(m_config.szSceneFolder, DEFAULT_MIRRORS_FILE, m_factory, m_factory);
  if (!m_pMirrors)
  {
    ShowMessage("Unable to load mirrors");
    //replace mirrors with null node
    m_pMirrors = m_factory.GetNullNode();
  }
  //cache mirror data
  else if (m_mirrorCache.Create(m_pMirrors))
  {
    //replace mirrors with display list
    m_pMirrors = &m_mirrorCache;
  }

  //load effects
  ShowMessage("Loading effects");
  m_pEffects = LoadLuaFile(m_config.szSceneFolder, DEFAULT_EFFECTS_FILE, m_factory, m_factory);
  if (!m_pEffects)
  {
    ShowMessage("Unable to load effects");
    //replace effects with null node
    m_pEffects = m_factory.GetNullNode();
  }

  //load model
  //m_pModel = LoadLuaModel(m_config.szModelFolder, m_factory);

  //load fonts
  ShowMessage("Loading fonts");
  if (!LoadFonts(m_config.szSkinFolder))
  {
    ShowMessage("Unable to load fonts, loading default");
    LoadFonts(DEFAULT_SKIN_FOLDER);
  }

  //load menus
  ShowMessage("Loading menus");
  if (!LoadMenus(m_config.szSkinFolder))
  {
    ShowMessage("Unable to load menus, loading default");
    LoadMenus(DEFAULT_SKIN_FOLDER);
  }

  //set default camera parameters
  m_camera.SetViewFrustum(DEFAULT_FIELD_OF_VIEW, DEFAULT_ASPECT_RATIO,
    DEFAULT_NEAR_DISTANCE, DEFAULT_FAR_DISTANCE);
  m_camera.SetPosition(m_config.pos);
  m_camera.SetAngles(m_config.angleX, m_config.angleY);

  //set flashlight parameters
  m_flashLight.SetPosition(0.0f, 0.0f, DEFAULT_NEAR_DISTANCE, 1.0f);
  m_flashLight.SetDiffuseCoeff (DEFAULT_FLASH_LIGHT_DIFFUSE);
  m_flashLight.SetSpecularCoeff(DEFAULT_FLASH_LIGHT_SPECULAR);
  m_flashLight.SetSpotExponent (DEFAULT_FLASH_LIGHT_EXPONENT);
  m_flashLight.SetSpotCutoff   (DEFAULT_FLASH_LIGHT_CUTOFF);
  //apply flashlight
  m_flashLight.Walk();

  //randomize game gravity
  Game::RandomGravity(m_gravityEarth, m_gravityGame);

  //todo: present start screen and pospone loading game data

#if 0
  //create shadow map texture
  glGenTextures(1, &m_shadowMapTexture);
  glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
  m_shadowMapWidth = 512;
  m_shadowMapHeight = 512;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
    m_shadowMapWidth, m_shadowMapHeight, 0,
    GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  Color white = COLOR_WHITE;
  glMaterialfv(GL_FRONT, GL_SPECULAR,  white);
  glMaterialf (GL_FRONT, GL_SHININESS, 16.0f);

  //get camera projection matrix
  glPushMatrix();
    gluPerspective(DEFAULT_FIELD_OF_VIEW,
      (float)m_config.width / m_config.height,
      DEFAULT_NEAR_DISTANCE, DEFAULT_FAR_DISTANCE);
    glGetFloatv(GL_MODELVIEW_MATRIX, m_cameraProjMatrix);
  glPopMatrix();

  //get camera view matrix
  glPushMatrix();
    m_camera.Walk();
    //Point &p = m_camera.GetPosition();
    //gluLookAt(p[CX], p[CY], p[CZ], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, m_cameraViewMatrix);
  glPopMatrix();

  //get light projection matrix
  glPushMatrix();
    gluPerspective(DEFAULT_FIELD_OF_VIEW, //1.0f,
      (float)m_config.width / m_config.height,
      DEFAULT_NEAR_DISTANCE, DEFAULT_FAR_DISTANCE);
    glGetFloatv(GL_MODELVIEW_MATRIX, m_lightProjMatrix);
  glPopMatrix();

  //get light view matrix
  glPushMatrix();
    gluLookAt(0.0f, 5.0f, -5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, m_lightViewMatrix);
  glPopMatrix();
#endif

  return true;
}

void Game::WalkEffects(ulong flags)
{
  //enable transparency
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  //enable horizontal clipping plane
  glClipPlane(GL_CLIP_PLANE0, m_clipPlane);
  glEnable(GL_CLIP_PLANE0);

  //draw effects
  m_pEffects->Walk(flags);

  //disable clipping plane
  glDisable(GL_CLIP_PLANE0);

  //restore blending function
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::WalkMenu()
{
  glEnable(GL_TEXTURE_2D);

  //draw menu items
  m_menuMain.Walk();

  //draw menu text
  //todo: revisit menu design and remove this hack
  glPushMatrix();
    glTranslatef(-0.02515f, 0.017125f, 0.0f);
    glColor4f(DEFAULT_MENU_ITEM_COLOR);
    for (uint i = 0; i < GAME_MENU_SIZE; ++i)
    {
      glPushMatrix();
        glTranslatef(0.0f, -0.004178f * i, 0.0f);
        glScalef(0.67125f, 0.67125f, 1.0f);
        m_font.DrawText(FONT_STYLE_NORMAL, m_aMenuStrings[i]);
      glPopMatrix();
    }
  glPopMatrix();

  //restore modelview matrix
  glDisable(GL_TEXTURE_2D);
}

void Game::OnDrawScene()
{
  //stop timer
  m_timer.Stop();

  //increment frame count
  ++m_frame;

  //retrieve elapsed time
  float dt = (float)m_timer.GetTime();
  DbgMessage("Elapsed time = %f seconds", dt);

  //restart timer
  m_timer.Start();

  //process input
  ProcessInput(dt);

  //update game state
  UpdateState(dt);

  bool stencilOn = (bool)(m_gameFlags & GAME_FLAGS_STENCIL);
  bool reflectOn = (bool)(m_gameFlags & GAME_FLAGS_REFLECT);
  bool shadowsOn = (bool)(m_gameFlags & GAME_FLAGS_SHADOWS);

  if (stencilOn)
  {
    //clear color, depth and stencil buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
  else
  {
    //clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  //save modelview matrix
  glPushMatrix();

  //apply camera transformation
  m_camera.Walk();

  //draw skybox
  if (m_gameFlags & GAME_FLAGS_SKYBOX)
  {
#ifndef _DEBUG
    //save fog settings
    glPushAttrib(GL_FOG_BIT);

      //apply skybox fog settings
      glFogi(GL_FOG_MODE, GL_EXP);
      glFogf(GL_FOG_DENSITY, DEFAULT_SKYBOX_FOG_DENSITY);
      glEnable(GL_FOG);
#endif

      glPushMatrix();

        if (m_gameFlags & GAME_FLAGS_NOFREECAM)
        {
          //cancel out camera translation
          Point &p = m_camera.GetPosition();
          glTranslatef(p[CX], p[CY], p[CZ]);
        }

        //draw skybox
        glEnable(GL_TEXTURE_2D);
        m_pSkyBox->Walk(m_nodeFlags);
        glDisable(GL_TEXTURE_2D);

      glPopMatrix();

#ifndef _DEBUG
    //restore fog settings
    glPopAttrib();
#endif
  }

  //draw reflections
  if (reflectOn)
  {
    if (stencilOn)
    {
      //disable writing to color and depth buffers
      glDisable(GL_DEPTH_TEST);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

      //enable stencil test
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_ALWAYS, 1, DEFAULT_STENCIL_MASK);
      glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

      //draw mirrors into stencil buffer
      m_pMirrors->Walk(NODE_FLAGS_PRIMITIVE | NODE_FLAGS_DISPLIST);

      //enable writing to color and depth buffers
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glEnable(GL_DEPTH_TEST);
    }

    //todo: draw reflected scene once for each mirror applying
    //each mirror's reflection transformation appropriately
    glPushMatrix();

      //reflect objects along y-axis
      glScalef(1.0f, -1.0f, 1.0f);

      //account for inverted normals
      glCullFace(GL_FRONT);

      //reflect light position
      m_pLight->Walk(NODE_FLAGS_TRANSFORM);

      glEnable(GL_LIGHTING);
      glEnable(GL_TEXTURE_2D);

#ifndef _DEBUG
      glEnable(GL_FOG);
#endif

      if (stencilOn)
      {
        //draw only where stencil is 1
        glStencilFunc(GL_EQUAL, 1, DEFAULT_STENCIL_MASK);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
      }

      //draw reflected scene
      m_pScene->Walk(m_nodeFlags);

      glDisable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING);

      //draw reflected effects
      if (m_gameFlags & GAME_FLAGS_EFFECTS)
      {
        WalkEffects(m_nodeFlags);
      }

#ifndef _DEBUG
      glDisable(GL_FOG);
#endif

      //restore back face culling
      glCullFace(GL_BACK);

    glPopMatrix();

    if (stencilOn)
    {
      glDisable(GL_STENCIL_TEST);
    }
  }

  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

#ifndef _DEBUG
  glEnable(GL_FOG);
#endif

  //reset current texture
  glBindTexture(GL_TEXTURE_2D, BAD_TEXTURE);

  if (shadowsOn)
  {
    if (stencilOn)
    {
      //draw ground with stencil value 2 to draw shadows once per pixel
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_ALWAYS, 2, DEFAULT_STENCIL_MASK);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    }
  }

  //apply light position
  m_pLight->Walk(NODE_FLAGS_TRANSFORM);

  //pull mirrors towards viewer to draw them on top of terrain
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(DEFAULT_OFFSET_MIRRORS);

  //draw mirrors
  m_pMirrors->Walk(m_nodeFlags);

  //disable polygon offset
  glDisable(GL_POLYGON_OFFSET_FILL);

  //draw terrain
  m_pTerrain->Walk(m_nodeFlags);

  if (shadowsOn)
  {
    if (stencilOn)
    {
      //disable stencil test
      glDisable(GL_STENCIL_TEST);
    }
  }

  //draw scene
  m_pScene->Walk(m_nodeFlags);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  //draw effects
  if (m_gameFlags & GAME_FLAGS_EFFECTS)
  {
    WalkEffects(m_nodeFlags);
  }

#ifndef _DEBUG
  glDisable(GL_FOG);
#endif

#ifdef _DEBUG
  //set polygon mode to lines and disable face culling
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDisable(GL_CULL_FACE);

  //pull bounding volumes towards viewer to draw them on top of objects
  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(DEFAULT_OFFSET_BOUNDS);

  //specify bounding volume color
  glColor4f(DEFAULT_BOUNDS_COLOR);

  //draw bounding volumes
  m_pBounds->DrawBounds();

  //disable polygon offset
  glDisable(GL_POLYGON_OFFSET_LINE);

  //restore polygon mode and restore face culling
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

  //draw shadows
  if (shadowsOn)
  {
    if (stencilOn)
    {
      //draw shadows only where stencil is above 1
      //update stencil to 1 to avoid redrawing shadows twice
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_LESS, 1, DEFAULT_STENCIL_MASK);
      glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    }

    glPushMatrix();

      //project shadows
      glMultMatrixf(m_shadowMatrix);

      //enable front face culling
      //glCullFace(GL_FRONT);
      //disable face culling
      glDisable(GL_CULL_FACE);

      //apply shadow color
      glColor4f(DEFAULT_SHADOW_COLOR);

      //pull shadows towards viewer to draw them on top of mirrors
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(DEFAULT_OFFSET_SHADOWS);

      //draw shadows
      m_pShadows->Walk(NODE_FLAGS_PRIMITIVE | NODE_FLAGS_DISPLIST);

      //disable polygon offset
      glDisable(GL_POLYGON_OFFSET_FILL);

      //restore back face culling
      //glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);

    glPopMatrix();

    if (stencilOn)
    {
      //disable stencil test
      glDisable(GL_STENCIL_TEST);
    }
  }

  //restore modelview matrix
  glPopMatrix();

  if (m_gameFlags & GAME_FLAGS_MENU)
  {
    //draw menu
    WalkMenu();
  }
  else
  {
    //draw crosshair
    glColor4fv(m_crossColor);
    m_cross.Walk();
  }

#if 0
  //draw shadows
  glPushMatrix();

    //save projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

      //draw scene as seen from light position
  
      //load light projection matrix
      glLoadMatrixf(m_lightProjMatrix);

      //load light view matrix
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf(m_lightViewMatrix);

      //use viewport of the shadow map size
      glViewport(0, 0, m_shadowMapWidth, m_shadowMapHeight);

      //save polygon settings
      glPushAttrib(GL_POLYGON_BIT);

        //shadow map contains only back faces
        glCullFace(GL_FRONT);

        //disable writing to color buffer
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        //enable flat shading
        glShadeModel(GL_FLAT);

        //draw scene into depth buffer
        m_pScene->Walk();

        //read shadow map texture from depth buffer
        glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
          m_shadowMapWidth, m_shadowMapHeight);

        //todo: use off-screen buffer to avoid restrictions such as
        //window being big enough to fit the shadow map texture and
        //window not being occluded by other windows
      
        //draw the entire scene with a dim diffuse light, as drawing
        //with only ambient light will produce unnatural flat shadows

        //restore shading
        glShadeModel(GL_SMOOTH);

        //enable writing to color buffer
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

      //restore polygon settings
      glPopAttrib();

      //clear depth buffer
      glClear(GL_DEPTH_BUFFER_BIT);

      //draw scene as seen from camera position

      //load camera projection matrix
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf(m_cameraProjMatrix);

      //load camera view matrix
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf(m_cameraViewMatrix);

      //use viewport of the window size
      glViewport(0, 0, m_config.width, m_config.height);

      //use dim light to represent shadowed areas
      Vector position = {0.0f, 5.0f, -5.0f, 0.0f};
      glLightfv(GL_LIGHT4, GL_POSITION, position);
      Color white = COLOR_WHITE;
      Color black = COLOR_BLACK;
      Color grey  = COLOR_GREY;
      glLightfv(GL_LIGHT4, GL_DIFFUSE,  grey);
      glLightfv(GL_LIGHT4, GL_SPECULAR, black);
      glEnable(GL_LIGHT4);

      //draw scene
      m_pScene->Walk();

	    glLightfv(GL_LIGHT4, GL_DIFFUSE, white);
	    glLightfv(GL_LIGHT4, GL_SPECULAR, white);

	    //Calculate texture matrix for projection
	    //This matrix takes us from eye space to the light's clip space
	    //It is postmultiplied by the inverse of the current view matrix
      //when specifying texgen
	    Matrix textureMatrix = {0.5f, 0.0f, 0.0f, 0.0f,
								              0.0f, 0.5f, 0.0f, 0.0f,
								              0.0f, 0.0f, 0.5f, 0.0f,
								              0.5f, 0.5f, 0.5f, 1.0f};
      Matrix resultMatrix;
      MatrixMultiply(textureMatrix, m_lightProjMatrix, resultMatrix);
      MatrixMultiply(resultMatrix, m_lightViewMatrix, textureMatrix);

      Vector row0 = {textureMatrix[0],
                     textureMatrix[4],
                     textureMatrix[8],
                     textureMatrix[12]};

      Vector row1 = {textureMatrix[1],
                     textureMatrix[5],
                     textureMatrix[9],
                     textureMatrix[13]};

      Vector row2 = {textureMatrix[2],
                     textureMatrix[6],
                     textureMatrix[10],
                     textureMatrix[14]};

      Vector row3 = {textureMatrix[3],
                     textureMatrix[7],
                     textureMatrix[11],
                     textureMatrix[15]};

	    //Set up texture coordinate generation.
	    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	    glTexGenfv(GL_S, GL_EYE_PLANE, row0);
	    glEnable(GL_TEXTURE_GEN_S);

	    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	    glTexGenfv(GL_T, GL_EYE_PLANE, row1);
	    glEnable(GL_TEXTURE_GEN_T);

	    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	    glTexGenfv(GL_R, GL_EYE_PLANE, row2);
	    glEnable(GL_TEXTURE_GEN_R);

	    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	    glTexGenfv(GL_Q, GL_EYE_PLANE, row3);
	    glEnable(GL_TEXTURE_GEN_Q);

	    //Bind & enable shadow map texture
	    glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	    glEnable(GL_TEXTURE_2D);

	    //Enable shadow comparison
	    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);

	    //Shadow comparison should be true (ie not in shadow) if r<=texture
	    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	    //Shadow comparison should generate an INTENSITY result
	    glTexParameteri(GL_TEXTURE_2D,
        GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

	    //Set alpha test to discard false comparisons
	    glAlphaFunc(GL_GEQUAL, 0.99f);
	    glEnable(GL_ALPHA_TEST);

	    m_pScene->Walk();

	    //Restore other states
	    glDisable(GL_LIGHTING);
	    glDisable(GL_ALPHA_TEST);

	    glDisable(GL_TEXTURE_GEN_S);
	    glDisable(GL_TEXTURE_GEN_T);
	    glDisable(GL_TEXTURE_GEN_R);
	    glDisable(GL_TEXTURE_GEN_Q);

    //restore projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

  //restore modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
#endif

  //swap buffers
  glutSwapBuffers();
}

void Game::ProcessInput(float dt)
{
  if (m_gameFlags & GAME_FLAGS_MENU)
  {
    if (m_aKeyStates[m_aKeyMap[ACTION_MENU]])
    {
      //turn menu off
      m_gameFlags &= ~GAME_FLAGS_MENU;
      DbgMessage("Menu off");
      //reset key state
      m_aKeyStates[m_aKeyMap[ACTION_MENU]] = false;
    }
    if (m_aKeyStates[m_aKeyMap[ACTION_MENU_SELECT]])
    {
      //select menu item
      //m_menuMain.SelectItem();
      m_menuMain.Select();
      //reset key state
      m_aKeyStates[m_aKeyMap[ACTION_MENU_SELECT]] = false;
    }
    else if (m_aKeyStates[m_aKeyMap[ACTION_MENU_UP]])
    {
      //move menu selection up
      //m_menuMain.PrevItem();
      m_menuMain.MoveUp();
      //reset key state
      m_aKeyStates[m_aKeyMap[ACTION_MENU_UP]] = false;
    }
    else if (m_aKeyStates[m_aKeyMap[ACTION_MENU_DOWN]])
    {
      //move menu selection down
      //m_menuMain.NextItem();
      m_menuMain.MoveDown();
      //reset key state
      m_aKeyStates[m_aKeyMap[ACTION_MENU_DOWN]] = false;
    }
    else if (m_aKeyStates[m_aKeyMap[ACTION_MENU_LEFT]])
    {
      //move menu selection left
      //m_menuMain.PrevItem();
      m_menuMain.MoveLeft();
      //reset key state
      m_aKeyStates[m_aKeyMap[ACTION_MENU_LEFT]] = false;
    }
    else if (m_aKeyStates[m_aKeyMap[ACTION_MENU_RIGHT]])
    {
      //move menu selection right
      //m_menuMain.NextItem();
      m_menuMain.MoveRight();
      //reset key state
      m_aKeyStates[m_aKeyMap[ACTION_MENU_RIGHT]] = false;
    }
  }
  else
  {
    if (m_aKeyStates[m_aKeyMap[ACTION_MENU]])
    {
      //turn menu on
      m_gameFlags |= GAME_FLAGS_MENU;
      DbgMessage("Menu on");
      //reset key state
      m_aKeyStates[m_aKeyMap[ACTION_MENU]] = false;
    }
    else
    {
      //check camera orientation keys
      bool lookUp   = m_aKeyStates[m_aKeyMap[ACTION_LOOK_UP]];
      bool lookDown = m_aKeyStates[m_aKeyMap[ACTION_LOOK_DOWN]];
      bool turnLeft  = m_aKeyStates[m_aKeyMap[ACTION_TURN_LEFT]];
      bool turnRight = m_aKeyStates[m_aKeyMap[ACTION_TURN_RIGHT]];
      if (lookUp || lookDown || turnLeft || turnRight)
      {
        //calculate angle increment
        float d = DEFAULT_KEYBOARD_LOOK_FACTOR * dt;
        //update camera look up/down orientation
        if (lookUp)
        {
          m_camera.LookUp(d);
        }
        if (lookDown)
        {
          m_camera.LookDown(d);
        }
        //update camera look left/right orientation
        if (turnLeft)
        {
          m_camera.TurnLeft(d);
        }
        if (turnRight)
        {
          m_camera.TurnRight(d);
        }
      }

      //check camera jump key
      if (m_aKeyStates[m_aKeyMap[ACTION_JUMP]] &&
         (m_gameFlags & GAME_FLAGS_NOFREECAM))
      {
        //check for ground level
        if (m_camera.GetPosition()[CY] <= DEFAULT_CAMERA_Y)
        {
          //set upward camera velocity
          m_camera.GetVelocity()[CY] = DEFAULT_JUMP_VELOCITY;
        }
      }

      //check camera movement keys
      bool moveForw  = m_aKeyStates[m_aKeyMap[ACTION_MOVE_FORW]];
      bool moveBack  = m_aKeyStates[m_aKeyMap[ACTION_MOVE_BACK]];
      bool moveLeft  = m_aKeyStates[m_aKeyMap[ACTION_MOVE_LEFT]];
      bool moveRight = m_aKeyStates[m_aKeyMap[ACTION_MOVE_RIGHT]];
      bool moveUp    = m_aKeyStates[m_aKeyMap[ACTION_MOVE_UP]];
      bool moveDown  = m_aKeyStates[m_aKeyMap[ACTION_MOVE_DOWN]];
      if (moveForw || moveBack || moveLeft || moveRight || moveUp || moveDown)
      {
        //calculate distance increment
        float d = DEFAULT_KEYBOARD_MOVE_FACTOR * dt;
        if ((moveForw || moveBack) && (moveLeft || moveRight))
        {
          //reduce velocity if moving diagonally
          d *= SQRT_2_OVER_2;
        }
        //update camera position
        if (moveForw)
        {
          m_camera.MoveForward(d);
        }
        if (moveBack)
        {
          m_camera.MoveBack(d);
        }
        if (moveLeft)
        {
          m_camera.MoveLeft(d);
        }
        if (moveRight)
        {
          m_camera.MoveRight(d);
        }
        //check for free camera
        if (!(m_gameFlags & GAME_FLAGS_NOFREECAM))
        {
          if (moveUp)
          {
            m_camera.MoveUp(d);
          }
          if (moveDown)
          {
            m_camera.MoveDown(d);
          }
        }
      }

      //check camera y-position reset key
      if (m_aKeyStates[m_aKeyMap[ACTION_Y_RESET]])
      {
        //reset camera y-position
        m_camera.MoveDown(m_camera.GetPosition()[CY] - DEFAULT_CAMERA_Y);
        //reset camera look up/down angle
        m_camera.SetAngleX(0.0f);
      }

      //check camera zoom keys
      bool zoomIn  = m_aKeyStates[m_aKeyMap[ACTION_ZOOM_IN]];
      bool zoomOut = m_aKeyStates[m_aKeyMap[ACTION_ZOOM_OUT]];
      if (zoomIn || zoomOut)
      {
        //calculate zoom increment
        float d = DEFAULT_KEYBOARD_ZOOM_FACTOR * dt;
        //update camera zoom
        if (zoomIn)
        {
          m_camera.ZoomIn(d);
        }
        if (zoomOut)
        {
          m_camera.ZoomOut(d);
        }
      }

      //check zoom reset key
      if (m_aKeyStates[m_aKeyMap[ACTION_ZOOM_RESET]])
      {
        //reset camera zoom
        m_camera.ZoomOut();
      }

      //check flashlight key
      if (m_aKeyStates[m_aKeyMap[ACTION_FLASH_LIGHT]])
      {
        //toggle flashlight on/off
        m_flashLight.Toggle();
        //reset key state
        m_aKeyStates[m_aKeyMap[ACTION_FLASH_LIGHT]] = false;
      }
    }
  }
}

//static
void Game::RandomGravity(Vector oldGravity, Vector newGravity)
{
  //randomize gravity
  float spread = RandomNumber(
    DEFAULT_GRAVITY_MIN_SPREAD, DEFAULT_GRAVITY_MAX_SPREAD);
  float s = FastSin(spread);
  newGravity[CX] = oldGravity[CX] + s * RandomNumber(-1.0f, 1.0f);
  newGravity[CZ] = oldGravity[CZ] + s * RandomNumber(-1.0f, 1.0f);
  newGravity[CY] = oldGravity[CY];
}

void Game::UpdateState(float dt)
{
  //randomize gravity
  Game::RandomGravity(m_gravityGame, m_gravityRandom);

  //save last camera position
  //PointCopy(m_lastPosition, m_camera.GetPosition());

  bool updateCamera = !CheckCollisions();

  //todo: find terrain level, slope and friction at camera position
  if (updateCamera)
  {
    //update camera position
    if (m_gameFlags & GAME_FLAGS_NOFREECAM)
    {
      if (m_gameFlags & GAME_FLAGS_PHYSICS)
      {
        //apply game gravity to camera
        m_camera.Update(m_frame, dt, m_gravityGame);
      }
      else
      {
        //apply earth gravity to camera
        m_camera.Update(m_frame, dt, m_gravityEarth);
      }
      //check terrain level
      //todo: load terrain heights from file
      Point &p = m_camera.GetPosition();
      if (p[CY] < DEFAULT_CAMERA_Y)
      {
        //reset camera position
        p[CY] = DEFAULT_CAMERA_Y;
        //reset camera velocity in y-direction
        m_camera.GetVelocity()[CY] = 0.0f;
        //reset camera velocity
        //m_camera.SetVelocity(0.0f, 0.0f, 0.0f);
      }
    }
  }

  //update animated game objects
  if (m_gameFlags & GAME_FLAGS_ANIMATION)
  {
    //check for physics
    if (m_gameFlags & GAME_FLAGS_PHYSICS)
    {
      //apply earth gravity to update effects
      m_pEffects->Update(m_frame, dt, m_gravityRandom);
    }
    else
    {
      //apply space gravity to update effects
      m_pEffects->Update(m_frame, dt, m_gravitySpace);
    }
  }
}

bool Game::CheckCollisions()
{
  //perform collision detection
  if (m_gameFlags & GAME_FLAGS_COLLISIONS)
  {
    //get list of scene objects
    GroupNode *pGroup = dynamic_cast<GroupNode *>(m_pScene);
    if (pGroup)
    {
      SceneNodeList &nodes = pGroup->GetNodeList();
      DbgMessage("Number of objects = %u", nodes.size());
      //find distance to each object in the scene
      for (SceneNodeList::iterator it = nodes.begin(); it != nodes.end(); it++)
      {
        DbgMessage("Object %s", (*it)->GetName());
        //get object bounding volume
        Bounds bounds;
        if ((*it)->GetBounds(bounds))
        {
          //find distance to the bounding volume center
          Vector v;
          PointSubtractPoint(bounds.center, m_camera.GetPosition(), v);
          float d = VectorLengthSquared(v);
          DbgMessage("Distance to %s is %f", (*it)->GetName(), d);
          if (d <= bounds.rSquared)
          {
            //there is a collision
            return true;
          }
        }
      }
    }
  }
  return false;
}

void Game::OnCloseWindow()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_NOKEYREPS)
  {
    //restore key repeats
    glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
  }
  //restore cursor
  glutSetCursor(GLUT_CURSOR_INHERIT);
  //save configuration
  ConfigSave(&m_config, m_config.szConfigFile);
}

void Game::OnChangeVisible(bool visible)
{
  DbgMessage("visible = %s", visible ? "true" : "false");
  //process visibility change event
  if (visible)
  {
    //get current window dimensions
    int width  = m_config.width;
    int height = m_config.height;

    //place cursor in the center of the window
    m_centerX = width  >> 1;
    m_centerY = height >> 1;
    glutWarpPointer(m_centerX, m_centerY);

    //mouse sampling weight is a function of window dimensions
    m_sampleWeightX = (float)width *
      (DEFAULT_MOUSE_SAMPLE_WEIGHT / DEFAULT_WINDOW_WIDTH);
    m_sampleWeightY = (float)height *
      (DEFAULT_MOUSE_SAMPLE_WEIGHT / DEFAULT_WINDOW_HEIGHT);

    //mouse average weight is a complement of sampling weight
    m_averageWeightX = 1.0f - m_sampleWeightX;
    m_averageWeightY = 1.0f - m_sampleWeightY;

    //start timer and set idle handler
    m_timer.Start();
    glutIdleFunc(DrawScene);
  }
  else
  {
    //stop timer and remove idle handler
    m_timer.Stop();
    glutIdleFunc(NULL);
  }
}

void Game::OnChangeSize(int width, int height)
{
  DbgMessage("width = %d, height = %d", width, height);
  //reset projection matrix to identity
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPushMatrix();

  //set OpenGL viewport
  glViewport(0, 0, width, height);

  //apply camera view frustum transformation
  m_camera.SetAspectRatio((float)width / height);
  m_camera.Walk(NODE_FLAGS_FRUSTUM);

  //update configuration
  m_config.width  = width;
  m_config.height = height;

  //switch to modelview matrix
  glMatrixMode(GL_MODELVIEW);
}

void Game::OnKey(uchar key, bool down, int x, int y)
{
  DbgMessage("key = %u, down = %s, x = %d, y = %d",
    static_cast<uint>(key), down ? "true" : "false", x, y);
  //update both lower and upper case to avoid accidental caps lock
  uchar u = toupper(key);
  uchar l = tolower(key);
  if (u < NUM_KEYS_ASCII)
  {
    m_aKeyStates[u] = down;
  }
  if (l < NUM_KEYS_ASCII)
  {
    m_aKeyStates[l] = down;
  }
}

void Game::OnSysKey(int key, bool down, int x, int y)
{
  DbgMessage("key = %d, down = %s, x = %d, y = %d", key,
    down ? "true" : "false", x, y);
  //process system key event
  switch (key)
  {
    case GLUT_KEY_LEFT:
      m_aKeyStates[SYS_KEY_LEFT] = down;
      break;

    case GLUT_KEY_UP:
      m_aKeyStates[SYS_KEY_UP] = down;
      break;

    case GLUT_KEY_RIGHT:
      m_aKeyStates[SYS_KEY_RIGHT] = down;
      break;

    case GLUT_KEY_DOWN:
      m_aKeyStates[SYS_KEY_DOWN] = down;
      break;
  }
}

void Game::OnMouseButton(int button, bool down, int x, int y)
{
  DbgMessage("button = %d, down = %s, x = %d, y = %d", button,
    down ? "true" : "false", x, y);
  //process mouse button event
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      m_aKeyStates[MOUSE_BUTTON_LEFT] = down;
      break;

    case GLUT_MIDDLE_BUTTON:
      m_aKeyStates[MOUSE_BUTTON_MIDDLE] = down;
      break;

    case GLUT_RIGHT_BUTTON:
      m_aKeyStates[MOUSE_BUTTON_RIGHT] = down;
      break;
  }
}

void Game::OnMouseWheel(int wheel, int dir, int x, int y)
{
  DbgMessage("wheel = %d, dir = %d, x = %d, y = %d", wheel, dir, x, y);
  //process mouse wheel event
  if (dir < 0)
  {
    //wheel down
    m_aKeyStates[MOUSE_WHEEL_DOWN] = true;
  }
  else
  {
    //wheel up
    m_aKeyStates[MOUSE_WHEEL_UP] = true;
  }
}

void Game::OnMouseMotion(int x, int y)
{
  DbgMessage("x = %d, y = %d", x, y);
  //process mouse motion event
  if (m_gameFlags & GAME_FLAGS_MOUSE)
  {
    //calculate mouse displacement
    float sampleDx = DEFAULT_MOUSE_LOOK_FACTOR * (x - m_centerX);
    float sampleDy = DEFAULT_MOUSE_LOOK_FACTOR * (y - m_centerY);

    //average several mouse samples for smooth motion
    m_averageDx = m_sampleWeightX * sampleDx + m_averageWeightX * m_averageDx;
    m_averageDy = m_sampleWeightY * sampleDy + m_averageWeightY * m_averageDy;

    //update camera turn left/right angle
    m_camera.Turn(m_averageDx);

    //update camera look up/down angle
    m_camera.Look(m_averageDy);

    //place cursor in the center of the window
    glutWarpPointer(m_centerX, m_centerY);

    //disable mouse motion callback to avoid recursion
    //m_gameFlags &= ~GAME_FLAGS_MOUSE;
  }
  /*else
  {
    //enable mouse motion callback
    m_gameFlags |= GAME_FLAGS_MOUSE;
  }*/
}

void Game::OnMouseMotion2(int x, int y)
{
  DbgMessage("x = %d, y = %d", x, y);
  //process mouse motion event
  if (m_gameFlags & GAME_FLAGS_MOUSE)
  {
    //calculate mouse displacement
    float sampleDx = DEFAULT_MOUSE_LOOK_FACTOR * (x - m_lastMouseX);
    float sampleDy = DEFAULT_MOUSE_LOOK_FACTOR * (y - m_lastMouseY);

    //average several mouse samples for smooth motion
    m_averageDx = m_sampleWeightX * sampleDx + m_averageWeightX * m_averageDx;
    m_averageDy = m_sampleWeightY * sampleDy + m_averageWeightY * m_averageDy;

    //update camera left/right turn angle
    m_camera.Turn(m_averageDx);

    //update camera up/down look angle
    m_camera.Look(m_averageDy);

    //update last mouse position
    m_lastMouseX = x;
    m_lastMouseY = y;
  }
}

void Game::OnToggleStencil()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_STENCIL)
  {
    //turn stencil buffer off
    m_gameFlags &= ~GAME_FLAGS_STENCIL;
  }
  else
  {
    //turn stencil buffer on
    m_gameFlags |= GAME_FLAGS_STENCIL;
  }
}

void Game::OnToggleReflections()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_REFLECT)
  {
    //turn reflections off
    m_gameFlags &= ~GAME_FLAGS_REFLECT;
  }
  else
  {
    //turn reflections on
    m_gameFlags |= GAME_FLAGS_REFLECT;
  }
}

void Game::OnToggleShadows()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_SHADOWS)
  {
    //turn shadows off
    m_gameFlags &= ~GAME_FLAGS_SHADOWS;
  }
  else
  {
    //turn shadows on
    m_gameFlags |= GAME_FLAGS_SHADOWS;
  }
}

void Game::OnToggleSkyBox()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_SKYBOX)
  {
    //turn skybox off
    m_gameFlags &= ~GAME_FLAGS_SKYBOX;
  }
  else
  {
    //turn skybox on
    m_gameFlags |= GAME_FLAGS_SKYBOX;
  }
}

void Game::OnToggleFreeCamera()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_NOFREECAM)
  {
    //turn free camera on
    m_gameFlags &= ~GAME_FLAGS_NOFREECAM;
    //reset camera velocity
    m_camera.SetVelocity(0.0f, 0.0f, 0.0f);
  }
  else
  {
    //turn free camera off
    m_gameFlags |= GAME_FLAGS_NOFREECAM;
  }
}

void Game::OnToggleEffects()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_EFFECTS)
  {
    //turn effects off
    m_gameFlags &= ~GAME_FLAGS_EFFECTS;
  }
  else
  {
    //turn effects on
    m_gameFlags |= GAME_FLAGS_EFFECTS;
  }
}

void Game::OnToggleCollisions()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_COLLISIONS)
  {
    //turn collisions off
    m_gameFlags &= ~GAME_FLAGS_COLLISIONS;
  }
  else
  {
    //turn collisions on
    m_gameFlags |= GAME_FLAGS_COLLISIONS;
  }
}

void Game::OnToggleAnimation()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_ANIMATION)
  {
    //turn animation off
    m_gameFlags &= ~GAME_FLAGS_ANIMATION;
  }
  else
  {
    //turn animation on
    m_gameFlags |= GAME_FLAGS_ANIMATION;
  }
}

void Game::OnTogglePhysics()
{
  DbgEnter();
  if (m_gameFlags & GAME_FLAGS_PHYSICS)
  {
    //turn physics off
    m_gameFlags &= ~GAME_FLAGS_PHYSICS;
    //reset camera velocity
    m_camera.SetVelocity(0.0f, 0.0f, 0.0f);
  }
  else
  {
    //turn physics on
    m_gameFlags |= GAME_FLAGS_PHYSICS;
  }
}

void Game::OnTriggerExit()
{
  DbgEnter();
  //exit game loop
  //m_gameFlags &= ~GAME_FLAGS_RUNNING;
  glutLeaveMainLoop();
}

bool Game::LoadFonts(cchar *szFolder)
{
  //form image file name
  char szFile [MAX_FILE_NAME_SIZE + 1];
  snprintf(szFile, MAX_FILE_NAME_SIZE, "%s/" DEFAULT_FONT_IMAGE, szFolder);
  szFile[MAX_FILE_NAME_SIZE] = NULL_CHAR;

  //load image
  PngImage image;
  if (!image.Image::Load(szFile))
  {
    DbgError("Unable to load %s", szFile);
    return false;
  }

  //create font texture
  if (!m_fontTexture.Create(&image, false))
  {
    DbgError("Unable to create texture from %s", szFile);
    return false;
  }

  //create font
  if (!m_font.Create(&m_fontTexture, 0.005f, DEFAULT_FONT_SIZE))
  {
    DbgEnter("Unable to create font");
    m_fontTexture.Destroy();
    return false;
  }

  return true;
}

bool Game::LoadMenus(cchar *szFolder)
{
  //form image file name
  char szFile [MAX_FILE_NAME_SIZE + 1];
  snprintf(szFile, MAX_FILE_NAME_SIZE, "%s/" DEFAULT_MENU_IMAGE, szFolder);
  szFile[MAX_FILE_NAME_SIZE] = NULL_CHAR;

  //load image
  PngImage image;
  if (!image.Image::Load(szFile))
  {
    DbgError("Unable to load %s", szFile);
    return false;
  }

  //create background texture
  if (!m_menuTexture.Create(&image, false, false, GL_LINEAR, GL_LINEAR))
  {
    DbgError("Unable to create texture from %s", szFile);
    return false;
  }

  //create menu
  Color backColor = {DEFAULT_MENU_BACK_COLOR};
  Color itemColor = {DEFAULT_MENU_ITEM_COLOR};
  Color selectColor = {DEFAULT_MENU_SELECT_COLOR};
  if (!m_menuMain.Create(1, GAME_MENU_SIZE,
    DEFAULT_MENU_ITEM_WIDTH, DEFAULT_MENU_ITEM_HEIGHT,
    itemColor, selectColor, backColor, &m_menuTexture))
  {
    DbgError("Unable to create main menu");
    m_menuTexture.Destroy();
    return false;
  }

  /*Color selectColor = {DEFAULT_MENU_SELECT_COLOR};
  if (!m_menuMain.Create(m_factory.GetNullNode(), selectColor))
  {
    DbgError("Unable to create main menu");
    return false;
  }*/

  //create menu icon
  Square icon;
  if (!m_menuIcon.Create(&icon))
  {
    DbgError("Unable to create menu icon");
    m_menuTexture.Destroy();
    return false;
  }

  //menu callbacks
  const MenuCallback aMenuCallbacks [GAME_MENU_SIZE] =
  {
    ToggleStencil,
    ToggleReflections,
    ToggleShadows,
    ToggleSkyBox,
    ToggleFreeCamera,
    ToggleEffects,
    ToggleCollisions,
    ToggleAnimation,
    TogglePhysics,
    TriggerExit,
  };

  //fill main menu
  for (uint i = 0; i < GAME_MENU_SIZE; ++i)
  {
    //create menu item
#if 0
    MenuItem *pItem = m_factory.NewMenuItem();
    if (!pItem->Create(m_aMenuStrings[i],
      &m_menuIcon, &m_font, FONT_STYLE_NORMAL))
    {
      DbgError("Unable to create menu item");
      return false;
    }
    //set menu item
    m_menuMain.SetItem(0, i, pItem, aMenuCallbacks[i]);
#endif
    m_menuMain.SetItem(0, i, &m_menuIcon, aMenuCallbacks[i]);
    //m_menuMain.AddItem(&m_menuIcon, aMenuCallbacks[i]);
  }

  DbgMessage("Menu initialized successfully");
  return true;
}

void DrawScene()
{
  g_game.OnDrawScene();
}

void CloseWindow()
{
  g_game.OnCloseWindow();
}

void ChangeSize(int width, int height)
{
  g_game.OnChangeSize(width, height);
}

void ChangeVisible(int state)
{
  g_game.OnChangeVisible(GLUT_VISIBLE == state);
}

void KeyDown(uchar key, int x, int y)
{
  g_game.OnKey(key, true, x, y);
}

void KeyUp(uchar key, int x, int y)
{
  g_game.OnKey(key, false, x, y);
}

void SysKeyDown(int key, int x, int y)
{
  g_game.OnSysKey(key, true, x, y);
}

void SysKeyUp(int key, int x, int y)
{
  g_game.OnSysKey(key, false, x, y);
}

void MouseButton(int button, int state, int x, int y)
{
  g_game.OnMouseButton(button, (GLUT_DOWN == state), x, y);
}

void MouseWheel(int wheel, int dir, int x, int y)
{
  g_game.OnMouseWheel(wheel, dir, x, y);
}

void MouseMotion(int x, int y)
{
  g_game.OnMouseMotion2(x, y);
}

void ToggleStencil()
{
  g_game.OnToggleStencil();
}

void ToggleReflections()
{
  g_game.OnToggleReflections();
}

void ToggleShadows()
{
  g_game.OnToggleShadows();
}

void ToggleSkyBox()
{
  g_game.OnToggleSkyBox();
}

void ToggleFreeCamera()
{
  g_game.OnToggleFreeCamera();
}

void ToggleEffects()
{
  g_game.OnToggleEffects();
}

void ToggleCollisions()
{
  g_game.OnToggleCollisions();
}

void ToggleAnimation()
{
  g_game.OnToggleAnimation();
}

void TogglePhysics()
{
  g_game.OnTogglePhysics();
}

void TriggerExit()
{
  g_game.OnTriggerExit();
}

//static game menu strings
cchar *Game::m_aMenuStrings [GAME_MENU_SIZE] =
{
  "Stencil     On/Off",
  "Reflections On/Off",
  "Shadows     On/Off",
  "Sky Box     On/Off",
  "Free Camera On/Off",
  "Effects     On/Off",
  "Collisions  On/Off",
  "Animation   On/Off",
  "Physics     On/Off",
  "Exit Game         ",
};

//static game key map
uchar Game::m_aKeyMap [NUM_ACTIONS] =
{
  'w',                        //ACTION_MOVE_FORW
  's',                        //ACTION_MOVE_BACK
  'a',                        //ACTION_MOVE_LEFT
  'd',                        //ACTION_MOVE_RIGHT
  ']',                        //ACTION_MOVE_UP
  '[',                        //ACTION_MOVE_DOWN
  '\\',                       //ACTION_Y_RESET
  (uchar)SYS_KEY_UP,          //ACTION_LOOK_UP
  (uchar)SYS_KEY_DOWN,        //ACTION_LOOK_DOWN
  (uchar)SYS_KEY_LEFT,        //ACTION_TURN_LEFT
  (uchar)SYS_KEY_RIGHT,       //ACTION_TURN_RIGHT
  '=',                        //ACTION_ZOOM_IN
  '-',                        //ACTION_ZOOM_OUT
  (uchar)KEY_SPACE,           //ACTION_ZOOM_RESET
  ' ',                        //ACTION_JUMP
  'l',                        //ACTION_FLASH_LIGHT
  (uchar)MOUSE_BUTTON_LEFT,   //ACTION_FIRE_1
  (uchar)MOUSE_BUTTON_RIGHT,  //ACTION_FIRE_2
  (uchar)MOUSE_WHEEL_UP,      //ACTION_WEAPON_NEXT
  (uchar)MOUSE_WHEEL_DOWN,    //ACTION_WEAPON_PREV
  '1',                        //ACTION_WEAPON_1
  '2',                        //ACTION_WEAPON_2
  '3',                        //ACTION_WEAPON_3
  '4',                        //ACTION_WEAPON_4
  '5',                        //ACTION_WEAPON_5
  '6',                        //ACTION_WEAPON_6
  '7',                        //ACTION_WEAPON_7
  '8',                        //ACTION_WEAPON_8
  '9',                        //ACTION_WEAPON_9
  (uchar)KEY_ESCAPE,          //ACTION_MENU
  (uchar)SYS_KEY_UP,          //ACTION_MENU_UP
  (uchar)SYS_KEY_DOWN,        //ACTION_MENU_DOWN
  (uchar)SYS_KEY_LEFT,        //ACTION_MENU_LEFT
  (uchar)SYS_KEY_RIGHT,       //ACTION_MENU_RIGHT
  (uchar)KEY_ENTER,           //ACTION_MENU_SELECT
};

NAMESPACE_END

USING_NAMESPACE

int main(int argc, char *argv[])
{
  DbgEnter();
  if (argc > 1 && 0 == strcmp(argv[1], "help"))
  {
    //output help string if necessary
    printf("Usage: %s\n", argv[0]);
    ConfigPrintUsage(stdout);
    return 0;
  }
  else if (argc > 1 && 0 == strcmp(argv[1], "dbg"))
  {
    //include debug info in messages
    Log::g_flags |= LOG_FLAGS_DBG;
  }
  //run the game
  return g_game.Run(argc, argv);
}
