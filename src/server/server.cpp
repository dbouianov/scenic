#include "server.h"
#include "loader.h"
#include "light.h"
#include "timer.h"

#define THRIFT_SIMPLE_SERVER
#ifndef THRIFT_SIMPLE_SERVER
#include <protocol/TBinaryProtocol.h>
#include <server/TNonblockingServer.h>
#else
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#endif

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io_new/bmp_all.hpp>
#include <boost/gil/extension/io_new/png_all.hpp>
#include <boost/gil/extension/io_new/jpeg_all.hpp>
#include <boost/gil/extension/io_new/tiff_all.hpp>
#include <boost/scoped_array.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <sstream>
#include <csignal>
#include <iostream>

using namespace std;
using namespace boost;
using namespace boost::gil;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

namespace s  = scenic;
namespace po = boost::program_options;

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_MAIN, "Server",__FILE__);

//server constants
#define DEFAULT_THRIFT_PORT       9090
#define DEFAULT_IMAGE_WIDTH       640
#define DEFAULT_IMAGE_HEIGHT      480
#define MAX_IMAGE_WIDTH           800
#define MAX_IMAGE_HEIGHT          600

//static
ScenicHandler *ScenicHandler::g_pInstance = NULL;

void ScenicHandler::ExitHandler(int sig)
{
  DbgEnter();
  g_pInstance->OnExit();
  exit(sig);
}

void ScenicHandler::OnExit()
{
  DbgEnter();

  //destroy scene data and rendering context
  m_factory.DeleteObjects();
  m_context.Destroy();

  //output server stats
  ShowMessage("Total time: %lf seconds", m_totalTime);
  ShowMessage("Image count: %d", m_numImages);
  ShowMessage("Average time: %lf seconds/image", m_numImages ? m_totalTime / m_numImages : 0.0);
  ShowMessage("Request rate: %lf images/second", m_totalTime ? m_numImages / m_totalTime : 0.0);
}

ScenicHandler::ScenicHandler(uint width, uint height)
: m_width     (width),
  m_height    (height),
  m_totalTime (0.0),
  m_numImages (0)
{
  DbgEnter();

  //create rendering context
  InitContext(m_width, m_height);

  //register exit handler
  g_pInstance = this;
  signal(SIGINT,  &ExitHandler);
  signal(SIGABRT, &ExitHandler);
  signal(SIGTERM, &ExitHandler);
  signal(SIGQUIT, &ExitHandler);
}

ScenicHandler::~ScenicHandler()
{
  DbgEnter();
}

void ScenicHandler::Ping()
{
  ShowMessage("Ping");
}

void ScenicHandler::GetSceneImage(string &result,
                                  const ss::Scene  &sceneArgs,
                                  const ss::Camera &cameraArgs,
                                  const ss::Image  &imageArgs)
{
  ShowMessage("GetSceneImage");
  Timer timer;
  timer.Start();

  //check scene name
  if (sceneArgs.name.empty())
  {
    DbgError("Empty scene name!");
    throw runtime_error("Empty scene name");
  }

  //check if context is valid
  if (!m_context.IsValid())
  {
    DbgError("Invalid rendering context!");
    throw runtime_error("Invalid rendering context");
  }

  //use scene name as folder name
  string folder = DEFAULT_DATA_FOLDER;
  folder += "/" + sceneArgs.name;

  //load scene objects
  SceneNode *pScene = LoadScene(sceneArgs.name, folder);
  if (!pScene)
  {
    DbgError("Error loading scene %s!", sceneArgs.name.c_str());
    throw runtime_error("Error loading scene");
  }

  //load scene lights
  GroupNode *pLights = LoadLights(sceneArgs.name, folder);

  //initialize camera parameters
  if (!InitCamera(cameraArgs))
  {
    DbgError("Invalid camera parameters!");
    throw runtime_error("Invalid camera parameters");
  }

  //apply ambient color if present
  if (sceneArgs.__isset.ambient)
  {
    ApplyAmbient(sceneArgs.ambient);
  }

  //render scene image
  DrawScene(pScene, pLights, NODE_FLAGS_ALL);

  //encode scene image
  if (!SaveImage(result, imageArgs))
  {
    DbgError("Unable to encode image!");
    throw runtime_error("Error encoding image");
  }

  timer.Stop();
  ShowMessage("Finished in %lf seconds", timer.GetTime());

  m_totalTime += timer.GetTime();
  ++m_numImages;
}

ss::SceneNodeId ScenicHandler::GetSceneNode(const ss::Scene  &sceneArgs,
                                            const ss::Camera &cameraArgs,
                                            const int32_t x,
                                            const int32_t y)
{
  ShowMessage("GetSceneNode");
  Timer timer;
  timer.Start();

  //check scene name
  if (sceneArgs.name.empty())
  {
    DbgError("Empty scene name!");
    throw runtime_error("Empty scene name");
  }

  //check selection coordinates
  if (static_cast<uint>(x) >= m_width || static_cast<uint>(y) >= m_height)
  {
    DbgError("Invalid selection coordinates!");
    throw runtime_error("Invalid selection coordinates");
  }

  //check if context is valid
  if (!m_context.IsValid())
  {
    DbgError("Invalid rendering context!");
    throw runtime_error("Invalid rendering context");
  }

  //use scene name as folder name
  string folder = DEFAULT_DATA_FOLDER;
  folder += "/" + sceneArgs.name;

  //load scene objects
  SceneNode *pScene = LoadScene(sceneArgs.name, folder);
  if (!pScene)
  {
    DbgError("Error loading scene %s!", sceneArgs.name.c_str());
    throw runtime_error("Error loading scene");
  }

  //initialize camera parameters
  if (!InitCamera(cameraArgs))
  {
    DbgError("Invalid camera parameters!");
    throw runtime_error("Invalid camera parameters");
  }

  //initialize selection mode
  DbgMessage("Selecting node at x = %d, y = %d", x, y);
  InitSelection(x, y);

  //render scene in selection mode
  DrawScene(pScene, NULL,
    NODE_FLAGS_SELECT | NODE_FLAGS_TRANSFORM | NODE_FLAGS_PRIMITIVE | NODE_FLAGS_HALFLOD);

  //get selected node
  ss::SceneNodeId id = StopSelection();
  DbgMessage("Selected node: %d", id);

  timer.Stop();
  ShowMessage("Finished in %lf seconds", timer.GetTime());
  return id;
}

bool ScenicHandler::InitContext(uint width, uint height)
{
  DbgMessage("Context width %d, height %d", width, height);

  //check image dimensions
  if (width > MAX_IMAGE_WIDTH || height > MAX_IMAGE_HEIGHT)
  {
    DbgError("Invalid image dimensions!");
    return false;
  }

  //create context
  if (!m_context.Create(width, height))
  {
    DbgError("Unable to create context!");
    return false;
  }

  //initialize GL state
  InitState(width, height);
  return true;
}

void ScenicHandler::InitState(uint width, uint height)
{
  DbgEnter();

  //specify clear values for color and depth buffers
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

  //enable anti-aliasing for lines
  glHint(GL_LINE_SMOOTH, GL_FASTEST);
  glEnable(GL_LINE_SMOOTH);

  //enable anti-aliasing for polygons
  glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  glEnable(GL_POLYGON_SMOOTH);

  //enable color and material properties
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);

  //specify lighting model
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,  GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
  glEnable(GL_LIGHTING);

  //enable normalization of scaled normals
  glEnable(GL_NORMALIZE);

  //specify color buffer pixel alignment
  glPixelStorei(GL_PACK_ALIGNMENT,   DEFAULT_PIXEL_ALIGNMENT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, DEFAULT_PIXEL_ALIGNMENT);

  //initialize projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //set GL viewport
  glViewport(0, 0, width, height);

  //apply view frustum transformation
  m_camera.SetViewFrustum(DEFAULT_FIELD_OF_VIEW, static_cast<float>(width) / height,
    DEFAULT_NEAR_DISTANCE, DEFAULT_FAR_DISTANCE);
  m_camera.Walk(NODE_FLAGS_FRUSTUM);

  //initialize modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //GL reads pixels from bottom left of the frame buffer
  //image encoding functions expect pixels to start from upper left
  //flip objects along y-axis so the scene comes out right when read into an image
  glScalef(1.0f, -1.0f, 1.0f);

  //change polygon winding to clock-wize to account for reflected polygons
  glFrontFace(GL_CW);
}

SceneNode *ScenicHandler::LoadScene(const string &name, const string &folder)
{
  DbgMessage("Loading scene %s", name.c_str());

  //check if scene is already loaded
  SceneNodeMap::iterator it = m_scenes.find(name);
  if (it != m_scenes.end())
  {
    //scene is already loaded
    return it->second;
  }

  //texture factory has scene lifetime
  Factory *pTextures = m_factory.NewObject<Factory>();

  //load scene objects
  SceneNode *pScene =
    LoadLuaFile(folder.c_str(), DEFAULT_SCENE_FILE, m_factory, *pTextures);
  if (!pScene)
  {
    DbgError("Unable to load scene %s!", name.c_str());
    //delete texture factory on error
    m_factory.DeleteObject(pTextures);
    return NULL;
  }

  //optimize scene nodes
  pScene->Optimize();

  //save scene and textures
  m_scenes  [name] = pScene;
  m_textures[name] = pTextures;
  return pScene;
}

GroupNode *ScenicHandler::LoadLights(const string &name, const string &folder)
{
  DbgMessage("Loading lights for scene %s", name.c_str());

  //check if lights are already loaded
  GroupNodeMap::iterator it = m_lights.find(name);
  if (it != m_lights.end())
  {
    //lights are already loaded
    return it->second;
  }

  //load scene lights
  GroupNode *pLights = dynamic_cast<GroupNode *>(
    LoadLuaFile(folder.c_str(), DEFAULT_LIGHT_FILE, m_factory, m_factory));
  if (!pLights)
  {
    DbgWarning("Unable to load lights for scene %s!", name.c_str());
    return NULL;
  }

  //save scene lights
  m_lights[name] = pLights;
  return pLights;
}

bool ScenicHandler::InitCamera(const ss::Camera &cameraArgs)
{
  m_camera.SetPosition(cameraArgs.pos.x, cameraArgs.pos.y, cameraArgs.pos.z);
  if (cameraArgs.__isset.lookAt)
  {
    m_camera.LookAt(cameraArgs.lookAt.x, cameraArgs.lookAt.y, cameraArgs.lookAt.z);
    return true;
  }
  else if (cameraArgs.__isset.angleX && cameraArgs.__isset.angleY)
  {
    m_camera.SetAngles(cameraArgs.angleX, cameraArgs.angleY);
    return true;
  }
  return false;
}

void ScenicHandler::ApplyAmbient(const ss::Color &ambient)
{
  Color color;
  ColorInit(color, ambient.r, ambient.g, ambient.b, 1.0f);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color);
}

void ScenicHandler::ToggleLights(GroupNode *pLights, bool on)
{
  if (pLights)
  {
    //go through child nodes
    SceneNodeList &nodes = pLights->GetNodeList();
    for (SceneNodeList::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
      //check for light
      Light *pLight = dynamic_cast<Light *>(*it);
      if (pLight)
      {
        //apply light parameters when turning the light on
        if (on)
        {
          pLight->Walk();
        }
        pLight->SetEnabled(on);
      }
    }
  }
}

void ScenicHandler::DrawScene(SceneNode *pScene, GroupNode *pLights, ulong flags)
{
  DbgEnter();
  DbgAssert(pScene);

  //clear color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //save modelview matrix
  glPushMatrix();

    //apply camera transformation
    m_camera.Walk(NODE_FLAGS_TRANSFORM);

    if (flags & NODE_FLAGS_LIGHT)
    {
      //turn scene lights on
      ToggleLights(pLights, true);
    }

    //draw scene
    pScene->Walk(flags);

    if (flags & NODE_FLAGS_LIGHT)
    {
      //turn scene lights off
      ToggleLights(pLights, false);
    }

  //restore modelview matrix
  glPopMatrix();
}

bool ScenicHandler::SaveImage(string &result, const ss::Image &imageArgs)
{
  DbgEnter();
  scoped_array<GLubyte> pixels(new GLubyte [m_width * m_height * 3]);

  //get raw pixel data from GL
  glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, pixels.get());

  //construct a GIL interleaved image view
  rgb8_view_t image = interleaved_view(m_width, m_height,
    reinterpret_cast<rgb8_pixel_t *>(pixels.get()), m_width * sizeof(GLubyte) * 3);

  //open memory output stream
  stringstream out(ios::out | ios::binary);

  //encode image in provided format
  ss::ImageType::type type = imageArgs.__isset.type ? imageArgs.type : ss::ImageType::JPEG;
  if (ss::ImageType::JPEG == type)
  {
    static const int MAX_QUALITY = 100;
    int quality = MAX_QUALITY;
    if (imageArgs.__isset.quality && imageArgs.quality > 0 && imageArgs.quality < MAX_QUALITY)
    {
      quality = imageArgs.quality;
    }
    DbgMessage("Encoding JPEG image at %d%% quality", quality);
    write_view(out, image, image_write_info<jpeg_tag>(quality));
  }
  else if (ss::ImageType::PNG == type)
  {
    DbgMessage("Encoding PNG image");
    write_view(out, image, png_tag());
  }
  else if (ss::ImageType::BMP == type)
  {
    DbgMessage("Encoding BMP image");
    write_view(out, image, bmp_tag());
  }
  else if (ss::ImageType::TIFF == type)
  {
    DbgMessage("Encoding TIFF image");
    write_view(out, image, tiff_tag());
  }
  else
  {
    DbgError("Invalid image type %d", type);
    return false;
  }

  //assign encoded image to result without incurring a copy
  result = out.str();
  return true;
}

void ScenicHandler::InitSelection(uint x, uint y)
{
  //specify selection buffer
  glSelectBuffer(SELECT_BUFFER_SIZE, m_selectBuffer);

  //switch to selection mode
  glRenderMode(GL_SELECT);

  //save and reset projection matrix to identity
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  //define selection mode view volume
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  gluPickMatrix(x, viewport[3] - y, 5, 5, viewport);
  m_camera.Walk(NODE_FLAGS_FRUSTUM);

  //switch to modelview matrix
  glMatrixMode(GL_MODELVIEW);

  //initialize selection mode name stack
  glInitNames();
}

uint ScenicHandler::StopSelection()
{
  //restore projection matrix
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  //switch to modelview matrix
  glMatrixMode(GL_MODELVIEW);

  //switch to render mode
  int numSelectHits = glRenderMode(GL_RENDER);
  DbgMessage("Selection hit count = %d", numSelectHits);

  //process selection hits
  GLuint *pHit = m_selectBuffer;
  GLuint minZ  = 0xffffffff;
  GLuint *pClosestIds = NULL;
  for (int i = 0; i < numSelectHits; ++i)
  {
    GLuint numIds = *pHit++;
    if (numIds && *pHit < minZ)
    {
      //found closer hit
      minZ = *pHit;
      pClosestIds = pHit + 2;
      DbgMessage("Closest selection hit id = %d", pClosestIds[0]);
    }
    pHit += numIds + 2;
  }

  if (pClosestIds)
  {
    //return first hit id
    return pClosestIds[0];
  }

  //no hits found
  return 0;
}

NAMESPACE_END

USING_NAMESPACE

int main(int argc, char **argv)
{
  //parse command line arguments
  s::uint port, width, height;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h",    "show this help message")
    ("version,v", "show program version")
    ("port",   po::value<s::uint>(&port)->default_value  (DEFAULT_THRIFT_PORT),  "specify server port")
    ("width",  po::value<s::uint>(&width)->default_value (DEFAULT_IMAGE_WIDTH),  "specify image width")
    ("height", po::value<s::uint>(&height)->default_value(DEFAULT_IMAGE_HEIGHT), "specify image height")
    ;
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);

  if (vm.count("help") || width > MAX_IMAGE_WIDTH || height > MAX_IMAGE_WIDTH)
  {
    //output help message
    cout << desc << endl;
    return 0;
  }
  else if (vm.count("version"))
  {
    //output server version
    cout << "Scenic Server " << MAJOR_VERSION << "." << MINOR_VERSION << endl;
    return 0;
  }

  //output configuration
  cout << "Server port:  " << port   << endl;
  cout << "Image width:  " << width  << endl;
  cout << "Image height: " << height << endl;

  //create server
  boost::shared_ptr<ScenicHandler> handler  (new ScenicHandler(width, height));
  boost::shared_ptr<TProcessor>    processor(new ss::ScenicProcessor(handler));
#ifndef THRIFT_SIMPLE_SERVER
  TNonblockingServer server(processor, port);
#else
  boost::shared_ptr<TServerTransport>  serverTransport (new TServerSocket(port));
  boost::shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
  boost::shared_ptr<TProtocolFactory>  protocolFactory (new TBinaryProtocolFactory());
  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
#endif

  //run server
  server.serve();
  return 0;
}
