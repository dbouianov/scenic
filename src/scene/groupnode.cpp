#include "groupnode.h"
#include "primitive.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_SCENE, "GroupNode",__FILE__);

//static
GLuint GroupNode::m_nextSelectId = 1;

GroupNode::GroupNode()
: m_frame      (0),
  m_selectable (false),
  m_selected   (false)
{
#ifdef _DEBUG
  SetName("group");
#endif
  MemZero(&m_bounds, sizeof(m_bounds));
  //increment next selection mode id
  m_selectId = m_nextSelectId++;
}

GroupNode::~GroupNode()
{}

void GroupNode::AddNode(SceneNode *pNode)
{
  if (pNode)
  {
    //add node to child list
    m_nodes.push_back(pNode);
    //increment node reference count
    pNode->AddCount();
  }
}

ulong GroupNode::GetNumNodes() const
{
  return m_nodes.size();
}

SceneNodeList &GroupNode::GetNodeList()
{
  return m_nodes;
}

uint GroupNode::GetSelectId() const
{
  return m_selectId;
}

void GroupNode::SetSelectable(bool selectable)
{
  m_selectable = selectable;
}

bool GroupNode::IsSelectable() const
{
  return m_selectable;
}

void GroupNode::SetSelected(bool selected)
{
  if (IsSelectable())
  {
    m_selected = selected;
  }
}

bool GroupNode::IsSelected() const
{
  return m_selected;
}

void GroupNode::Walk(ulong flags)
{
  //todo: perform bounding volume/view volume intersection test
  bool select = IsSelectable() && (flags & NODE_FLAGS_SELECT);
  if (select)
  {
    glPushName(m_selectId);
  }

  //save modelview matrix
  glPushMatrix();

    //walk through all child nodes
    for (SceneNodeList::iterator it = m_nodes.begin(); it != m_nodes.end(); it++)
    {
      (*it)->Walk(flags);
    }

  //restore modelview matrix
  glPopMatrix();

  if (select)
  {
    glPopName();
  }
}

void GroupNode::Optimize()
{
  //set child node list capacity to number of child nodes
  m_nodes.resize(m_nodes.size());
  //optimize child nodes
  for (SceneNodeList::iterator it = m_nodes.begin(); it != m_nodes.end(); it++)
  {
    (*it)->Optimize();
  }
}

void GroupNode::Update(ulong frame, float dt, Vector gravity)
{
  //avoid multiple updates
  if (frame != m_frame)
  {
    //update all child nodes
    for (SceneNodeList::iterator it = m_nodes.begin(); it != m_nodes.end(); it++)
    {
      (*it)->Update(frame, dt, gravity);
    }
    //update current frame
    m_frame = frame;
  }
}

bool GroupNode::GetBounds(Bounds &bounds)
{
  bool valid = false;

  //initialize minimums to very large positive values
  bounds.minX = ONE_OVER_EPSILON;
  bounds.minY = ONE_OVER_EPSILON;
  bounds.minZ = ONE_OVER_EPSILON;

  //initialize maximums to very large negative values
  bounds.maxX = -ONE_OVER_EPSILON;
  bounds.maxY = -ONE_OVER_EPSILON;
  bounds.maxZ = -ONE_OVER_EPSILON;

  //save modelview matrix
  glPushMatrix();
  //find union of bounding volumes of all children
  for (SceneNodeList::iterator it = m_nodes.begin(); it != m_nodes.end(); it++)
  {
    //find child bounding volume
    SceneNode *pNode = *it;
    Bounds childBounds;
    if (pNode->GetBounds(childBounds))
    {
      DbgMessage("Found bounding volume for %s", pNode->GetName());

      //update parent bounding volume
      bounds.minX = FloatMin(bounds.minX, childBounds.minX);
      bounds.minY = FloatMin(bounds.minY, childBounds.minY);
      bounds.minZ = FloatMin(bounds.minZ, childBounds.minZ);
      bounds.maxX = FloatMax(bounds.maxX, childBounds.maxX);
      bounds.maxY = FloatMax(bounds.maxY, childBounds.maxY);
      bounds.maxZ = FloatMax(bounds.maxZ, childBounds.maxZ);
      bounds.centerX = (bounds.maxX - bounds.minX) * 0.5f;
      bounds.centerY = (bounds.maxY - bounds.minY) * 0.5f;
      bounds.centerZ = (bounds.maxZ - bounds.minZ) * 0.5f;

      DbgMessage("min x = %f, y = %f, z = %f",
        bounds.minX, bounds.minY, bounds.minZ);
      DbgMessage("max x = %f, y = %f, z = %f",
        bounds.maxX, bounds.maxY, bounds.maxZ);

      //parent has a valid bounding volume
      valid = true;
    }
  }
  //restore modelview matrix
  glPopMatrix();
  if (valid)
  {
    //find bounding sphere radius
    Primitive::GetBoundsSphere(bounds);
    //save bounding volume
    m_bounds = bounds;
  }
  return valid;
}

void GroupNode::DrawBounds()
{
  //draw child bounding volumes
  for (SceneNodeList::iterator it = m_nodes.begin(); it != m_nodes.end(); it++)
  {
    (*it)->DrawBounds();
  }
  //draw parent bounding volume
  Primitive::DrawBounds(m_bounds);
}

NAMESPACE_END
