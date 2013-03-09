#include "context.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_CONTEXT, "Context",__FILE__);

Context::Context()
: m_width      (0),
  m_height     (0),
  m_display    (NULL),
  m_context    (0),
  m_pbuffer    (0),
  m_frameBuffer(0)
{
  MemZero(m_renderBuffers, sizeof(m_renderBuffers));
}

Context::~Context()
{
  Destroy();
}

bool Context::Create(uint width, uint height)
{
  DbgEnter();

  if (m_display)
  {
    DbgError("GL context alredy created!");
    return false;
  }

  static int attributes[] = {
    GLX_CONFIG_CAVEAT, GLX_NONE,
    GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    None
  };

  //create GL context
  m_display = XOpenDisplay(NULL);
  if (!m_display)
  {
    DbgError("Unable to access display!");
    return false;
  }

  int numConfigs = 0;
  GLXFBConfig *configs = glXChooseFBConfig(m_display, 0, attributes, &numConfigs);
  if (!configs)
  {
    DbgError("Invalid FB configuration!");
    return false;
  }

  m_context = glXCreateNewContext(m_display, configs[0], GLX_RGBA_TYPE, NULL, True);
  m_pbuffer = glXCreatePbuffer   (m_display, configs[0], NULL);
  if (!glXMakeContextCurrent(m_display, m_pbuffer, m_pbuffer, m_context))
  {
    DbgError("Unable to create GL context!");
    return false;
  }

  //initialize GL extensions
  if (glewInit() != GLEW_OK)
  {
    DbgError("Unable to initialize GL extensions!");
    return false;
  }

  //create FBO
  glGenFramebuffersEXT(1, &m_frameBuffer);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBuffer);

  //create color and depth render buffers
  glGenRenderbuffersEXT(2, m_renderBuffers);

  //attach color buffer object
  GLuint colorBuffer = m_renderBuffers[0];
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, colorBuffer);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA, width, height);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
    GL_RENDERBUFFER_EXT, colorBuffer);

  //attach depth and stencil buffer objects
  GLuint depthBuffer = m_renderBuffers[1];
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
    GL_RENDERBUFFER_EXT, depthBuffer);

  //check FBO status
  if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
  {
    DbgError("Failed FBO initialization!");
    return false;
  }

  return true;
}

bool Context::Destroy()
{
  DbgEnter();

  if (!m_display)
  {
    DbgError("GL context already destroyed!");
    return false;
  }

  //destroy FBO and render buffers
  glDeleteRenderbuffersEXT(2, m_renderBuffers);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glDeleteFramebuffersEXT(1, &m_frameBuffer);

  //destroy GL context
  glXMakeCurrent(m_display, None, NULL);
  glXDestroyContext(m_display, m_context);
  glXDestroyPbuffer(m_display, m_pbuffer);
  XCloseDisplay(m_display);

  m_display = NULL;
  return true;
}

bool Context::IsValid() const
{
  return m_display != NULL;
}

uint Context::GetWidth() const
{
  return m_width;
}

uint Context::GetHeight() const
{
  return m_height;
}

NAMESPACE_END
