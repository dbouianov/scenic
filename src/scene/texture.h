#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "scenenode.h"
#include "image.h"

NAMESPACE_BEGIN

#define BAD_TEXTURE   0

//texture
class Texture : public SceneNode
{
public:
  Texture();
  virtual ~Texture();

  bool Create(Image *pImage,
              bool repeat = false,
              bool mipmap = true,
              GLint magFilter = GL_LINEAR,
              GLint minFilter = GL_NEAREST_MIPMAP_LINEAR);
  bool Destroy();
  bool IsValid() const;

  GLuint GetTextureId() const;

  virtual void Walk(ulong flags = NODE_FLAGS_TEXTURE);

private:
  //texture id
  GLuint m_texture;

  //maximum possible texture size
  static GLint m_maxTextureSize;

  //get texture size which is a power of 2
  static uint GetSafeSize(uint size);
};

NAMESPACE_END

#endif //_TEXTURE_H_
