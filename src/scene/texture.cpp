#include "texture.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_TEXTURE, "Texture",__FILE__);

//static
GLint Texture::m_maxTextureSize = (GLint)BAD_SIZE;

Texture::Texture()
: m_texture (BAD_TEXTURE)
{
#ifdef _DEBUG
  SetName("texture");
#endif
}

Texture::~Texture()
{
  if (m_texture != BAD_TEXTURE)
  {
    glDeleteTextures(1, &m_texture);
  }
}

bool Texture::Create(Image *pImage,
                     bool repeat, bool mipmap,
                     GLint magFilter, GLint minFilter)
{
  if (pImage && pImage->IsValid())
  {
    //create texture with power of two dimensions
    uint imageWidth  = pImage->GetWidth();
    uint imageHeight = pImage->GetHeight();
    uint textureWidth  = Texture::GetSafeSize(imageWidth);
    uint textureHeight = Texture::GetSafeSize(imageHeight);

    DbgMessage("Texture width = %d, height = %d", textureWidth, textureHeight);

    ubyte *pImageData = pImage->GetImageData();
    ubyte *pTextureData = pImageData;
    if (textureWidth != imageWidth || textureHeight != imageHeight)
    {
      //create texture from subimage with power of two dimensions
      //uint imageRowSize = imageWidth * IMAGE_NUM_COMPONENTS;
      uint textureRowSize = textureWidth * IMAGE_NUM_COMPONENTS;
      pTextureData = new ubyte [textureHeight * textureRowSize];
      /*for (uint row = 0; row < textureHeight; ++row)
      {
        //copy subimage with power of two dimensions into texture array
        MemCopy(&pTextureData[row * textureRowSize],
          &pImageData[row * imageRowSize], textureRowSize);
      }*/
      gluScaleImage(GL_RGB, imageWidth, imageHeight,
        GL_UNSIGNED_BYTE, pImageData, textureWidth, textureHeight,
        GL_UNSIGNED_BYTE, pTextureData);
    }
    //generate texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    if (!repeat)
    {
      //disable texture wrapping which is enabled by default
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    if (mipmap)
    {
      //generate texture mipmaps
      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, textureWidth, textureHeight,
        GL_RGB, GL_UNSIGNED_BYTE, pTextureData);
    }
    else
    {
      //generate texture
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pTextureData);
    }
    if (pTextureData != pImageData)
    {
      //destroy texture array if necessary
      delete [] pTextureData;
    }
    glBindTexture(GL_TEXTURE_2D, BAD_TEXTURE);
    return true;
  }
  return false;
}

bool Texture::Destroy()
{
  if (m_texture != BAD_TEXTURE)
  {
    //destroy texture only if it is valid
    glDeleteTextures(1, &m_texture);
    m_texture = BAD_TEXTURE;
    return true;
  }
  return false;
}

bool Texture::IsValid() const
{
  return m_texture != BAD_TEXTURE;
}

GLuint Texture::GetTextureId() const
{
  return m_texture;
}

void Texture::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_TEXTURE)
  {
    //apply texture
    glBindTexture(GL_TEXTURE_2D, m_texture);
  }
}

//static
uint Texture::GetSafeSize(uint size)
{
  if (static_cast<GLint>(BAD_SIZE) == m_maxTextureSize)
  {
    //initialize maximum texture size
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
  }
  //get first power of two which is less than specified size
  uint safeSize = m_maxTextureSize;
  while (safeSize > size) safeSize >>= 1;
  return safeSize;
}

NAMESPACE_END
