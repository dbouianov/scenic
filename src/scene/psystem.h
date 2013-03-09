#ifndef _PSYSTEM_H_
#define _PSYSTEM_H_

#include "scenenode.h"
#include "algebra.h"
#include "displist.h"

NAMESPACE_BEGIN

#define MAX_NUM_PARTICLES  DEFAULT_NUM_PARTICLES

//single particle
struct Particle
{
  //particle life
  float life;
  //particle fade rate
  float fade;
  //particle position
  PointUnionPrefix(position, p);
  //particle velocity
  VectorUnionPrefix(velocity, v);
};

//particle system
class ParticleSystem : public SceneNode
{
public:
  ParticleSystem();
  virtual ~ParticleSystem();

  bool Create(SceneNode *pParticle,
              Color color,
              Vector velocity,
              Vector acceleration,
              uint count = DEFAULT_NUM_PARTICLES,
              bool reduceSize  = false,
              bool reduceAlpha = false,
              float maxSize = DEFAULT_PARTICLE_SIZE,
              float spread  = DEFAULT_PARTICLE_SPREAD,
              float minFade = DEFAULT_PARTICLE_MIN_FADE,
              float maxFade = DEFAULT_PARTICLE_MAX_FADE,
              float maxLife = DEFAULT_PARTICLE_MAX_LIFE);
  bool Destroy();
  bool IsValid() const;
  void Reset(bool random);

  virtual void Walk(ulong flags = NODE_FLAGS_PSYSTEM);
  virtual void Update(ulong frame, float dt, Vector gravity);

private:
  //particle display list
  DisplayList m_particle;
  //particle color
  ColorUnionPrefix(m_color, m_c);
  //initial particle velocity
  VectorUnionPrefix(m_velocity, m_v);
  //acceleration vector
  VectorUnionPrefix(m_accel, m_a);
  //size reduction indicator
  bool m_reduceSize;
  //alpha reduction indicator
  bool m_reduceAlpha;
  //max particle size
  float m_maxSize;
  //particle spread angle
  float m_spread;
  //min/max particle fade rate
  float m_minFade;
  float m_maxFade;
  //max particle life
  float m_maxLife;
  //number of particles
  uint m_count;
  //current frame
  ulong m_frame;
  //array of particles
  Particle m_aParticles [MAX_NUM_PARTICLES];

  //initialize single particle
  void InitParticle(OUT Particle &particle, bool random);
};

NAMESPACE_END

#endif //_PSYSTEM_H_
