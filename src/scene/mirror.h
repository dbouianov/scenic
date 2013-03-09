#ifndef _MIRROR_H_
#define _MIRROR_H_

#include "scenenode.h"
#include "plane.h"

NAMESPACE_BEGIN

class Mirror : public SceneNode
{
public:
  Mirror();
  virtual ~Mirror();

  bool Create (SceneNode *pSurface, Plane plane);
  bool Destroy();
  bool IsValid() const;

  GLenum    GetClipPlaneId();
  GLdouble *GetClipPlaneGl();

  virtual void Walk(ulong flags = NODE_FLAGS_ALL);

private:
  //mirror surface
  SceneNode *m_pSurface;
  //mirror plane
  Plane    m_plane;
  GLdouble m_planeGl [DIM_PLANE];
  //reflection matrix
  Matrix m_matrix;
  //clip plane id
  GLenum m_clipPlane;

  //next clip plane id
  static GLenum m_nextClipPlane;
};

NAMESPACE_END

#endif //_MIRROR_H_
