#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "common.h"

NAMESPACE_BEGIN

//logger constants
#define LOG_PREFIX_MSG    ""
#define LOG_PREFIX_WRN    "Warning! "
#define LOG_PREFIX_ERR    "Error! "

//logger flags
enum
{
  LOG_FLAGS_NONE  = 0x00000000,
  LOG_FLAGS_DBG   = 0x80000000,
  LOG_FLAGS_ALL   = 0xffffffff,
};

//logger
class Log
{
public:
  struct Info
  {
    ulong  _flags;
    cchar *_title;
    cchar *_file;
  };

  Log(const Info &info, FILE &stream,
      cchar *prefix, cchar *line, cchar *func);

  void write(cchar *format, ...);

  //no-op message for release builds
  static void noop(...) {}

  static ulong g_flags;

private:
  const Info *_info;
  FILE  *_stream;
  cchar *_prefix;
  cchar *_line;
  cchar *_func;
};

NAMESPACE_END

//logger declaration
#define LOG_DECLARE(flags, title, file) \
static const NAMESPACE::Log::Info g_logInfo = { flags, title, file }

//logger macros
#define LogMsg \
NAMESPACE::Log(g_logInfo,*stdout,LOG_PREFIX_MSG,STRING2(__LINE__),__FUNCTION__).write
#define LogWrn \
NAMESPACE::Log(g_logInfo,*stderr,LOG_PREFIX_WRN,STRING2(__LINE__),__FUNCTION__).write
#define LogErr \
NAMESPACE::Log(g_logInfo,*stderr,LOG_PREFIX_ERR,STRING2(__LINE__),__FUNCTION__).write

#ifndef _DEBUG
#define DbgMessage    Log::noop
#define DbgWarning    Log::noop
#define DbgError      Log::noop
#define DbgAssert(x)
#define DbgEnter(x)
#define DbgLeave(x)
#define ShowMessage   LogMsg
#define ShowWarning   LogWrn
#define ShowError     LogErr
#else
#define DbgMessage    LogMsg
#define DbgWarning    LogWrn
#define DbgError      LogErr
#define DbgAssert(x)
#define DbgEnter(x)   LogMsg("enter")
#define DbgLeave(x)   LogMsg("leave")
#define ShowMessage   LogMsg
#define ShowWarning   LogWrn
#define ShowError     LogErr
#endif //_DEBUG

#endif //_LOGGER_H_
