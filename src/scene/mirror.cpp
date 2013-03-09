#include "mirror.h"

NAMESPACE_BEGIN

//static
GLenum Mirror::m_nextClipPlane = GL_CLIP_PLANE0;

Mirror::Mirror()
: m_pSurface (NULL)
{
#ifdef _DEBUG
  SetName("mirror");
#endif
  //zero out mirror data
  PlaneZero(m_plane);
  MemZero(m_planeGl, sizeof(m_planeGl));
  MatrixZero(m_matrix);
  //initialize clip plane id
  m_clipPlane = m_nextClipPlane++;
}

Mirror::~Mirror()
{}

bool Mirror::Create(SceneNode *pSurface, Plane plane)
{
  if (pSurface)
  {
    //save mirror surface
    m_pSurface = pSurface;
    //save mirror plane
    PlaneCopy(m_plane, plane);
    //initialize OpenGL plane equation
    m_planeGl[CX] = m_plane[CX];
    m_planeGl[CY] = m_plane[CY];
    m_planeGl[CZ] = m_plane[CZ];
    m_planeGl[CW] = m_plane[CW];
    //initialize reflection matrix
    MatrixReflection(m_matrix, m_plane);
    return true;
  }
  return false;
}

bool Mirror::Destroy()
{
  m_pSurface = NULL;
  return true;
}

bool Mirror::IsValid() const
{
  return static_cast<bool>(m_pSurface);
}

GLenum Mirror::GetClipPlaneId()
{
  return m_clipPlane;
}

GLdouble *Mirror::GetClipPlaneGl()
{
  return m_planeGl;
}

void Mirror::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_MIRROR)
  {
    if (m_pSurface)
    {
      //draw mirror surface
      m_pSurface->Walk(flags);
    }
  }
  else if (flags & NODE_FLAGS_TRANSFORM)
  {
    //apply reflection matrix
    glMultMatrixf(m_matrix);
  }
}

NAMESPACE_END
