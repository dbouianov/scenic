#include "render.h"
#include "loader.h"
#include "light.h"
//#include "image.h"
#include "displist.h"

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io_new/bmp_all.hpp>
#include <boost/gil/extension/io_new/png_all.hpp>
#include <boost/gil/extension/io_new/jpeg_all.hpp>
#include <boost/gil/extension/io_new/tiff_all.hpp>
#include <boost/scoped_array.hpp>

#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::gil;

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_MAIN, "Render",__FILE__);

Render::Render()
: m_pScene (NULL),
  m_pLight (NULL)
{
  //initialize configuration
  ConfigInit(&m_config);
}

int Render::Run(int argc, char *argv[])
{
  m_timer.Start();

  //load configuration parameters
  ConfigParse(&m_config, argc, argv);

  //initialize rendering context
  if (InitContext(m_config.width, m_config.height))
  {
    //initialize rendering state
    InitState();

    Timer frameTimer;
#if 0
    ShowMessage("Loading scene each frame");
    for (int i = 0; i < 5; ++i)
    {
      frameTimer.Start();

      //load scene
      LoadScene();
      SetView(m_config.width, m_config.height);

      //render scene
      DrawScene();

      char szFile [MAX_FILE_NAME_SIZE + 1];
      snprintf(szFile, MAX_FILE_NAME_SIZE, "frame%d" DEFAULT_IMAGE_EXT, i);
      szFile[MAX_FILE_NAME_SIZE] = NULL_CHAR;
      SaveImage(szFile);

      //perform garbage collection
      m_sceneFactory.DeleteObjects();
      m_renderFactory.DeleteObjects();
      m_textureFactory.DeleteObjects();

      frameTimer.Stop();
      ShowMessage("Frame in %f seconds", frameTimer.GetTime());
    }
#else
    ShowMessage("Loading scene once");
    LoadScene();
    SetView(m_config.width, m_config.height);
    for (int i = 0; i < 5; ++i)
    {
      frameTimer.Start();
      DrawScene();
      frameTimer.Stop();
      ShowMessage("Frame in %f seconds", frameTimer.GetTime());
    }
    SaveImage(m_config.szSceneImage);
    m_sceneFactory.DeleteObjects();
    m_renderFactory.DeleteObjects();
    m_textureFactory.DeleteObjects();
#endif
  }

  FreeContext();

  m_timer.Stop();
  ShowMessage("Finished in %f seconds", m_timer.GetTime());

  return 0;
}

bool Render::InitContext(int width, int height)
{
  return m_context.Create(width, height);
}

void Render::FreeContext()
{
  m_context.Destroy();
}

void Render::InitState()
{
  glClearColor(DEFAULT_CLEAR_COLOR);
  glClearDepth(DEFAULT_CLEAR_DEPTH);
  glEnable(GL_DEPTH_TEST);

  //enable back face culling
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  //specify polygon mode
  glPolygonMode(GL_FRONT, GL_FILL);

  //specify shading model
  glShadeModel(GL_SMOOTH);

  //enable color and material properties
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);

  //specify lighting model
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
  Color ambient = {DEFAULT_AMBIENT_COLOR};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  glEnable(GL_LIGHTING);

  //enable normalization of scaled normals
  glEnable(GL_NORMALIZE);

  //do not align pixel rows in frame buffer
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  //read images from renderbuffer
  glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

  //initialize projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glPushMatrix();

  //initialize modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //flip GL image along y-axis so it comes out right when saved into an image
  //since GL reads pixels from bottom left corner of the frame buffer
  glScalef(1.0f, -1.0f, 1.0f);

  //account for reversed polygon winding
  glFrontFace(GL_CW);

  //set camera parameters
  m_camera.SetViewFrustum(DEFAULT_FIELD_OF_VIEW, DEFAULT_ASPECT_RATIO,
    DEFAULT_NEAR_DISTANCE, DEFAULT_FAR_DISTANCE);
  m_camera.SetPosition(m_config.pos);
  m_camera.SetAngles(m_config.angleX, m_config.angleY);
}

void Render::LoadScene()
{
  //load scene
  DbgMessage("Loading scene");
  m_pScene = LoadLuaFile(m_config.szSceneFolder, DEFAULT_SCENE_FILE,
    m_sceneFactory, m_textureFactory);
  if (!m_pScene)
  {
    DbgError("Unable to load scene!");
    //replace scene with null node
    m_pScene = m_sceneFactory.GetNullNode();
  }
  else
  {
    DisplayList *pSceneCache = m_renderFactory.NewObject<DisplayList>();
    if (pSceneCache->Create(m_pScene))
    {
      //replace scene with display list
      m_pScene = pSceneCache;

      //perform garbage collection
      m_sceneFactory.DeleteObjects();
    }
  }

  //load light
  DbgMessage("Loading light");
  m_pLight = LoadLuaLight(m_config.szSkyFolder, DEFAULT_LIGHT_FILE, m_sceneFactory);
  if (!m_pLight)
  {
    DbgError("Unable to load light!");
    //replace light with null node
    m_pLight = m_sceneFactory.GetNullNode();
  }

  //apply light parameters
  m_pLight->Walk(NODE_FLAGS_LIGHT);
}

void Render::SetView(int width, int height)
{
  DbgMessage("width = %d, height = %d", width, height);

  //reset projection matrix to identity
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPushMatrix();

  //set GL viewport
  glViewport(0, 0, width, height);

  //apply camera view frustum transformation
  m_camera.SetAspectRatio(static_cast<float>(width) / height);
  m_camera.Walk(NODE_FLAGS_FRUSTUM);

  //update configuration
  m_config.width  = width;
  m_config.height = height;

  //switch to modelview matrix
  glMatrixMode(GL_MODELVIEW);
}

void Render::DrawScene()
{
  //clear color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //save modelview matrix
  glPushMatrix();

    //apply camera transformation
    m_camera.Walk(NODE_FLAGS_TRANSFORM);

    //apply light position
    m_pLight->Walk(NODE_FLAGS_TRANSFORM);

    //draw scene
    m_pScene->Walk(NODE_FLAGS_ALL);

  //restore modelview matrix
  glPopMatrix();
}

/*void Render::SaveImage(cchar *szFile)
{
  PngImage image;
  image.Create(m_config.width, m_config.height, IMAGE_BPP_8);

  glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
  glReadPixels(0, 0, m_config.width, m_config.height,
    GL_RGB, GL_UNSIGNED_BYTE, image.GetImageData());

  image.Image::Save(szFile);
}*/

void Render::SaveImage(cchar *szFile)
{
  scoped_array<GLubyte> pixels(new GLubyte [m_config.width * m_config.height * 3]);

  //get raw pixel data from GL
  glReadPixels(0, 0, m_config.width, m_config.height, GL_RGB, GL_UNSIGNED_BYTE, pixels.get());

  //construct a GIL interleaved image view
  rgb8_view_t image = interleaved_view(m_config.width, m_config.height,
    reinterpret_cast<rgb8_pixel_t *>(pixels.get()), m_config.width * sizeof(GLubyte) * 3);

  //save image in PNG format
  ofstream out(szFile, ios::out | ios::binary);
  write_view(out, image, png_tag());
}

NAMESPACE_END

USING_NAMESPACE

static Render g_render;

int main(int argc, char *argv[])
{
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
  //render scene
  return g_render.Run(argc, argv);
}
