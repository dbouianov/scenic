#ifndef _SCENENODE_H_
#define _SCENENODE_H_

#include "common.h"
#include "object.h"
#include "algebra.h"

NAMESPACE_BEGIN

//maximum node name size
#define MAX_NODE_NAME_SIZE  16

enum NodeFlags
{
  NODE_FLAGS_NONE         = 0x00000000,
  //all standard flags
  NODE_FLAGS_ALL          = 0x00ffffff,
  //standard flags
  NODE_FLAGS_FRUSTUM      = 0x00000001,
  NODE_FLAGS_TRANSFORM    = 0x00000002,
  NODE_FLAGS_PRIMITIVE    = 0x00000004,
  NODE_FLAGS_COLOR        = 0x00000008,
  NODE_FLAGS_LIGHT        = 0x00000010,
  NODE_FLAGS_MATERIAL     = 0x00000020,
  NODE_FLAGS_TEXTURE      = 0x00000040,
  NODE_FLAGS_SKYBOX       = 0x00000080,
  NODE_FLAGS_DISPLIST     = 0x00000100,
  NODE_FLAGS_PSYSTEM      = 0x00000200,
  NODE_FLAGS_MIRROR       = 0x00000400,
  //special flags
  NODE_FLAGS_HALFLOD      = 0x80000000,
  NODE_FLAGS_BOUNDS       = 0x40000000,
  NODE_FLAGS_SELECT       = 0x20000000,
};

//bounding volume
struct Bounds
{
  //bounding volume center
  PointUnion(center);
  //bounding box corners
  PointUnion(frontTopLeft);
  PointUnion(frontTopRight);
  PointUnion(frontBtmLeft);
  PointUnion(frontBtmRight);
  PointUnion(backTopLeft);
  PointUnion(backTopRight);
  PointUnion(backBtmLeft);
  PointUnion(backBtmRight);
  //axis-aligned bounding box min/max coordinates
  PointUnionPrefix(minCorner, min);
  PointUnionPrefix(maxCorner, max);
  //bounding sphere radius squared
  float rSquared;
};

//generic scene node
class SceneNode : public Object
{
protected:
  SceneNode();

public:
  virtual ~SceneNode() = 0;

  //manipulate reference count
  void AddCount();
  uint GetCount() const;

  //get/set node name
  void SetName(cchar *szName);
  cchar *GetName() const;

  //execute OpenGL commands associated with this node
  virtual void Walk(ulong flags = NODE_FLAGS_ALL);

  //optimize node implementation details
  virtual void Optimize();

  //update node state
  virtual void Update(ulong frame, float dt, Vector gravity);

  //find/draw bounding volume
  virtual bool GetBounds(OUT Bounds &bounds);
  virtual void DrawBounds();

private:
  //node reference count
  uint m_count;
#ifdef _DEBUG
  //node name
  char m_szName [MAX_NODE_NAME_SIZE + 1];
#endif
};

NAMESPACE_END

#endif //_SCENENODE_H_
