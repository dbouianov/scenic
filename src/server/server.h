#ifndef _SERVER_H_
#define _SERVER_H_

#include "context.h"
#include "camera.h"
#include "groupnode.h"
#include "factory.h"
#include "Scenic.h"
#include <string>
#include <unordered_map>

namespace ss = scenic::server;

NAMESPACE_BEGIN

typedef std::unordered_map<std::string, SceneNode *> SceneNodeMap;
typedef std::unordered_map<std::string, GroupNode *> GroupNodeMap;
typedef std::unordered_map<std::string, Factory *>   FactoryMap;

class ScenicHandler : virtual public ss::ScenicIf
{
public:
  ScenicHandler(uint width, uint height);
  virtual ~ScenicHandler();

  void Ping();

  void GetSceneImage(std::string &result,
                     const ss::Scene  &sceneArgs,
                     const ss::Camera &cameraArgs,
                     const ss::Image  &imageArgs);

  ss::SceneNodeId GetSceneNode(const ss::Scene  &sceneArgs,
                               const ss::Camera &cameraArgs,
                               const int32_t x,
                               const int32_t y);

private:
  Context m_context;
  Factory m_factory;
  Camera  m_camera;

  SceneNodeMap m_scenes;
  GroupNodeMap m_lights;
  FactoryMap   m_textures;

  //scene image dimensions
  uint m_width;
  uint m_height;

  //server stats
  double m_totalTime;
  uint   m_numImages;

  //selection mode state
  static const size_t SELECT_BUFFER_SIZE = 512;
  GLuint m_selectBuffer[SELECT_BUFFER_SIZE];

  //todo: replace exit handler with lambda expression after compiler upgrade
  static ScenicHandler *g_pInstance;
  static void ExitHandler(int sig);
  void OnExit();

  //initialization methods
  bool InitContext(uint width, uint height);
  void InitState  (uint width, uint height);

  //loader methods
  SceneNode *LoadScene (const std::string &name, const std::string &folder);
  GroupNode *LoadLights(const std::string &name, const std::string &folder);

  //lighting methods
  void ApplyAmbient(const ss::Color &ambient);
  void ToggleLights(GroupNode *pLights, bool on);

  //render mode methods
  bool InitCamera(const ss::Camera &cameraArgs);
  void DrawScene (SceneNode *pScene, GroupNode *pLights, ulong flags);
  bool SaveImage (std::string &result, const ss::Image &imageArgs);

  //selection mode methods
  void InitSelection(uint x, uint y);
  uint StopSelection();
};

NAMESPACE_END

#endif //_SERVER_H_
