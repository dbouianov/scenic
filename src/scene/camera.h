#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "scenenode.h"
#include "algebra.h"

NAMESPACE_BEGIN

class Camera : public SceneNode
{
public:
  Camera();
  virtual ~Camera();

  float GetFieldOfView() const;
  void  SetFieldOfView(float fov);

  float GetAspectRatio() const;
  void  SetAspectRatio(float aspect);

  float GetNearDistance() const;
  void  SetNearDistance(float near);

  float GetFarDistance() const;
  void  SetFarDistance(float far);

  void SetViewFrustum(float fov, float aspect, float near, float far);

  Point &GetPosition();
  void SetPosition(float x, float y, float z);
  void SetPosition(Point p);

  Vector &GetVelocity();
  void SetVelocity(float x, float y, float z);
  void SetVelocity(Vector v);

  float GetAngleX() const;
  void  SetAngleX(float degrees);

  float GetAngleY() const;
  void  SetAngleY(float degrees);

  void SetAngles(float degreesX, float degreesY);

  Vector &GetDirForward();
  Vector &GetDirRight();

  void LookAt(Point p);
  void LookAt(float x, float y, float z);

  void Look    (float deg);
  void LookUp  (float deg);
  void LookDown(float deg);

  void Turn     (float deg);
  void TurnLeft (float deg);
  void TurnRight(float deg);

  void MoveForward(float dist);
  void MoveBack   (float dist);
  void MoveRight  (float dist);
  void MoveLeft   (float dist);
  void MoveUp     (float dist);
  void MoveDown   (float dist);

  void ZoomIn (float zoom);
  void ZoomOut(float zoom);
  void ZoomOut();

  virtual void Walk  (ulong flags = NODE_FLAGS_TRANSFORM);
  virtual void Update(ulong frame, float dt, Vector gravity);

private:
  //current frame
  ulong m_frame;
  //camera position and velocity
  PointUnionPrefix(m_position, m_p);
  VectorUnionPrefix(m_velocity, m_v);
  //direction vectors
  VectorUnionPrefix(m_forward, m_f);
  VectorUnionPrefix(m_right, m_r);
  //field of view angle
  float m_fov;
  //viewing frustum aspect ratio
  float m_aspect;
  //near/far viewing frustum distances
  float m_near;
  float m_far;
  //direction angles
  float m_angleX;
  float m_angleY;
  //indicates whether direction vectors are valid
  bool m_dirBad;
  //camera zoom
  float m_zoom;
  //indicates whether zoom is enabled
  bool m_zoomOn;

  //check and calculate new direction vectors
  void CheckDir();
};

NAMESPACE_END

#endif //_CAMERA_H_
