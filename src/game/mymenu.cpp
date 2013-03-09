#include "mymenu.h"
#include "primitive.h"

NAMESPACE_BEGIN

Menu::Menu()
  : m_nWidth      (0),
    m_nHeight     (0),
    m_fWidth      (0.0f),
    m_fHeight     (0.0f),
    m_fItemWidth  (0.0f),
    m_fItemHeight (0.0f),
    m_fSpaceX     (0.0f),
    m_fSpaceY     (0.0f),
    m_fStartX     (0.0f),
    m_fStartY     (0.0f),
    m_nSelectX    (0),
    m_nSelectY    (0),
    m_nTexture    (BAD_TEXTURE)
{
  MemZero(m_aItems, sizeof(m_aItems));
  ColorZero(m_backColor);
  ColorZero(m_itemColor);
  ColorZero(m_selectColor);
}

Menu::~Menu()
{}

bool Menu::Create(uint nWidth,
                  uint nHeight,
                  float fItemWidth,
                  float fItemHeight,
                  Color itemColor,
                  Color selectColor,
                  Color backColor,
                  Texture *pTexture)
{
  if (nWidth  <= MAX_MENU_WIDTH  &&
      nHeight <= MAX_MENU_HEIGHT &&
      fItemWidth  > 0.0f &&
      fItemHeight > 0.0f &&
      itemColor   &&
      selectColor &&
      backColor)
  {
    //create background display list
    Square square;
    if (m_back.Create(&square))
    {
      //initialize item dimensions
      m_fItemWidth  = fItemWidth;
      m_fItemHeight = fItemHeight;
      //initialize item spacers
      m_fSpaceX = m_fItemWidth  * DEFAULT_MENU_SPACE_FACTOR;
      m_fSpaceY = m_fItemHeight * DEFAULT_MENU_SPACE_FACTOR;
      //initialize menu dimensions
      m_nWidth  = nWidth;
      m_nHeight = nHeight;
      m_fWidth  = m_fSpaceX * (0.5f * m_nWidth  + 0.125f);
      m_fHeight = m_fSpaceY * (0.5f * m_nHeight + 0.125f);
      //initialize item start coordinates
      m_fStartX = -m_fWidth  + m_fSpaceX * 0.625f;
      m_fStartY =  m_fHeight - m_fSpaceY * 0.625f;
      //initialize background color
      ColorCopy(m_backColor, backColor);
      //initialize item color
      ColorCopy(m_itemColor, itemColor);
      //initialize selected item color
      ColorCopy(m_selectColor, selectColor);
      //initialize background texture
      if (pTexture)
      {
        m_nTexture = pTexture->GetTextureId();
      }
      return true;
    }
  }
  return false;
}

bool Menu::Destroy()
{
  //reset menu dimensions
  m_nWidth = m_nHeight = 0;
  return true;
}

bool Menu::IsValid()
{
  return m_nWidth > 0;
}

void Menu::SetItem(uint x, uint y, SceneNode *pItem,
                   MenuCallback pCallback)
{
  if (x < m_nWidth && y < m_nHeight && pItem)
  {
    m_aItems[x][y].pItem = pItem;
    m_aItems[x][y].pCallback = pCallback;
  }
}

void Menu::MoveUp()
{
  if (m_nSelectY > 0)
  {
    m_nSelectY -= 1;
  }
  else if (m_nSelectY <= 0)
  {
    m_nSelectY = m_nHeight - 1;
  }
}

void Menu::MoveDown()
{
  if (m_nSelectY < m_nHeight - 1)
  {
    m_nSelectY += 1;
  }
  else if (m_nSelectY >= m_nHeight - 1)
  {
    m_nSelectY = 0;
  }
}

void Menu::MoveLeft()
{
  if (m_nSelectX > 0)
  {
    m_nSelectX -= 1;
  }
  else if (m_nSelectX <= 0)
  {
    m_nSelectX = m_nWidth - 1;
  }
}

void Menu::MoveRight()
{
  if (m_nSelectX < m_nWidth - 1)
  {
    m_nSelectX += 1;
  }
  else if (m_nSelectX >= m_nWidth - 1)
  {
    m_nSelectX = 0;
  }
}

void Menu::Select()
{
  if (m_nSelectX < m_nWidth && m_nSelectY < m_nHeight)
  {
    MenuCallback pCallback = m_aItems[m_nSelectX][m_nSelectY].pCallback;
    if (pCallback)
    {
      //invoke menu item callback
      (*pCallback)();
    }
  }
}

void Menu::Walk(ulong ulFlags)
{
  //draw menu background
  glPushMatrix();
    //position background
    glTranslatef(0.0f, 0.0f, -DEFAULT_NEAR_DISTANCE - m_fSpaceX);
    //todo: remove hack
    //scale background to required size
    glScalef(m_fWidth, m_fHeight, m_fWidth);
    //apply background color
    glColor4fv(m_backColor);
    //apply background texture
    glBindTexture(GL_TEXTURE_2D, m_nTexture);
    //draw background
    m_back.Walk(ulFlags);
    glBindTexture(GL_TEXTURE_2D, BAD_TEXTURE);
  glPopMatrix();
  //offset menu items from background
  glEnable(GL_POLYGON_OFFSET_FILL);
  //draw all menu items
  for (uint y = 0; y < m_nHeight; ++y)
  {
    for (uint x = 0; x < m_nWidth; ++x)
    {
      SceneNode *pItem = m_aItems[x][y].pItem;
      if (pItem)
      {
        //save modelview matrix
        glPushMatrix();
          //position menu item
          glTranslatef(m_fStartX + x * m_fSpaceX,
                       m_fStartY - y * m_fSpaceY,
                      -DEFAULT_NEAR_DISTANCE - m_fSpaceX);
          //todo: remove hack
          //scale menu item
          glScalef(m_fItemWidth, m_fItemHeight, m_fItemWidth);
          if (m_nSelectX == x && m_nSelectY == y)
          {
            //apply selected item color
            glColor4fv(m_selectColor);
          }
          else
          {
            //apply menu item color
            glColor4fv(m_itemColor);
          }
          //draw menu item
          pItem->Walk(ulFlags);
        //restore modelview matrix
        glPopMatrix();
      }
    }
  }
  //disable polygon offset
  glDisable(GL_POLYGON_OFFSET_FILL);
}

void Menu::Update(ulong nFrame, float dt, Vector gravity)
{
  //update all menu items
  for (uint y = 0; y < m_nHeight; ++y)
  {
    for (uint x = 0; x < m_nHeight; ++x)
    {
      SceneNode *pItem = m_aItems[x][y].pItem;
      if (pItem)
      {
        //update menu item
        pItem->Update(nFrame, dt, gravity);
      }
    }
  }
}

NAMESPACE_END
