#include "material.h"

NAMESPACE_BEGIN

Material::Material()
: m_shininess (DEFAULT_SHININESS)
{
#ifdef _DEBUG
  SetName("material");
#endif
  ColorInit(m_diffuse,  1.0f, 1.0f, 1.0f, 1.0f);
  ColorInit(m_specular, 1.0f, 1.0f, 1.0f, 1.0f);
}

Material::~Material()
{}

void Material::SetDiffuse(float r, float g, float b, float a)
{
  ColorInit(m_diffuse, r, g, b, a);
}

void Material::SetDiffuse(Color c)
{
  ColorCopy(m_diffuse, c);
}

Color &Material::GetDiffuse()
{
  return m_diffuse;
}

void Material::SetSpecular(float r, float g, float b)
{
  ColorInit(m_specular, r, g, b, 1.0f);
}

void Material::SetSpecular(Color c)
{
  ColorCopy(m_specular, c);
}

Color &Material::GetSpecular()
{
  return m_specular;
}

void Material::SetShininess(float shininess)
{
  m_shininess = shininess;
}

float Material::GetShininess()
{
  return m_shininess;
}

void Material::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_MATERIAL)
  {
    //apply all material properties
    glColor4fv(m_diffuse);
    //glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  m_specular);
    glMaterialf (GL_FRONT, GL_SHININESS, m_shininess);
  }
  else if (flags & NODE_FLAGS_COLOR)
  {
    //apply material color only
    glColor4fv(m_diffuse);
  }
}

NAMESPACE_END
