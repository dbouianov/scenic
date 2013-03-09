#ifndef _POINT_H_
#define _POINT_H_

#include "common.h"

NAMESPACE_BEGIN

enum { DIM_POINT = 4 };
typedef float Point [DIM_POINT];

//point and vector components
#ifndef XYZW
#define XYZW
enum { CX = 0, CY, CZ, CW };
#endif

//common points
#define POINT_ZERO    {0.0f, 0.0f, 0.0f, 1.0f}
#define POINT_AXIS_X  {1.0f, 0.0f, 0.0f, 1.0f}
#define POINT_AXIS_Y  {0.0f, 1.0f, 0.0f, 1.0f}
#define POINT_AXIS_Z  {0.0f, 0.0f, 1.0f, 1.0f}

//macros to create an unnamed union point type inside a named type
//useful for index-free access to elements
#define PointUnionPrefix(name, prefix)  \
union\
{\
  Point name;\
  struct\
  {\
    float CONCAT(prefix, X);\
    float CONCAT(prefix, Y);\
    float CONCAT(prefix, Z);\
    float CONCAT(prefix, W);\
  };\
}
#define PointUnion(name)  PointUnionPrefix(name, name)

//zero-initialize a point
INLINE void PointZero(OUT Point p)
{
  p[CX] = 0.0f;
  p[CY] = 0.0f;
  p[CZ] = 0.0f;
  p[CW] = 1.0f;
}

//copy a point
INLINE void PointCopy(OUT Point p, Point a)
{
  p[CX] = a[CX];
  p[CY] = a[CY];
  p[CZ] = a[CZ];
  p[CW] = a[CW];
}

//initialize a point
INLINE void PointInit(OUT Point p, float x, float y, float z)
{
  p[CX] = x;
  p[CY] = y;
  p[CZ] = z;
  p[CW] = 1.0f;
}

//multiply a point by a scalar
INLINE void PointScale(Point a, float s, OUT Point p)
{
  p[CX] = a[CX] * s;
  p[CY] = a[CY] * s;
  p[CZ] = a[CZ] * s;
}

NAMESPACE_END

#endif //_POINT_H_
