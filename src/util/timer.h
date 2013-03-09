#ifndef _TIMER_H_
#define _TIMER_H_

#include "common.h"

NAMESPACE_BEGIN

class Timer
{
public:
  Timer();
 ~Timer();

  void Start();
  void Stop ();

  double GetTime();

private:
  timespec m_start;
  timespec m_finish;
};

NAMESPACE_END

#endif //_TIMER_H_
