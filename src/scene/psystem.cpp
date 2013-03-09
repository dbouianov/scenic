#include "psystem.h"
#include "primitive.h"

NAMESPACE_BEGIN

ParticleSystem::ParticleSystem()
: m_reduceSize  (false),
  m_reduceAlpha (false),
  m_maxSize (0.0f),
  m_spread  (0.0f),
  m_minFade (0.0f),
  m_maxFade (0.0f),
  m_maxLife (0.0f),
  m_count   (0),
  m_frame   (0)
{
#ifdef _DEBUG
  SetName("psystem");
#endif
  ColorZero(m_color);
  VectorZero(m_velocity);
  VectorZero(m_accel);
  MemZero(m_aParticles, sizeof(m_aParticles));
}

ParticleSystem::~ParticleSystem()
{
  //destroy particle display list
  m_particle.Destroy();
}

bool ParticleSystem::Create(SceneNode *pParticle,
                            Color color,
                            Vector velocity,
                            Vector accel,
                            uint count,
                            bool reduceSize,
                            bool reduceAlpha,
                            float maxSize,
                            float spread,
                            float minFade,
                            float maxFade,
                            float maxLife)
{
  if (pParticle && count <= MAX_NUM_PARTICLES)
  {
    //create particle display list
    if (m_particle.Create(pParticle))
    {
      //initialize color
      ColorCopy(m_color, color);
      //initialize particle velocity
      VectorCopy(m_velocity, velocity);
      //initialize acceleration
      VectorCopy(m_accel, accel);
      //initialize other parameters
      m_reduceSize  = reduceSize;
      m_reduceAlpha = reduceAlpha;
      m_maxSize = maxSize;
      m_spread  = spread;
      m_maxLife = maxLife;
      m_minFade = minFade;
      m_maxFade = maxFade;
      //initialize all particles
      m_count = count;
      Reset(true);
      return true;
    }
  }
  return false;
}

bool ParticleSystem::Destroy()
{
  //destroy particle display list
  m_particle.Destroy();
  m_count = 0;
  return true;
}

bool ParticleSystem::IsValid() const
{
  return m_count > 0;
}

void ParticleSystem::Reset(bool random)
{
  for (uint i = 0; i < m_count; ++i)
  {
    InitParticle(m_aParticles[i], random);
  }
}

void ParticleSystem::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_PSYSTEM)
  {
    //todo: perform intersection test with bounding volume
    //draw all particles
    for (uint i = 0; i < m_count; ++i)
    {
      Particle &particle = m_aParticles[i];
      //size and alpha depend on particle life
      float size = m_maxSize;
      if (m_reduceSize)
      {
        size *= particle.life;
      }
      float fAlpha = m_cA;
      if (m_reduceAlpha)
      {
        fAlpha *= particle.life;
      }
      if (flags & NODE_FLAGS_COLOR)
      {
        glColor4f(m_cR, m_cG, m_cB, fAlpha);
      }
      //save modelview matrix
      glPushMatrix();
        //position particle
        glTranslatef(particle.pX, particle.pY, particle.pZ);
        //scale particle
        glScalef(size, size, size);
        //draw particle
        m_particle.Walk(flags);
      //restore modelview matrix
      glPopMatrix();
    }
  }
}

void ParticleSystem::Update(ulong frame, float dt, Vector gravity)
{
  //avoid multiple updates within the same frame
  if (frame != m_frame)
  {
    //update all particles
    for (uint i = 0; i < m_count; ++i)
    {
      Particle &particle = m_aParticles[i];
      //reduce particle life
      particle.life -= particle.fade * dt;
      if (particle.life < 0.0f)
      {
        //respawn dead particle
        InitParticle(particle, true);
      }
      else
      {
        //update particle position
        particle.pX += particle.vX * dt;
        particle.pY += particle.vY * dt;
        particle.pZ += particle.vZ * dt;
        //update particle velocity
        //particles are not afected by world gravity in y-direction
        particle.vX += (m_aX + gravity[CX]) * dt;
        particle.vY +=  m_aY * dt;
        particle.vZ += (m_aZ + gravity[CZ]) * dt;
      }
    }
    //update current frame
    m_frame = frame;
  }
}

void ParticleSystem::InitParticle(Particle &particle, bool random)
{
  if (random)
  {
    //random life
    particle.life = RandomNumber(0.0f, m_maxLife);
  }
  else
  {
    //full life
    particle.life = m_maxLife;
  }
  //random fade between min and max fade
  particle.fade = RandomNumber(m_minFade, m_maxFade);
  //zero position
  PointZero(particle.position);
  //perturb initial velocity by spread factor
  float s = FastSin(m_spread);
  particle.vX = m_vX + s * RandomNumber(-1.0f, 1.0f);
  particle.vY = m_vY + s * RandomNumber(-1.0f, 1.0f);
  particle.vZ = m_vZ + s * RandomNumber(-1.0f, 1.0f);
}

NAMESPACE_END
