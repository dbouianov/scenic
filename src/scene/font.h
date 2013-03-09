#ifndef _FONT_H_
#define _FONT_H_

#include "common.h"
#include "texture.h"

NAMESPACE_BEGIN

//font style
enum FontStyle
{
  FONT_STYLE_NORMAL = 0,
  FONT_STYLE_ITALIC,
};

//font
class Font : public Object
{
public:
  Font();
  virtual ~Font();

  bool Create (Texture *pFont, float charSize, uint fontSize);
  bool Destroy();
  bool IsValid() const;

  void DrawText(FontStyle style, cchar *szFormat, ...);

private:
  //font display list base
  GLuint m_base;
  //font texture id
  GLuint m_texture;
};

NAMESPACE_END

#endif //_FONT_H_
