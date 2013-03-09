#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "scenenode.h"
#include "primitive.h"
#include "texture.h"

NAMESPACE_BEGIN

class SkyBox : public SceneNode
{
public:
  SkyBox();
  virtual ~SkyBox();

  void SetTexture(uint face, Texture *pTexture);

  virtual void Walk(ulong flags = NODE_FLAGS_SKYBOX);

private:
  GLuint m_aTextures [NUM_CUBE_FACES];
};

NAMESPACE_END

#endif //_SKYBOX_H_
