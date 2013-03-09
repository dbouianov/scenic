#include "timer.h"
#include <sys/time.h>

NAMESPACE_BEGIN

Timer::Timer()
{
  MemZero(&m_start,  sizeof(m_start));
  MemZero(&m_finish, sizeof(m_finish));
}

Timer::~Timer()
{}

void Timer::Start()
{
  clock_gettime(CLOCK_REALTIME, &m_start);
}

void Timer::Stop()
{
  clock_gettime(CLOCK_REALTIME, &m_finish);
}

double Timer::GetTime()
{
  return (m_finish.tv_sec - m_start.tv_sec) +
    (m_finish.tv_nsec - m_start.tv_nsec) * 0.000000001;
}

NAMESPACE_END
