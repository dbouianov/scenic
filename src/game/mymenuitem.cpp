#include "mymenuitem.h"

NAMESPACE_BEGIN

MenuItem::MenuItem()
  : m_pIcon (NULL),
    m_pFont (NULL),
    m_style (FONT_STYLE_NORMAL)
{}

MenuItem::~MenuItem()
{}

bool MenuItem::Create(const char *szText, SceneNode *pIcon,
                      Font *pFont, FontStyle style)
{
  if (szText && pIcon && pFont)
  {
    //set menu item text
    SetName(szText);
    //save menu item icon
    m_pIcon = pIcon;
    //save menu item font and style
    m_pFont = pFont;
    m_style = style;
    return true;
  }
  return false;
}

bool MenuItem::Destroy()
{
  m_pFont = NULL;
  return true;
}

bool MenuItem::IsValid()
{
  return (bool)m_pFont;
}

void MenuItem::Walk(ulong ulFlags)
{
  if (m_pIcon)
  {
    //draw menu item icon
    m_pIcon->Walk(ulFlags);
  }
  if (m_pFont)
  {
    //draw menu item text
    m_pFont->DrawText(m_style, GetName());
  }
}

void MenuItem::Update(ulong nFrame, float dt, Vector gravity)
{
  if (m_pIcon)
  {
    //update menu item icon
    m_pIcon->Update(nFrame, dt, gravity);
  }
}

NAMESPACE_END
