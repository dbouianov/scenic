#include "mesh.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_SCENE, "Mesh",__FILE__);

Mesh::Mesh()
{}

Mesh::~Mesh()
{}

void Mesh::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_TEXTURE)
  {
    //process texture coordinates
  }
}

NAMESPACE_END
