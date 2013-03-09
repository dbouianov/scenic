#ifndef _MESH_H_
#define _MESH_H_

#include "primitive.h"
#include "algebra.h"
#include <unordered_map>

NAMESPACE_BEGIN

typedef std::unordered_map<uint, Point>  VertexMap;
typedef std::unordered_map<uint, Vector> NormalMap;

class Mesh : public Primitive
{
public:
  Mesh();
  Mesh(const Mesh &other);
  virtual ~Mesh();

  //void Create(uint numVertices, 

  virtual void Walk(ulong flags = NODE_FLAGS_PRIMITIVE);

private:
  
};

NAMESPACE_END

#endif //_MESH_H_
