#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "common.h"

NAMESPACE_BEGIN

enum { DIM_VECTOR = 4 };
typedef float Vector [DIM_VECTOR];

//point and vector components
#ifndef XYZW
#define XYZW
enum { CX = 0, CY, CZ, CW };
#endif

//common vectors
#define VECTOR_ZERO     {0.0f, 0.0f, 0.0f, 0.0f}
#define VECTOR_AXIS_X   {1.0f, 0.0f, 0.0f, 0.0f}
#define VECTOR_AXIS_Y   {0.0f, 1.0f, 0.0f, 0.0f}
#define VECTOR_AXIS_Z   {0.0f, 0.0f, 1.0f, 0.0f}

//macros to create an unnamed union vector type inside a named type
//useful for index-free access to elements
#define VectorUnionPrefix(name, prefix) \
union\
{\
  Vector name;\
  struct\
  {\
    float CONCAT(prefix, X);\
    float CONCAT(prefix, Y);\
    float CONCAT(prefix, Z);\
    float CONCAT(prefix, W);\
  };\
}
#define VectorUnion(name) VectorUnionPrefix(name, name)

//zero-initialize a vector
INLINE void VectorZero(OUT Vector v)
{
  v[CX] = 0.0f;
  v[CY] = 0.0f;
  v[CZ] = 0.0f;
  v[CW] = 0.0f;
}

//copy a vector
INLINE void VectorCopy(OUT Vector v, Vector a)
{
  v[CX] = a[CX];
  v[CY] = a[CY];
  v[CZ] = a[CZ];
  v[CW] = a[CW];
}

//initialize a vector
INLINE void VectorInit(OUT Vector v, float x, float y, float z)
{
  v[CX] = x;
  v[CY] = y;
  v[CZ] = z;
  v[CW] = 0.0f;
}

//set a vector to x-axis
INLINE void VectorAxisX(OUT Vector v)
{
  v[CX] = 1.0f;
  v[CY] = 0.0f;
  v[CZ] = 0.0f;
  v[CW] = 0.0f;
}

//set a vector to the y-axis
INLINE void VectorAxisY(OUT Vector v)
{
  v[CX] = 0.0f;
  v[CY] = 1.0f;
  v[CZ] = 0.0f;
  v[CW] = 0.0f;
}

//set a vector to the z-axis
INLINE void VectorAxisZ(OUT Vector v)
{
  v[CX] = 0.0f;
  v[CY] = 0.0f;
  v[CZ] = 1.0f;
  v[CW] = 0.0f;
}

//multiply a vector by a scalar
INLINE void VectorScale(Vector a, float s, OUT Vector v)
{
  v[CX] = a[CX] * s;
  v[CY] = a[CY] * s;
  v[CZ] = a[CZ] * s;
}

//add a vector to another vector
INLINE void VectorAdd(Vector a, Vector b, OUT Vector v)
{
  v[CX] = a[CX] + b[CX];
  v[CY] = a[CY] + b[CY];
  v[CZ] = a[CZ] + b[CZ];
}

//subtract a vector from another vector
INLINE void VectorSubtract(Vector a, Vector b, OUT Vector v)
{
  v[CX] = a[CX] - b[CX];
  v[CY] = a[CY] - b[CY];
  v[CZ] = a[CZ] - b[CZ];
}

//negate a vector
INLINE void VectorNegate(Vector a, OUT Vector v)
{
  v[CX] = -a[CX];
  v[CY] = -a[CY];
  v[CZ] = -a[CZ];
}

//calculate dot product of two vectors
INLINE float VectorDotProduct(Vector a, Vector b)
{
  return a[CX] * b[CX] + a[CY] * b[CY] + a[CZ] * b[CZ];
}

//calculate squared length of a vector
INLINE float VectorLengthSquared(Vector a)
{
  return VectorDotProduct(a, a);
}

//calculate length of a vector
INLINE float VectorLength(Vector a)
{
  return (float)sqrt(VectorLengthSquared(a));
}

//normalize a vector
INLINE void VectorNormalize(Vector a, OUT Vector v)
{
  float s = 1.0f / VectorLength(a);
  VectorScale(a, s, v);
}

//calculate cross product of two vectors
INLINE void VectorCrossProduct(Vector a, Vector b, OUT Vector v)
{
  v[CX] = a[CY] * b[CZ] - a[CZ] * b[CY];
  v[CY] = a[CZ] * b[CX] - a[CX] * b[CZ];
  v[CZ] = a[CX] * b[CY] - a[CY] * b[CX];
}

NAMESPACE_END

#endif //_VECTOR_H_
