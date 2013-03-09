#ifndef _MENUITEM_H_
#define _MENUITEM_H_

#include "scenenode.h"
#include "font.h"

NAMESPACE_BEGIN

class MenuItem : public SceneNode
{
public:
  MenuItem ();
  virtual ~MenuItem ();

  bool Create  (const char *szText,
                SceneNode *pIcon,
                Font *pFont,
                FontStyle style);
  bool Destroy ();
  bool IsValid ();

  virtual void Walk   (ulong ulFlags = NODE_FLAGS_ALL);
  virtual void Update (ulong nFrame, float dt, Vector gravity);

private:
  //icon to display before text 
  SceneNode *m_pIcon;
  //font for drawing text
  Font *m_pFont;
  //font style
  FontStyle m_style;
};

NAMESPACE_END

#endif //_MENUITEM_H_
