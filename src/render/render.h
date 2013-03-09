#ifndef _RENDER_H_
#define _RENDER_H_

#include "config.h"
#include "factory.h"
#include "camera.h"
#include "timer.h"
#include "context.h"

NAMESPACE_BEGIN

class Render
{
public:
  Render();

  //render scene
  int Run(int argc, char *argv[]);

private:
  Config  m_config;
  Factory m_sceneFactory;
  Factory m_renderFactory;
  Factory m_textureFactory;
  Camera  m_camera;
  Timer   m_timer;
  Context m_context;

  SceneNode *m_pScene;
  SceneNode *m_pLight;

  bool InitContext(int width, int height);
  void FreeContext();

  void InitState();
  void LoadScene();
  void DrawScene();
  void SaveImage(cchar *szFile);

  void SetView(int width, int height);
};

NAMESPACE_END

#endif //_RENDER_H_
