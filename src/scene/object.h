#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "common.h"

NAMESPACE_BEGIN

//base object
class Object
{
protected:
  Object();

public:
  virtual ~Object() = 0;

  virtual hash GetHash() const;
  static  hash GetHash(void *pObject);
};

NAMESPACE_END

#endif //_OBJECT_H_
