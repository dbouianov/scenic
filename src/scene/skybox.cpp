#include "skybox.h"

NAMESPACE_BEGIN

SkyBox::SkyBox()
{
#ifdef _DEBUG
  SetName("skybox");
#endif
  //initialize textures
  MemZero(m_aTextures, sizeof(m_aTextures));
}

SkyBox::~SkyBox()
{}

void SkyBox::SetTexture(uint face, Texture *pTexture)
{
  if (face < NUM_CUBE_FACES && pTexture)
  {
    m_aTextures[face] = pTexture->GetTextureId();
  }
}

void SkyBox::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_SKYBOX)
  {
    //oversize skybox faces to hide texture edges
    float size = DEFAULT_SKYBOX_SIZE;
#ifdef _DEBUG
    float overSize = DEFAULT_SKYBOX_SIZE * (1.0f - DEFAULT_SKYBOX_OVERSIZE);
#else
    float overSize = DEFAULT_SKYBOX_SIZE * (1.0f + DEFAULT_SKYBOX_OVERSIZE);
#endif
    //front face
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, m_aTextures[CUBE_FACE_FRONT]);
    glBegin(GL_QUADS);
      glNormal3f(0.0f, 0.0f, -1.0f);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(-overSize, -overSize, size);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-overSize, overSize, size);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(overSize, overSize, size);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(overSize, -overSize, size);
    glEnd();
    //bottom face
    glBindTexture(GL_TEXTURE_2D, m_aTextures[CUBE_FACE_BOTTOM]);
    glBegin(GL_QUADS);
      glNormal3f(0.0f, 1.0f, 0.0f);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(-overSize, -size, -overSize);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-overSize, -size, overSize);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(overSize, -size, overSize);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(overSize, -size, -overSize);
    glEnd();
    //back face
    glBindTexture(GL_TEXTURE_2D, m_aTextures[CUBE_FACE_BACK]);
    glBegin(GL_QUADS);
      glNormal3f(0.0f, 0.0f, 1.0f);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(-overSize, overSize, -size);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(-overSize, -overSize, -size);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(overSize, -overSize, -size);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(overSize, overSize, -size);
    glEnd();
    //top face
    glBindTexture(GL_TEXTURE_2D, m_aTextures[CUBE_FACE_TOP]);
    glBegin(GL_QUADS);
      glNormal3f(0.0f, -1.0f, 0.0f);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(-overSize, size, overSize);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(-overSize, size, -overSize);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(overSize, size, -overSize);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(overSize, size, overSize);
    glEnd();
    //right face
    glBindTexture(GL_TEXTURE_2D, m_aTextures[CUBE_FACE_RIGHT]);
    glBegin(GL_QUADS);
      glNormal3f(-1.0f, 0.0f, 0.0f);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(size, -overSize, overSize);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(size, overSize, overSize);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(size, overSize, -overSize);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(size, -overSize, -overSize);
    glEnd();
    //left face
    glBindTexture(GL_TEXTURE_2D, m_aTextures[CUBE_FACE_LEFT]);
    glBegin(GL_QUADS);
      glNormal3f(1.0f, 0.0f, 0.0f);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(-size, -overSize, -overSize);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-size, overSize, -overSize);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(-size, overSize, overSize);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(-size, -overSize, overSize);
    glEnd();
    //reset current texture
    glBindTexture(GL_TEXTURE_2D, BAD_TEXTURE);
  }
}

NAMESPACE_END
