#include "logger.h"

NAMESPACE_BEGIN

//static
#ifndef _DEBUG
ulong Log::g_flags = LOG_FLAGS_ALL & ~LOG_FLAGS_DBG;
#else
ulong Log::g_flags = LOG_FLAGS_ALL;
#endif

Log::Log(const Info &info, FILE &stream,
         cchar *prefix, cchar *line, cchar *func)
:_info(&info),_stream(&stream),_prefix(prefix),_line(line),_func(func)
{}

void Log::write(cchar *format, ...)
{
  if (g_flags & _info->_flags)
  {
    enum { BUFFER_SIZE = MAX_STRING_SIZE * 2 };
    char buffer [BUFFER_SIZE + 1] = EMPTY_STRING;
    uint numChars = 0;

    if (g_flags & LOG_FLAGS_DBG)
    {
      time_t tt = time(NULL);
      tm *t = localtime(&tt);
      numChars = snprintf(buffer, BUFFER_SIZE,
        "[%04u/%02u/%02u %02u:%02u:%02u] %s: %s(%s) %s(): %s",
        1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec,
        _info->_title, _info->_file, _line, _func, _prefix);
    }
    else
    {
      numChars = snprintf(buffer, BUFFER_SIZE,
        "%s: %s", _info->_title, _prefix);
    }

    if (numChars < BUFFER_SIZE)
    {
      va_list args;
      va_start(args, format);
      vsnprintf(&buffer[numChars], BUFFER_SIZE - numChars, format, args);
      va_end(args);
    }

    buffer[BUFFER_SIZE] = NULL_CHAR;

    fprintf(_stream, "%s\n", buffer);
    //fflush(_stream);
  }
}

NAMESPACE_END
