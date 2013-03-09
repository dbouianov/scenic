#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "common.h"

NAMESPACE_BEGIN

//find aRoots of a quadratic polynomial
INLINE uint SolveQuadratic(float a, float b, float c,
                           OUT float aRoots[2])
{
	if (0.0f == a)
  {
    //zero quadratic coefficient
		if (0.0f == b)
    {
      //zero linear coefficient, no aRoots
			return 0;
		}
    else
    {
      //non-zero linear coefficient, one root
			aRoots[0] = -c / b;
			return 1;
		}
	}
  else
  {
		//compute the discrimanant D = b^2 - 4ac
		float d = b * b - 4.0f * a * c;
		if (d < 0.0f)
    {
      //negative discriminant, no real aRoots
			return 0;
		}
    else
    {
			//non-negative discriminant, two real aRoots
			float q = -0.5f * (b + SIGN(b) * (float)sqrt(d));
      //first real root
			aRoots[0] = q / a;
			if (q != 0.0f)
      {
        //second real root
				aRoots[1] = c / q;
			}
      else
      {
        //two equal real aRoots
				aRoots[1] = aRoots[0];
			}
			return 2;
		}
	}
}

/*
//find aRoots of a cubic polynomial
INLINE uint SolveCubic(float a, float b, float c,
                       OUT float aRoots[3])
{
  //todo: implement
  return 0;
}

//find aRoots of a quartic polynomial
INLINE uint SolveQuartic(float a, float b, float c, float d,
                         OUT float aRoots[4])
{
  //todo: implement
  return 0;
}
*/

NAMESPACE_END

#endif //_SOLVER_H_
