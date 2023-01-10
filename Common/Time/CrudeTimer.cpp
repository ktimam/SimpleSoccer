#ifndef LINUX
#include "CrudeTimer.h"


CrudeTimer* CrudeTimer::Instance()
{
  static CrudeTimer instance;

  return &instance;
}

#endif