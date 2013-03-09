#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "common.h"
#include <GL/glx.h>

NAMESPACE_BEGIN

class Context
{
public:
  Context();
 ~Context();

  bool Create(uint width, uint height);
  bool Destroy();
  bool IsValid() const;

  uint GetWidth () const;
  uint GetHeight() const;

private:
  uint       m_width;
  uint       m_height;
  Display*   m_display;
  GLXContext m_context;
  GLXPbuffer m_pbuffer;
  GLuint     m_frameBuffer;
  GLuint     m_renderBuffers[2];
};

NAMESPACE_END

#endif //_CONTEXT_H_
