#ifndef _COMMON_H_
#define _COMMON_H_

//common headers
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

//common macros
#ifndef NAMESPACE
#define NAMESPACE_BEGIN
#define NAMESPACE_END
#define USING_NAMESPACE
#else
#define NAMESPACE_BEGIN   namespace NAMESPACE {
#define NAMESPACE_END     }
#define USING_NAMESPACE   using namespace NAMESPACE;
#endif

NAMESPACE_BEGIN

//common macros
#ifndef NULL
#define NULL    0
#endif

#ifndef INLINE
#define INLINE  __inline
#endif

#define IN
#define OUT
#define INOUT

#define UNUSED_PARAM(a)     ((void *)&(a))
#define EXPAND(a)           a
#define CONCAT(a, b)        a##b
#define STRING(a)           #a
#define STRING2(a)          STRING(a)
#define NUM_ELEMENTS(a)     (sizeof(a) / sizeof(a[0]))
#define BIT_FROM_INDEX(i)   (1 << (i))
#define SIGN(i)             ((i) < 0 ? -1 : 1)

//common functions
template<class T>
INLINE const T &MAX(const T &a, const T &b) { return a > b ? a : b; }

template<class T>
INLINE const T &MIN(const T &a, const T &b) { return a < b ? a : b; }

//common typedefs
typedef unsigned char   ubyte;
typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;
typedef size_t          hash;
typedef const char      cchar;

//common error codes
enum
{
  STATUS_OK           = 0,
  STATUS_ERR          = -1,
  ERR_BAD_INPUT       = -2,
  ERR_BAD_SIZE        = -3,
  ERR_BAD_INDEX       = -4,
  ERR_BAD_ID          = -5,
  ERR_BAD_HASH        = -6,
  ERR_OUT_OF_MEMORY   = -7,
};

//common constants
enum
{
  BAD_SIZE      = -1,
  BAD_INDEX     = -1,
  BAD_ID        = -1,
  BAD_HASH      = 0,
};

#define NULL_CHAR       '\0'
#define EMPTY_STRING    ""

NAMESPACE_END

//common headers
#include "custom.h"
#include "logger.h"

#endif //_COMMON_H_
