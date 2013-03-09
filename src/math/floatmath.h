#ifndef _FLOATMATH_H_
#define _FLOATMATH_H_

#define _USE_MATH_DEFINES
#include "common.h"

NAMESPACE_BEGIN

//common math constants
#define PI                  ((float)M_PI)
#define TWO_PI              ((float)M_PI * 2.0)
#define PI_OVER_2           ((float)M_PI_2)
#define PI_OVER_4           ((float)M_PI_4)
#define ONE_OVER_PI         ((float)M_1_PI)
#define TWO_OVER_PI         ((float)M_2_PI)
#define SQRT_2              1.4142136f
#define SQRT_3              1.7320508f
#define SQRT_2_OVER_2       0.7071068f
#define SQRT_3_OVER_2       0.5773503f
#define FLOAT_EPSILON       0.000001f
#define FLOAT_0             0.0f
#define DEG_0               0.0f
#define DEG_90              90.0f
#define DEG_180             180.0f
#define DEG_270             270.0f
#define DEG_360             360.0f
#define DEG_PRECISION       0.1f
#define DEG_FACTOR          10.0f
#define ONE_OVER_EPSILON    (1.0f / FLOAT_EPSILON)
#define ONE_OVER_MAX_RAND   ((float)(1.0f / RAND_MAX))

//precomputed sin and cos values
extern const float g_fastSin[];
extern const float g_fastCos[];

//epsilon checks for floating-point values
INLINE bool FloatIsZero(float a)
{
  return a > -FLOAT_EPSILON && a < FLOAT_EPSILON;
}

INLINE bool FloatAreEqual(float a, float b)
{
  a -= b;
  return a > -FLOAT_EPSILON && a < FLOAT_EPSILON;
}

//find min/max
INLINE float FloatMin(float a, float b)
{
  return a < b ? a : b;
}

INLINE float FloatMax(float a, float b)
{
  return a > b ? a : b;
}

INLINE float FloatMin3(float a, float b, float c)
{
  return FloatMin(a, FloatMin(b, c));
}

INLINE float FloatMax3(float a, float b, float c)
{
  return FloatMax(a, FloatMax(b, c));
}

INLINE float FloatMin4(float a, float b, float c, float d)
{
  return FloatMin(FloatMin(a, b), FloatMin(c, d));
}

INLINE float FloatMax4(float a, float b, float c, float d)
{
  return FloatMax(FloatMax(a, b), FloatMax(c, d));
}

//clamp angle in degrees to range [0, 360)
INLINE float ClampAngle(float deg)
{
  /*if (deg < DEG_0)
  {
    for (;;)
    {
      deg += DEG_360;
      if (deg >= DEG_0)
      {
        return deg;
      }
    }
  }
  else if (deg >= DEG_360)
  {
    for (;;)
    {
      deg -= DEG_360;
      if (deg < DEG_360)
      {
        return deg;
      }
    }
  }*/
  while (deg < DEG_0)
  {
    deg += DEG_360;
  }
  while (deg > DEG_360)
  {
    deg -= DEG_360;
  }
  return deg;
}

//convert degrees to radians and vice versa
INLINE float DegToRad(float deg)
{
  return deg * (PI / DEG_180);
}

INLINE float RadToDeg(float rad)
{
  return rad * (DEG_180 / PI);
}

//fast trig functions
INLINE float FastCos(float deg)
{
  return g_fastCos[(int)(ClampAngle(deg) * DEG_FACTOR)];
}

INLINE float FastSin(float deg)
{
  return g_fastSin[(int)(ClampAngle(deg) * DEG_FACTOR)];
}

//seed random number generator
INLINE void RandomSeed()
{
  srand(time(NULL));
}

//get random number in range [min, max]
INLINE float RandomNumber(float min, float max)
{
  return min + (float)rand() * (max - min) * ONE_OVER_MAX_RAND;
}

NAMESPACE_END

#endif //_FLOATMATH_H_
