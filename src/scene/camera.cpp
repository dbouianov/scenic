#include "camera.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_CAMERA, "Camera",__FILE__);

Camera::Camera()
: m_frame  (0),
  m_fov    (DEFAULT_FIELD_OF_VIEW),
  m_aspect (DEFAULT_ASPECT_RATIO),
  m_near   (DEFAULT_NEAR_DISTANCE),
  m_far    (DEFAULT_FAR_DISTANCE),
  m_angleX (0.0f),
  m_angleY (0.0f),
  m_dirBad (false),
  m_zoom   (0.0f),
  m_zoomOn (false)
{
#ifdef _DEBUG
  SetName("camera");
#endif
  PointZero(m_position);
  VectorZero(m_velocity);
  //default camera direction starts at negative z-axis
  VectorInit(m_forward, 0.0f, 0.0f, -1.0f);
  VectorAxisX(m_right);
}

Camera::~Camera()
{}

float Camera::GetFieldOfView() const
{
  return m_fov;
}

void Camera::SetFieldOfView(float fov)
{
  m_fov = fov;
}

float Camera::GetAspectRatio() const
{
  return m_aspect;
}

void Camera::SetAspectRatio(float aspect)
{
  m_aspect = aspect;
}

float Camera::GetNearDistance() const
{
  return m_near;
}

void Camera::SetNearDistance(float near)
{
  m_near = near;
}

float Camera::GetFarDistance() const
{
  return m_far;
}

void Camera::SetFarDistance(float far)
{
  m_far = far;
}

void Camera::SetViewFrustum(float fov, float aspect, float near, float far)
{
  m_fov    = fov;
  m_aspect = aspect;
  m_near   = near;
  m_far    = far;
}

Point &Camera::GetPosition()
{
  return m_position;
}

void Camera::SetPosition(float x, float y, float z)
{
  m_pX = x;
  m_pY = y;
  m_pZ = z;
}

void Camera::SetPosition(Point p)
{
  PointCopy(m_position, p);
}

Vector &Camera::GetVelocity()
{
  return m_velocity;
}

void Camera::SetVelocity(float x, float y, float z)
{
  m_vX = x;
  m_vY = y;
  m_vZ = z;
}

void Camera::SetVelocity(Vector v)
{
  VectorCopy(m_velocity, v);
}

float Camera::GetAngleX() const
{
  return m_angleX;
}

void Camera::SetAngleX(float deg)
{
  m_angleX = deg;
}

float Camera::GetAngleY() const
{
  return m_angleY;
}

void Camera::SetAngleY(float deg)
{
  m_angleY = deg;
  //rotation about y-axis invalidates direction vectors
  m_dirBad = true;
}

void Camera::SetAngles(float degX, float degY)
{
  m_angleX = degX;
  m_angleY = degY;
  //rotation about y-axis invalidates direction vectors
  m_dirBad = true;
}

Vector &Camera::GetDirForward()
{
  return m_forward;
}

Vector &Camera::GetDirRight()
{
  return m_right;
}

void Camera::LookAt(Point p)
{
  //new camera direction vector is the difference between look point and camera position
  Vector dir;
  PointSubtractPoint(p, m_position, dir);

  //angle about x-axis = angle between yz-plane projection vector and negative z-axis
  //angle about y-axis = angle between zx-plane projection vector and negative z-axis
  Vector yzProj, zxProj;
  VectorInit(yzProj, 0.0f, dir[CY], dir[CZ]);
  VectorInit(zxProj, dir[CX], 0.0f, dir[CZ]);

  //cos angleX = yzProj dot nzAxis / |yzProj| |nzAxis| = -yzProj[CZ] / |yzProj|
  //cos angleY = zxProj dot nzAxis / |zxProj| |nzAxis| = -zxProj[CZ] / |zxProj|
  float yzProjLen = VectorLength(yzProj);
  float zxProjLen = VectorLength(zxProj);
  float angleX = FloatIsZero(yzProjLen) ? 0.0f : RadToDeg(acos(-yzProj[CZ] / yzProjLen));
  float angleY = FloatIsZero(zxProjLen) ? 0.0f : RadToDeg(acos(-zxProj[CZ] / zxProjLen));

  SetAngles(angleX, angleY);
}

void Camera::LookAt(float x, float y, float z)
{
  Point p;
  PointInit(p, x, y, z);
  LookAt(p);
}

void Camera::Look(float deg)
{
  deg = m_angleX + deg;
  if (deg < -DEG_90)
  {
    deg = -DEG_90;
  }
  else if (deg > DEG_90)
  {
    deg = DEG_90;
  }
  m_angleX = deg;
}

void Camera::LookUp(float deg)
{
  deg = m_angleX - deg;
  if (deg < -DEG_90)
  {
    deg = -DEG_90;
  }
  m_angleX = deg;
}

void Camera::LookDown(float deg)
{
  deg = m_angleX + deg;
  if (deg > DEG_90)
  {
    deg = DEG_90;
  }
  m_angleX = deg;
}

void Camera::Turn(float deg)
{
  m_angleY += deg;
  //rotation about y-axis invalidates direction vectors
  m_dirBad = true;
}

void Camera::TurnLeft(float deg)
{
  Turn(-deg);
}

void Camera::TurnRight(float deg)
{
  Turn(deg);
}

void Camera::MoveForward(float dist)
{
  CheckDir();
  m_pX += dist * m_fX;
  m_pZ += dist * m_fZ;
  //reset camera velocity
  m_vX = m_vZ = 0.0f;
}

void Camera::MoveBack(float dist)
{
  CheckDir();
  m_pX -= dist * m_fX;
  m_pZ -= dist * m_fZ;
  //reset camera velocity
  m_vX = m_vZ = 0.0f;
}

void Camera::MoveRight(float dist)
{
  CheckDir();
  m_pX += dist * m_rX;
  m_pZ += dist * m_rZ;
  //reset camera velocity
  m_vX = m_vZ = 0.0f;
}

void Camera::MoveLeft(float dist)
{
  CheckDir();
  m_pX -= dist * m_rX;
  m_pZ -= dist * m_rZ;
  //reset camera velocity
  m_vX = m_vZ = 0.0f;
}

void Camera::MoveUp(float dist)
{
  m_pY += dist;
}

void Camera::MoveDown(float dist)
{
  m_pY -= dist;
}

void Camera::ZoomIn(float zoom)
{
  if (zoom > 0.0f)
  {
    m_zoom += zoom;
    m_zoomOn = true;
  }
}

void Camera::ZoomOut(float zoom)
{
  if (m_zoom > 0.0f)
  {
    zoom = m_zoom - zoom;
    if (zoom > 0.0f)
    {
      m_zoom = zoom;
      m_zoomOn = true;
    }
    else
    {
      ZoomOut();
    }
  }
}

void Camera::ZoomOut()
{
  m_zoom = 0.0f;
  m_zoomOn = false;
}

void Camera::CheckDir()
{
  if (m_dirBad)
  {
    //calculate new unit direction vectors
    float c = FastCos(m_angleY);
    float s = FastSin(m_angleY);
    m_fZ = -c;
    m_fX = s;
    m_rZ = s;
    m_rX = c;
    DbgMessage("Forward: %f, %f, %f", m_fX, m_fY, m_fZ);
    m_dirBad = false;
  }
}

void Camera::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_TRANSFORM)
  {
    //set camera transformation
    if (m_zoomOn)
    {
      glTranslatef(0.0f, 0.0f, m_zoom);
    }
    glRotatef(m_angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(m_angleY, 0.0f, 1.0f, 0.0f);
    glTranslatef(-m_pX, -m_pY, -m_pZ);
  }
  else if (flags & NODE_FLAGS_FRUSTUM)
  {
    //set perspective transformation
    gluPerspective(m_fov, m_aspect, m_near, m_far);
    /*float fov = DEFAULT_FIELD_OF_VIEW * 0.5f;
    float h = DEFAULT_NEAR_DISTANCE * FastSin(fov) / FastCos(fov);
    float w = h * width / height;
    glFrustum(w, -w, h, -h, DEFAULT_NEAR_DISTANCE, DEFAULT_FAR_DISTANCE);*/
  }
}

void Camera::Update(ulong frame, float dt, Vector gravity)
{
  //avoid multiple updates within the same frame
  if (frame != m_frame)
  {
    //update camera position
    m_pX += m_vX * dt;
    m_pY += m_vY * dt;
    m_pZ += m_vZ * dt;
    //update camera velocity
    m_vX += gravity[CX] * dt;
    m_vY += gravity[CY] * dt;
    m_vZ += gravity[CZ] * dt;
    //update current frame
    m_frame = frame;
  }
}

NAMESPACE_END
