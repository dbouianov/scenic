#include "selectnode.h"

NAMESPACE_BEGIN

//static
GLuint SelectNode::m_nextSelectId = BAD_SELECT_ID;

SelectNode::SelectNode()
{
  //increment next selection mode id
  m_selectId = ++m_nextSelectId;
}

SelectNode::~SelectNode()
{}

GLuint SelectNode::GetSelectId() const
{
  return m_selectId;
}

void SelectNode::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_SELECT)
  {
    glPushName(m_selectId);
  }

  NodeType::Walk(flags);

  if (flags & NODE_FLAGS_SELECT)
  {
    glPopName();
  }
}

NAMESPACE_END
