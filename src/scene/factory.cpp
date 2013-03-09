#include "factory.h"
#include "groupnode.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_FACTORY, "Factory",__FILE__);

//object factory
Factory::Factory()
{}

Factory::~Factory()
{
  DeleteObjects();
}

bool Factory::RemoveObject(Object *pObject)
{
  DbgEnter();
  if (pObject)
  {
    ObjectMap::iterator it = m_objects.find(pObject->GetHash());
    if (it != m_objects.end())
    {
      m_objects.erase(it);
      return true;
    }
  }
  return false;
}

bool Factory::DeleteObject(Object *pObject)
{
  DbgEnter();
  if (RemoveObject(pObject))
  {
    delete pObject;
    return true;
  }
  return false;
}

void Factory::DeleteObjects()
{
  DbgEnter();
  if (!m_objects.empty())
  {
    DbgMessage("%u objects", m_objects.size());
    //delete all objects
    for (ObjectMap::iterator it = m_objects.begin(); it != m_objects.end(); it++)
    {
      delete it->second;
    }
    //clear object map
    m_objects.clear();
  }
}

void Factory::AddNode(GroupNode *pParent, SceneNode *pChild)
{
  DbgEnter();
  if (pParent && pChild)
  {
    pParent->AddNode(CopyNode(pChild));
  }
}

SceneNode *Factory::CopyNode(SceneNode *pNode)
{
  DbgEnter();
  //check if node already belongs to some parent
  if (pNode && pNode->GetCount() > 0)
  {
    GroupNode *pGroup = dynamic_cast<GroupNode *>(pNode);
    if (pGroup)
    {
      //save list of child nodes
      SceneNodeList &nodes = pGroup->GetNodeList();
      //create new group node
      pGroup = NewObject<GroupNode>();
      //add copies of all child nodes to new group node
      for (SceneNodeList::iterator it = nodes.begin(); it != nodes.end(); it++)
      {
        pGroup->AddNode(CopyNode(*it));
      }
      return pGroup;
    }
  }
  //return original node
  return pNode;
}

SceneNode *Factory::GetNullNode() const
{
  DbgEnter();
  //lazy instantiation
  static GroupNode node;
  return &node;
}

void Factory::AddSelectNode(GroupNode *pNode)
{
  //check if group node is selectable
  if (pNode->IsSelectable())
  {
    //add node to selectable node list
    DbgMessage("Adding selectable node %d: %s", pNode->GetSelectId(), pNode->GetName());
    m_selectNodes[pNode->GetSelectId()] = pNode;
  }
}

GroupNode *Factory::GetSelectNode(uint id) const
{
  SelectMap::const_iterator it = m_selectNodes.find(id);
  if (it != m_selectNodes.end())
  {
    return it->second;
  }
  return NULL;
}

NAMESPACE_END
