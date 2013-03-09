#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "common.h"
#include "object.h"
#include <unordered_map>

NAMESPACE_BEGIN

//forward declarations
class SceneNode;
class GroupNode;

typedef std::unordered_map<hash, Object *>    ObjectMap;
typedef std::unordered_map<uint, GroupNode *> SelectMap;

//object factory
class Factory : public Object
{
public:
  Factory();
  virtual ~Factory();

  template<class T>
  T *NewObject()
  {
    return AddObject(new T);
  }

  bool DeleteObject(Object *pObject);
  void DeleteObjects();

  //parent/child node management
  void AddNode(OUT GroupNode *pParent, SceneNode *pChild);
  SceneNode *CopyNode(SceneNode *pNode);
  SceneNode *GetNullNode() const;

  //selectable node management
  void AddSelectNode(GroupNode *pNode);
  GroupNode *GetSelectNode(uint id) const;

private:
  ObjectMap m_objects;
  //selectable node list
  SelectMap m_selectNodes;

  template<class T>
  T *AddObject(T *pObject)
  {
    if (pObject)
    {
      m_objects[pObject->GetHash()] = pObject;
    }
    return pObject;
  }

  bool RemoveObject(Object *pObject);
};

NAMESPACE_END

#endif //_FACTORY_H_
