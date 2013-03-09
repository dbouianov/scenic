#ifndef _WORLD_H_
#define _WORLD_H_

#include "algebra.h"

NAMESPACE_BEGIN

struct World
{
  //physical model
  //constant downward gravity along y-axis acting on player and objects
  //constant wind with small random perturbations in x-z plane acting on objects
  Vector gravity;
  //todo: define more complex physical model
};

NAMESPACE_END

#endif //_WORLD_H_
