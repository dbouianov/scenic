#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "scenenode.h"
#include "algebra.h"

NAMESPACE_BEGIN

class Light : public SceneNode
{
public:
  Light();
  virtual ~Light();

  void SetPosition(float x, float y, float z, float w = 0.0f);
  void SetPosition(Vector p);
  Vector &GetPosition();

  void SetDirection(float x, float y, float z);
  void SetDirection(Vector v);
  Vector &GetDirection();

  void SetDiffuseCoeff(float kr, float kg, float kb);
  void SetDiffuseCoeff(Vector kd);
  Vector &GetDiffuseCoeff();

  void SetSpecularCoeff(float kr, float kg, float kb);
  void SetSpecularCoeff(Vector ks);
  Vector &GetSpecularCoeff();

  void  SetSpotExponent(float exp);
  float GetSpotExponent() const;

  void  SetSpotCutoff(float cutoff);
  float GetSpotCutoff() const;

  void SetEnabled(bool enabled);
  void Toggle();
  bool IsEnabled() const;

  GLenum GetLightId() const;

  virtual void Walk(ulong flags = NODE_FLAGS_LIGHT);

private:
  //light index
  GLenum m_light;
  bool m_enabled;
  //light position and direction
  Vector m_position;
  Vector m_direction;
  //light properties
  Vector m_kd;
  Vector m_ks;
  //spot light properties
  float m_exponent;
  float m_cutoff;

  //next light id
  static GLenum m_nextLight;
};

NAMESPACE_END

#endif //_LIGHT_H_
