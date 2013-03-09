#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "common.h"

NAMESPACE_BEGIN

//allocate a block of memory
INLINE void *MemAlloc(ulong size)
{
  return malloc(size);
}

//re-allocate a block of memory
INLINE void *MemReAlloc(void *pBlock, ulong size)
{
  return realloc(pBlock, size);
}

//free a block of memory
INLINE void MemFree(void *pBlock)
{
  free(pBlock);
}

//move a block of memory
INLINE void *MemMove(OUT void *pDst, void *pSrc, ulong size)
{
  return memmove(pDst, pSrc, size);
}

//copy a block of memory
INLINE void *MemCopy(OUT void *pDst, void *pSrc, ulong size)
{
  return memcpy(pDst, pSrc, size);
}

//fill block of memory with a certain value
INLINE void MemFill(OUT void *pDst, ubyte uValue, ulong size)
{
  memset(pDst, uValue, size);
}

//zero-initialize a block of memory
INLINE void MemZero(OUT void *pDst, ulong size)
{
  memset(pDst, 0, size);
}

//compare two blocks of memory
INLINE int MemCompare(void *pLeft, void *pRight, ulong size)
{
  return memcmp(pLeft, pRight, size);
}

NAMESPACE_END

#endif //_MEMORY_H_
