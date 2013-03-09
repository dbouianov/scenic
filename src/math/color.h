#ifndef _COLOR_H_
#define _COLOR_H_

#include "common.h"
#include "vector.h"

NAMESPACE_BEGIN

enum { DIM_COLOR = 4 };
typedef float Color [DIM_COLOR];

//color components
#ifndef RGBA
#define RGBA
enum { CR = 0, CG, CB, CA };
#endif

//common colors
#define COLOR_BLACK    {0.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_GREY     {0.5f, 0.5f, 0.5f, 1.0f}
#define COLOR_WHITE    {1.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_RED      {1.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_GREEN    {0.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BLUE     {0.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_YELLOW   {1.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_CYAN     {0.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_MAGENTA  {1.0f, 0.0f, 1.0f, 1.0f}

//macros to create an unnamed union color type inside a named type
//useful for index-free access to elements
#define ColorUnionPrefix(name, prefix) \
union\
{\
  Color name;\
  struct\
  {\
    float CONCAT(prefix, R);\
    float CONCAT(prefix, G);\
    float CONCAT(prefix, B);\
    float CONCAT(prefix, A);\
  };\
}
#define ColorUnion(name) ColorUnionPrefix(name, name)

//zero-initialize a color
INLINE void ColorZero(OUT Color c)
{
  c[CR] = 0.0f;
  c[CG] = 0.0f;
  c[CB] = 0.0f;
  c[CA] = 1.0f;
}

//copy a color
INLINE void ColorCopy(OUT Color c, Color a)
{
  c[CR] = a[CR];
  c[CG] = a[CG];
  c[CB] = a[CB];
  c[CA] = a[CA];
}

//initialize a color
INLINE void ColorInit(OUT Color c, float r, float g, float b, float a)
{
  c[CR] = r;
  c[CG] = g;
  c[CB] = b;
  c[CA] = a;
}

//multiply a color by a scalar
INLINE void ColorScale(Color a, float s, OUT Color c)
{
  c[CR] = a[CR] * s;
  c[CG] = a[CG] * s;
  c[CB] = a[CB] * s;
}

//add a color to another color
INLINE void ColorAdd(Color a, Color b, OUT Color c)
{
  c[CR] = a[CR] + b[CR];
  c[CG] = a[CG] + b[CG];
  c[CB] = a[CB] + b[CB];
}

//subtract a color from another color
INLINE void ColorSubtract(Color a, Color b, OUT Color c)
{
  c[CR] = a[CR] - b[CR];
  c[CG] = a[CG] - b[CG];
  c[CB] = a[CB] - b[CB];
}

NAMESPACE_END

#endif //_COLOR_H_
