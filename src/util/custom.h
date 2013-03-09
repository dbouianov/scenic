#ifndef _CUSTOM_H_
#define _CUSTOM_H_

#include "common.h"

//graphics headers
#include <GL/glew.h>

NAMESPACE_BEGIN

//error codes
enum
{
  //todo: add new error codes here
};

//constants
enum
{
  MAX_STRING_SIZE     = 256,
  MAX_FILE_NAME_SIZE  = 256,
  MAX_LINE_SIZE       = MAX_STRING_SIZE,
};
#define MAX_LINE_SIZE_STRING    STRING2(MAX_LINE_SIZE)

//logger flags
enum
{
  LOG_FLAGS_MAIN      = 0x00000001,
  LOG_FLAGS_CONFIG    = 0x00000002,
  LOG_FLAGS_IMAGE     = 0x00000004,
  LOG_FLAGS_TEXTURE   = 0x00000008,
  LOG_FLAGS_CAMERA    = 0x00000010,
  LOG_FLAGS_FACTORY   = 0x00000020,
  LOG_FLAGS_LOADER    = 0x00000040,
  LOG_FLAGS_CACHE     = 0x00000080,
  LOG_FLAGS_SCENE     = 0x00000100,
  LOG_FLAGS_CONTEXT   = 0x00000200,
};

NAMESPACE_END

#include "memory.h"
#include "default.h"

#endif //_CUSTOM_H_
