#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "common.h"
#include "object.h"
#include "algebra.h"
#include <png.h>

NAMESPACE_BEGIN

//generic image definitions
enum
{
  IMAGE_MIN_WIDTH       = 16,
  IMAGE_MIN_HEIGHT      = 16,
  IMAGE_NUM_COMPONENTS  = 3,
  IMAGE_BITS_PER_BYTE   = 8,
  IMAGE_BPP_1           = 1,
  IMAGE_BPP_2           = 2,
  IMAGE_BPP_8           = 8,
  IMAGE_BPP_16          = 16,
  IMAGE_MAX_BYTE        = 0xff,
};

//generic image interface
class Image : public Object
{
protected:
  Image();

public:
  virtual ~Image();

  bool Create (uint width, uint height, uint bpp);
  bool Destroy();

  uint GetWidth () const;
  uint GetHeight() const;
  uint GetBpp   () const;

  void   SetPixel(uint x, uint y, Color color);
  ubyte *GetPixel(uint x, uint y);

  ubyte *GetImageData();
  bool   IsValid() const;

  virtual bool Load(FILE *pStream) = 0;
  virtual bool Save(FILE *pStream) = 0;

  virtual bool Load(cchar *szFile);
  virtual bool Save(cchar *szFile);

protected:
  uint m_width;
  uint m_height;
  uint m_bpp;
  ubyte *m_pImageData;
};

//png image definitions
enum
{
  PNG_IMAGE_HEADER_SIZE = 8,
  PNG_IMAGE_MIN_BPP     = IMAGE_BPP_8,
  PNG_IMAGE_MAX_BPP     = IMAGE_BPP_16,
};

//png image
class PngImage : public Image
{
public:
  PngImage();
  virtual ~PngImage();

  virtual bool Load(FILE *pStream);
  virtual bool Save(FILE *pStream);
};

NAMESPACE_END

#endif //_IMAGE_H_
