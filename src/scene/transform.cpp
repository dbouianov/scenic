#include "transform.h"

NAMESPACE_BEGIN

Transform::Transform()
{}

Transform::~Transform()
{}

bool Transform::GetBounds(Bounds &)
{
  //apply node transformation to help find bounding volume
  Walk();
  return false;
}

Translation::Translation()
: m_dx (0),
  m_dy (0),
  m_dz (0)
{
#ifdef _DEBUG
  SetName("translation");
#endif
}

Translation::~Translation()
{}

void Translation::SetDeltaX(float dx)
{
  m_dx = dx;
}

void Translation::SetDeltaY(float dy)
{
  m_dy = dy;
}

void Translation::SetDeltaZ(float dz)
{
  m_dz = dz;
}

void Translation::SetDeltas(float dx, float dy, float dz)
{
  m_dx = dx;
  m_dy = dy;
  m_dz = dz;
}

void Translation::SetDeltas(Vector d)
{
  m_dx = d[CX];
  m_dy = d[CY];
  m_dz = d[CZ];
}

float Translation::GetDeltaX() const
{
  return m_dx;
}

float Translation::GetDeltaY() const
{
  return m_dy;
}

float Translation::GetDeltaZ() const
{
  return m_dz;
}

void Translation::GetDeltas(Vector d) const
{
  d[CX] = m_dx;
  d[CY] = m_dy;
  d[CZ] = m_dz;
}

void Translation::Walk(ulong)
{
  glTranslatef(m_dx, m_dy, m_dz);
}

Scaling::Scaling()
: m_sx (1.0f),
  m_sy (1.0f),
  m_sz (1.0f)
{
#ifdef _DEBUG
  SetName("scaling");
#endif
}

Scaling::~Scaling()
{}

void Scaling::SetScaleX(float sx)
{
  m_sx = sx;
}

void Scaling::SetScaleY(float sy)
{
  m_sy = sy;
}

void Scaling::SetScaleZ(float sz)
{
  m_sz = sz;
}

void Scaling::SetScales(float sx, float sy, float sz)
{
  m_sx = sx;
  m_sy = sy;
  m_sz = sz;
}

void Scaling::SetScales(Vector s)
{
  m_sx = s[CX];
  m_sy = s[CY];
  m_sz = s[CZ];
}

float Scaling::GetScaleX() const
{
  return m_sx;
}

float Scaling::GetScaleY() const
{
  return m_sy;
}

float Scaling::GetScaleZ() const
{
  return m_sz;
}

void Scaling::GetScales(Vector s) const
{
  s[CX] = m_sx;
  s[CY] = m_sy;
  s[CZ] = m_sz;
}

void Scaling::Walk(ulong)
{
  glScalef(m_sx, m_sy, m_sz);
}

Rotation::Rotation()
: m_x     (0.0f),
  m_y     (0.0f),
  m_z     (1.0f),
  m_angle (0.0f)
{
#ifdef _DEBUG
  SetName("rotation");
#endif
}

Rotation::~Rotation()
{}

void Rotation::SetAxisX(float x)
{
  m_x = x;
}

void Rotation::SetAxisY(float y)
{
  m_y = y;
}

void Rotation::SetAxisZ(float z)
{
  m_z = z;
}

void Rotation::SetAxis(float x, float y, float z)
{
  m_x = x;
  m_y = y;
  m_z = z;
}

void Rotation::SetAxis(Vector u)
{
  m_x = u[CX];
  m_y = u[CY];
  m_z = u[CZ];
}

void Rotation::SetAngle(float deg)
{
  m_angle = deg;
}

float Rotation::GetAxisX() const
{
  return m_x;
}

float Rotation::GetAxisY() const
{
  return m_y;
}

float Rotation::GetAxisZ() const
{
  return m_z;
}

void Rotation::GetAxis(Vector u) const
{
  u[CX] = m_x;
  u[CY] = m_y;
  u[CZ] = m_z;
}

float Rotation::GetAngle() const
{
  return m_angle;
}

void Rotation::Walk(ulong)
{
  glRotatef(m_angle, m_x, m_y, m_z);
}

NAMESPACE_END
