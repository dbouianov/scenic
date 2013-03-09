#include "displist.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_CACHE, "Cache",__FILE__);

DisplayList::DisplayList()
: m_list (BAD_DISPLAY_LIST)
{
#ifdef _DEBUG
  SetName("cache");
#endif
}

DisplayList::~DisplayList()
{
  //destroy display list
  Destroy();
}

bool DisplayList::Create(SceneNode *pNode, ulong flags, bool execute)
{
  if (pNode && BAD_DISPLAY_LIST == m_list)
  {
    //generate new display list only if not already generated
    DbgMessage("Creating display list for %s", pNode->GetName());
    m_list = glGenLists(1);
    if (m_list != BAD_DISPLAY_LIST)
    {
      //generate display list
      glNewList(m_list, execute ? GL_COMPILE_AND_EXECUTE : GL_COMPILE);
        pNode->Walk(flags);
      glEndList();
#ifdef _DEBUG
      SetName(pNode->GetName());
#endif
      return true;
    }
  }
  return false;
}

bool DisplayList::Destroy()
{
  if (m_list != BAD_DISPLAY_LIST)
  {
    //destroy display list only if it is valid
    glDeleteLists(m_list, 1);
    m_list = BAD_DISPLAY_LIST;
    return true;
  }
  return false;
}

bool DisplayList::IsValid() const
{
  return m_list != BAD_DISPLAY_LIST;
}

void DisplayList::Walk(ulong flags)
{
  if (flags & NODE_FLAGS_DISPLIST)
  {
    //call display list
    glCallList(m_list);
  }
}

NAMESPACE_END
