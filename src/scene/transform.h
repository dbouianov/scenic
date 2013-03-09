#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "scenenode.h"
#include "algebra.h"

NAMESPACE_BEGIN

//generic transform
class Transform : public SceneNode
{
protected:
  Transform();

public:
  virtual ~Transform() = 0;

  //apply node transformation to help find bounding volumes
  virtual bool GetBounds(OUT Bounds &bounds);
};

//translation transform
class Translation : public Transform
{
public:
  Translation();
  virtual ~Translation();

  void SetDeltaX(float dx);
  void SetDeltaY(float dy);
  void SetDeltaZ(float dz);

  void SetDeltas(float dx, float dy, float dz);
  void SetDeltas(Vector d);

  float GetDeltaX() const;
  float GetDeltaY() const;
  float GetDeltaZ() const;

  void GetDeltas(OUT Vector d) const;

  virtual void Walk(ulong flags = NODE_FLAGS_TRANSFORM);

private:
  //translation vector components
  float m_dx;
  float m_dy;
  float m_dz;
};

//scaling transform
class Scaling : public Transform
{
public:
  Scaling();
  virtual ~Scaling();

  void SetScaleX(float sx);
  void SetScaleY(float sy);
  void SetScaleZ(float sz);

  void SetScales(float sx, float sy, float sz);
  void SetScales(Vector s);

  float GetScaleX() const;
  float GetScaleY() const;
  float GetScaleZ() const;

  void GetScales(OUT Vector s) const;

  virtual void Walk(ulong flags = NODE_FLAGS_TRANSFORM);

private:
  //scaling factors
  float m_sx;
  float m_sy;
  float m_sz;
};

//rotation transform
class Rotation : public Transform
{
public:
  Rotation();
  virtual ~Rotation();

  void SetAxisX(float x);
  void SetAxisY(float y);
  void SetAxisZ(float z);

  void SetAxis(float x, float y, float z);
  void SetAxis(Vector u);

  void SetAngle(float deg);

  float GetAxisX() const;
  float GetAxisY() const;
  float GetAxisZ() const;

  void GetAxis(OUT Vector u) const;

  float GetAngle() const;

  virtual void Walk(ulong flags = NODE_FLAGS_TRANSFORM);

private:
  //rotation axis components
  float m_x;
  float m_y;
  float m_z;
  //rotation angle
  float m_angle;
};

NAMESPACE_END

#endif //_TRANSFORM_H_
