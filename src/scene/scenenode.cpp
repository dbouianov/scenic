#include "scenenode.h"

NAMESPACE_BEGIN

SceneNode::SceneNode()
: m_count (0)
{
#ifdef _DEBUG
  m_szName[0] = NULL_CHAR;
#endif
}

SceneNode::~SceneNode()
{}

void SceneNode::AddCount()
{
  ++m_count;
}

uint SceneNode::GetCount() const
{
  return m_count;
}

void SceneNode::SetName(cchar *szName)
{
#ifndef _DEBUG
  UNUSED_PARAM(szName);
#else
  strncpy(m_szName, szName, MAX_NODE_NAME_SIZE);
  m_szName[MAX_NODE_NAME_SIZE] = NULL_CHAR;
#endif
}

cchar *SceneNode::GetName() const
{
#ifndef _DEBUG
  return EMPTY_STRING;
#else
  return m_szName;
#endif
}

void SceneNode::Walk(ulong)
{}

void SceneNode::Optimize()
{}

void SceneNode::Update(ulong, float, Vector)
{}

bool SceneNode::GetBounds(Bounds &)
{
  //no bounding volume for empty node
  return false;
}

void SceneNode::DrawBounds()
{}

NAMESPACE_END
