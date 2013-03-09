#ifndef _MENU_H_
#define _MENU_H_

#include "scenenode.h"
#include "color.h"
#include "displist.h"
#include "texture.h"

NAMESPACE_BEGIN

//menu parameters
#define MAX_MENU_WIDTH    DEFAULT_MENU_WIDTH
#define MAX_MENU_HEIGHT   DEFAULT_MENU_HEIGHT

//menu callback
typedef void (* MenuCallback)();

//menu data
struct MenuData
{
  SceneNode *pItem;
  MenuCallback pCallback;
};

//menu
class Menu : public SceneNode
{
public:
  Menu ();
  virtual ~Menu ();

  bool Create (uint nWidth,
               uint nHeight,
               float fItemWidth,
               float fItemHeight,
               Color itemColor,
               Color selectColor,
               Color backColor,
               Texture *pTexture);
  bool Destroy ();
  bool IsValid ();

  void SetItem (uint x, uint y, SceneNode *pItem,
                MenuCallback pCallback);

  void MoveUp    ();
  void MoveDown  ();
  void MoveLeft  ();
  void MoveRight ();

  void Select ();

  virtual void Walk (ulong ulFlags = NODE_FLAGS_ALL);
  virtual void Update (ulong nFrame, float dt, Vector gravity);

private:
  //background square
  DisplayList m_back;
  //background color
  Color m_backColor;
  //item color
  Color m_itemColor;
  //selected item color
  Color m_selectColor;
  //menu dimensions
  uint  m_nWidth;
  uint  m_nHeight;
  float m_fWidth;
  float m_fHeight;
  //menu item dimensions
  float m_fItemWidth;
  float m_fItemHeight;
  //menu item spacers
  float m_fSpaceX;
  float m_fSpaceY;
  //menu item start coordinates
  float m_fStartX;
  float m_fStartY;
  //selected item coordinates
  uint m_nSelectX;
  uint m_nSelectY;
  //menu background texture
  GLuint m_nTexture;
  //array of menu items
  MenuData m_aItems [MAX_MENU_WIDTH] [MAX_MENU_HEIGHT];
};

NAMESPACE_END

#endif //_MENU_H_
