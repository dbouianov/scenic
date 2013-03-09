#ifndef _PLANE_H_
#define _PLANE_H_

#include "common.h"

NAMESPACE_BEGIN

enum { DIM_PLANE = 4 };
typedef float Plane [DIM_PLANE];

//plane components
#ifndef XYZW
#define XYZW
enum { CX = 0, CY, CZ, CW };
#endif

//common planes
#define PLANE_XY  {0.0f, 0.0f, 1.0f, 0.0f}
#define PLANE_YZ  {1.0f, 0.0f, 0.0f, 0.0f}
#define PLANE_ZX  {0.0f, 1.0f, 0.0f, 0.0f}

//macros to create an unnamed union plane type inside a named type
//useful for index-free access to elements
#define PlaneUnionPrefix(name, prefix) \
union\
{\
  Vector name;\
  struct\
  {\
    float CONCAT(prefix, A);\
    float CONCAT(prefix, B);\
    float CONCAT(prefix, C);\
    float CONCAT(prefix, D);\
  };\
}
#define PlaneUnion(name)  PlaneUnionPrefix(name, name)

//zero-initialize a plane
INLINE void PlaneZero(OUT Plane p)
{
  p[CX] = 0.0f;
  p[CY] = 0.0f;
  p[CZ] = 0.0f;
  p[CW] = 0.0f;
}

//copy a plane
INLINE void PlaneCopy(OUT Plane p, Plane a)
{
  p[CX] = a[CX];
  p[CY] = a[CY];
  p[CZ] = a[CZ];
  p[CW] = a[CW];
}

//initialize a plane
INLINE void PlaneInit(OUT Plane p, float a, float b, float c, float d)
{
  p[CX] = a;
  p[CY] = b;
  p[CZ] = c;
  p[CW] = d;
}

NAMESPACE_END

#endif //_PLANE_H_
