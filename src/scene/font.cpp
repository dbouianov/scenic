#include "font.h"
#include "displist.h"

NAMESPACE_BEGIN

Font::Font()
:m_base(BAD_DISPLAY_LIST),m_texture (BAD_TEXTURE)
{}

Font::~Font()
{}

bool Font::Create(Texture *pFont, float charSize, uint fontSize)
{
  if (BAD_DISPLAY_LIST == m_base)
  {
    //create font only if not already created
    if (pFont && charSize > 0.0f && DEFAULT_FONT_SIZE == fontSize)
    {
      //create display lists for all font characters
      m_base = glGenLists(DEFAULT_FONT_SIZE);
      if (m_base != BAD_DISPLAY_LIST)
      {
        //save font texture
        m_texture = pFont->GetTextureId();
        //calculate character size in texture coordinates
        float charTexSize = (float)DEFAULT_CHAR_SIZE / DEFAULT_FONT_SIZE;
        float spaceX = charSize * DEFAULT_CHAR_STEP;
        for (uint i = 0; i < DEFAULT_FONT_SIZE; ++i)
        {
          //calculate character texture coordinates
          float x = (i % DEFAULT_CHAR_SIZE) * charTexSize;
          float y = (i / DEFAULT_CHAR_SIZE) * charTexSize;
          //create display list for each character
          glNewList(m_base + i, GL_COMPILE);
            //each character is a textured quad
            glBegin(GL_QUADS);
              glTexCoord2f(x, y + charTexSize);
              glVertex2f(0.0f, 0.0f);

              glTexCoord2f(x + charTexSize, y + charTexSize);
              glVertex2f(charSize, 0.0f);

              glTexCoord2f(x + charTexSize, y);
              glVertex2f(charSize, charSize);

              glTexCoord2f(x, y);
              glVertex2f(0.0f, charSize);
            glEnd();
            //move to the right of the previous character
            glTranslatef(spaceX, 0.0f, 0.0f);
          glEndList();
        }
        return true;
      }
    }
  }
  return false;
}

bool Font::Destroy()
{
  if (m_base != BAD_DISPLAY_LIST)
  {
    //destroy font only if not already destroyed
    glDeleteLists(m_base, DEFAULT_FONT_SIZE);
    m_base = BAD_DISPLAY_LIST;
    m_texture = BAD_TEXTURE;
    return true;
  }
  return false;
}

bool Font::IsValid() const
{
  return m_base != BAD_DISPLAY_LIST;
}

void Font::DrawText(FontStyle style, cchar *szFormat, ...)
{
  if (m_base != BAD_DISPLAY_LIST)
  {
    enum { BUFFER_SIZE = MAX_STRING_SIZE };
    char szBuffer [BUFFER_SIZE + 1];
    va_list args;
    va_start(args, szFormat);
    //format string
    vsnprintf(szBuffer, BUFFER_SIZE, szFormat, args);
    va_end(args);
    //select font texture
    glBindTexture(GL_TEXTURE_2D, m_texture);
    //set display list base
    glListBase(m_base - DEFAULT_FONT_START + (DEFAULT_FONT_SIZE >> 1) * style);
    //save modelview matrix
    glPushMatrix();
      glTranslatef(0.0f, 0.0f, -DEFAULT_NEAR_DISTANCE);
      //draw string characters
      glCallLists(strlen(szBuffer), GL_BYTE, szBuffer);
    //restore modelview matrix
    glPopMatrix();
  }
}

NAMESPACE_END
