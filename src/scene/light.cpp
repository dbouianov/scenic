#include "light.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_SCENE, "Light",__FILE__);

//static
GLenum Light::m_nextLight = GL_LIGHT0;

Light::Light()
: m_enabled  (false),
  m_exponent (0.0f),
  m_cutoff   (180.0f)
{
#ifdef _DEBUG
  SetName("light");
#endif
  VectorInit(m_position,  0.0f, 0.0f, 1.0f);
  VectorInit(m_direction, 0.0f, 0.0f, -1.0f);
  VectorInit(m_kd, 1.0f, 1.0f, 1.0f);
  VectorInit(m_ks, 1.0f, 1.0f, 1.0f);
  m_light = m_nextLight++;
}

Light::~Light()
{
  DbgMessage("Light %d destroyed", m_light);
  if (m_enabled)
  {
    DbgMessage("Light %d disabled", m_light);
    glDisable(m_light);
  }
}

void Light::SetPosition(float x, float y, float z, float w)
{
  VectorInit(m_position, x, y, z);
  m_position[CW] = w;
}

void Light::SetPosition(Vector p)
{
  VectorCopy(m_position, p);
}

Vector &Light::GetPosition()
{
  return m_position;
}

void Light::SetDirection(float x, float y, float z)
{
  VectorInit(m_direction, x, y, z);
}

void Light::SetDirection(Vector v)
{
  VectorCopy(m_direction, v);
}

Vector &Light::GetDirection()
{
  return m_direction;
}

void Light::SetDiffuseCoeff(float kr, float kg, float kb)
{
  VectorInit(m_kd, kr, kg, kb);
}

void Light::SetDiffuseCoeff(Vector kd)
{
  VectorCopy(m_kd, kd);
}

Vector &Light::GetDiffuseCoeff()
{
  return m_kd;
}

void Light::SetSpecularCoeff(float kr, float kg, float kb)
{
  VectorInit(m_ks, kr, kg, kb);
}

void Light::SetSpecularCoeff(Vector ks)
{
  VectorCopy(m_ks, ks);
}

Vector &Light::GetSpecularCoeff()
{
  return m_ks;
}

void Light::SetSpotExponent(float exponent)
{
  m_exponent = exponent;
}

float Light::GetSpotExponent() const
{
  return m_exponent;
}

void Light::SetSpotCutoff(float cutoff)
{
  m_cutoff = cutoff;
}

float Light::GetSpotCutoff() const
{
  return m_cutoff;
}

void Light::SetEnabled(bool enabled)
{
  if (enabled)
  {
    DbgMessage("Light %d enabled", m_light);
    glEnable(m_light);
  }
  else
  {
    DbgMessage("Light %d disabled", m_light);
    glDisable(m_light);
  }
  m_enabled = enabled;
}

void Light::Toggle()
{
  SetEnabled(!m_enabled);
}

bool Light::IsEnabled() const
{
  return m_enabled;
}

GLenum Light::GetLightId() const
{
  return m_light;
}

void Light::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_LIGHT)
  {
    //apply all light properties
    glLightfv(m_light, GL_POSITION, m_position);
    glLightfv(m_light, GL_SPOT_DIRECTION, m_direction);
    glLightf (m_light, GL_SPOT_EXPONENT, m_exponent);
    glLightf (m_light, GL_SPOT_CUTOFF, m_cutoff);
    glLightfv(m_light, GL_DIFFUSE,  m_kd);
    glLightfv(m_light, GL_SPECULAR, m_ks);
  }
  else if (flags & NODE_FLAGS_TRANSFORM)
  {
    //re-position the light
    glLightfv(m_light, GL_POSITION, m_position);
  }
}

NAMESPACE_END
