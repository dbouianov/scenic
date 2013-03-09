#ifndef _DISPLIST_H_
#define _DISPLIST_H_

#include "scenenode.h"

NAMESPACE_BEGIN

#define BAD_DISPLAY_LIST    0

class DisplayList : public SceneNode
{
public:
  DisplayList();
  virtual ~DisplayList();

  bool Create(SceneNode *pNode,
              ulong flags = NODE_FLAGS_ALL,
              bool execute = false);
  bool Destroy();
  bool IsValid() const;

  void Walk(ulong flags = NODE_FLAGS_DISPLIST);

private:
  //display list
  GLuint m_list;
};

NAMESPACE_END

#endif //_DISPLIST_H_
