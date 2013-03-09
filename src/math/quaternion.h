#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "common.h"

NAMESPACE_BEGIN

enum { DIM_QUATERNION = 4 };
typedef float Quaternion [DIM_QUATERNION];

//quaternion components
#ifndef XYZW
#define XYZW
enum { CX = 0, CY, CZ, CW };
#endif

//common quaternions
#define QUATERNION_ZERO
#define QUATERNION_I
#define QUATERNION_J
#define QUATERNION_K

//macros to create an unnamed union quaternion type inside a named type
//useful for index-free access to elements
#define QuaternionUnionPrefix(name, prefix) \
union\
{\
  Quaternion name;\
  struct\
  {\
    float CONCAT(prefix, X);\
    float CONCAT(prefix, Y);\
    float CONCAT(prefix, Z);\
    float CONCAT(prefix, W);\
  };\
}
#define QuaternionUnion(name) QuaternionUnionPrefix(name, name)

//zero-initialize a quaternion
INLINE void QuaternionZero(OUT Quaternion q)
{
  q[CX] = 0;
  q[CY] = 0;
  q[CZ] = 0;
  q[CW] = 0;
}

//copy a quaternion
INLINE void QuaternionCopy(OUT Quaternion q, Quaternion a)
{
  q[CX] = a[CX];
  q[CY] = a[CY];
  q[CZ] = a[CZ];
  q[CW] = a[CW];
}

//initialize a quaternion
INLINE void QuaternionInit(OUT Quaternion q,
                           float x, float y, float z, float w)
{
  q[CX] = x;
  q[CY] = y;
  q[CZ] = z;
  q[CW] = w;
}

//

//multiply a quaternion by a scalar
INLINE void QuaternionScale(Quaternion a, float s, OUT Quaternion q)
{
  q[CX] = a[CX] * s;
  q[CY] = a[CY] * s;
  q[CZ] = a[CZ] * s;
  q[CW] = a[CW] * s;
}

//add a quaternion to another quaternion
INLINE void QuaternionAdd(Quaternion a, Quaternion b, OUT Quaternion q)
{
  q[CX] = a[CX] + b[CX];
  q[CY] = a[CY] + b[CY];
  q[CZ] = a[CZ] + b[CZ];
  q[CW] = a[CW] + b[CW];
}

//subtract a quaternion from another quaternion
INLINE void QuaternionSubtract(Quaternion a, Quaternion b, OUT Quaternion q)
{
  q[CX] = a[CX] - b[CX];
  q[CY] = a[CY] - b[CY];
  q[CZ] = a[CZ] - b[CZ];
  q[CW] = a[CW] - b[CW];
}

//multiply a quaternion by another quaternion
INLINE void QuaternionMultiply(Quaternion a, Quaternion b, OUT Quaternion q)
{
  q[CX] = a[CW] * b[CX] + a[CX] * b[CW] + a[CY] * b[CZ] - a[CZ] * b[CY];
  q[CY] = a[CW] * b[CY] - a[CX] * b[CZ] + a[CY] * b[CW] + a[CZ] * b[CX];
  q[CZ] = a[CW] * b[CZ] + a[CX] * b[CY] - a[CY] * b[CX] + a[CZ] * b[CW];
  q[CW] = a[CW] * b[CW] - a[CX] * b[CX] - a[CY] * b[CY] - a[CZ] * b[CZ];
}

//get a quaternion congugate
INLINE void QuaternionCongugate(Quaternion p, OUT Quaternion q)
{
  q[CX] = -p[CX];
  q[CY] = -p[CY];
  q[CZ] = -p[CZ];
  q[CW] =  p[CW];
}

//calculate dot product of two quaternions
INLINE float QuaternionDotProduct(Quaternion a, Quaternion b)
{
  return a[CX] * b[CX] + a[CY] * b[CY] + a[CZ] * b[CZ] + a[CW] * b[CW];
}

//calculate norm of a quaternion
INLINE float QuaternionNorm(Quaternion a)
{
  return QuaternionDotProduct(a, a);
}

//calculate inverse of a quaternion
INLINE void QuaternionInverse(Quaternion a, OUT Quaternion q)
{
  float s = 1.0f / QuaternionNorm(a);
  QuaternionCongugate(a, q);
  QuaternionScale(q, s, q);
}

NAMESPACE_END

#endif //_QUATERNION_H_
