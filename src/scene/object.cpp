#include "object.h"

NAMESPACE_BEGIN

//base object
Object::Object()
{}

Object::~Object()
{}

hash Object::GetHash() const
{
  return reinterpret_cast<hash>(this);
}

//static
hash Object::GetHash(void *pObject)
{
  return reinterpret_cast<hash>(pObject);
}

NAMESPACE_END
