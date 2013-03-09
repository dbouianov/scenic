#ifndef _GROUPNODE_H_
#define _GROUPNODE_H_

#include "scenenode.h"
#include <vector>

NAMESPACE_BEGIN

//list of child nodes
typedef std::vector<SceneNode *> SceneNodeList;

//group node
class GroupNode : public SceneNode
{
public:
  GroupNode();
  virtual ~GroupNode();

  //child node management
  void AddNode(SceneNode *pNode);
  ulong GetNumNodes() const;
  SceneNodeList &GetNodeList();

  //selection mode id
  uint GetSelectId() const;

  void SetSelectable(bool selectable);
  bool IsSelectable() const;

  void SetSelected(bool selected);
  bool IsSelected() const;

  //execute OpenGL commands associated with child nodes
  virtual void Walk(ulong flags = NODE_FLAGS_ALL);

  //optimize node implementation details
  virtual void Optimize();

  //update child node states
  virtual void Update(ulong frame, float dt, Vector gravity);

  //find/draw bounding volume
  virtual bool GetBounds(OUT Bounds &bounds);
  virtual void DrawBounds();

private:
  //current frame
  ulong m_frame;
  //selection mode id
  GLuint m_selectId;
  bool   m_selectable;
  bool   m_selected;
  //bounding volume
  Bounds m_bounds;
  //list of child nodes
  SceneNodeList m_nodes;

  //next selection mode id
  static GLuint m_nextSelectId;

  //hide default copy constructor and assignment operator
  GroupNode(const GroupNode &other);
  GroupNode &operator=(const GroupNode &other);
};

NAMESPACE_END

#endif //_GROUPNODE_H_
