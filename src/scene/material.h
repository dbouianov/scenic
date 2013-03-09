#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "scenenode.h"
#include "algebra.h"

NAMESPACE_BEGIN

class Material : public SceneNode
{
public:
  Material();
  virtual ~Material();

  void SetDiffuse(float r, float g, float b, float a = 1.0f);
  void SetDiffuse(Color c);
  Color &GetDiffuse();

  void SetSpecular(float r, float g, float b);
  void SetSpecular(Color c);
  Color &GetSpecular();

  void  SetShininess(float s);
  float GetShininess();

  virtual void Walk(ulong flags = NODE_FLAGS_MATERIAL);

private:
  Color m_diffuse;
  Color m_specular;
  float m_shininess;
};

NAMESPACE_END

#endif //_MATERIAL_H_
