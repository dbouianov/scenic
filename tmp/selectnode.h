#ifndef _SELECTNODE_H_
#define _SELECTNODE_H_

#include "scenenode.h"

NAMESPACE_BEGIN

#define BAD_SELECT_ID   0

class SelectNode : public SceneNode
{
public:
  SelectNode();
  virtual ~SelectNode();

  //selection mode id
  GLuint GetSelectId() const;

  virtual void Walk(ulong flags = NODE_FLAGS_SELECT);

private:
  //selection mode id
  GLuint m_selectId;

  //next selection mode id
  static GLuint m_nextSelectId;
};

NAMESPACE_END

#endif //_SELECTNODE_H_
