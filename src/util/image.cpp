#include "image.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_IMAGE, "Image",__FILE__);

//generic image interface
Image::Image()
: m_width      (BAD_SIZE),
  m_height     (BAD_SIZE),
  m_bpp        (BAD_SIZE),
  m_pImageData (NULL)
{}

Image::~Image()
{
  //free image data if necessary
  delete m_pImageData;
}

bool Image::Create(uint width, uint height, uint bpp)
{
  if (!m_pImageData && IMAGE_BPP_8 == bpp &&
      width > IMAGE_MIN_WIDTH && height > IMAGE_MIN_HEIGHT)
  {
    //create image only if not already created
    ulong size = width * height * IMAGE_NUM_COMPONENTS;
    m_pImageData = new ubyte [size];
    MemZero(m_pImageData, size);
    m_width  = width;
    m_height = height;
    m_bpp    = bpp;
    return true;
  }
  return false;
}

bool Image::Destroy()
{
  if (m_pImageData)
  {
    //destroy image onloy if already created
    delete m_pImageData;
    m_pImageData = NULL;
    m_width  = BAD_SIZE;
    m_height = BAD_SIZE;
    m_bpp    = BAD_SIZE;
    return true;
  }
  return false;
}

uint Image::GetWidth() const
{
  return m_width;
}

uint Image::GetHeight() const
{
  return m_height;
}

uint Image::GetBpp() const
{
  return m_bpp;
}

ubyte *Image::GetImageData()
{
  return m_pImageData;
}

void Image::SetPixel(uint x, uint y, Color color)
{
  ubyte *pPixel = GetPixel(x, y);
  if (pPixel)
  {
    float c = color[CR];
    if (c > 1.0f) c = 1.0f;
    pPixel[CR] = (ubyte)(IMAGE_MAX_BYTE * c);

    c = color[CG];
    if (c > 1.0f) c = 1.0f;
    pPixel[CG] = (ubyte)(IMAGE_MAX_BYTE * c);

    c = color[CB];
    if (c > 1.0f) c = 1.0f;
    pPixel[CB] = (ubyte)(IMAGE_MAX_BYTE * c);

    c = color[CA];
    if (c > 1.0f) c = 1.0f;
    pPixel[CA] = (ubyte)(IMAGE_MAX_BYTE * c);
  }
}

ubyte *Image::GetPixel(uint x, uint y)
{
  if (m_pImageData && x < m_width && y < m_height)
  {
    return &m_pImageData[(y * m_width + x) * IMAGE_NUM_COMPONENTS];
  }
  return NULL;
}

bool Image::IsValid() const
{
  return (bool)m_pImageData;
}

bool Image::Load(cchar *szFile)
{
  if (!szFile)
  {
    DbgError("Null file name!");
    return false;
  }

  DbgMessage("Image %s", szFile);

  FILE *pFile = fopen(szFile, "rb");
  if (!pFile)
  {
    DbgError("Unable to open file %s for reading!", szFile);
    return false;
  }

  bool success = Load(pFile);
  fclose(pFile);
  return success;
}

bool Image::Save(cchar *szFile)
{
  if (!szFile)
  {
    DbgError("Null file name!");
    return false;
  }

  DbgMessage("Image %s", szFile);

  FILE *pFile = fopen(szFile, "wb");
  if (!pFile)
  {
    DbgError("Unable to open file %s for writing!", szFile);
    return false;
  }

  bool success = Save(pFile);
  fclose(pFile);
  return success;
}

//png image
PngImage::PngImage()
{}

PngImage::~PngImage()
{}

bool PngImage::Load(FILE *pStream)
{
  if (pStream && !IsValid())
  {
    bool success = false;
    ubyte aPngHeader[PNG_IMAGE_HEADER_SIZE];
    png_structp pPngStruct = NULL;
    png_infop   pPngInfo   = NULL;
    //float gamma = 0.0;
    uint rowSize;
    ubyte     *pImageData = NULL;
    png_bytep *pImageRows = NULL;
    uint offset = 0;
    png_uint_32 width  = 0;
    png_uint_32 height = 0;
    int bpp = 0;
    int colorType = 0;

    //check png signature
    if (sizeof(aPngHeader) != fread(aPngHeader, 1, sizeof(aPngHeader), pStream))
    {
      //read failed
      goto Exit_Load;
    }
    if (!png_check_sig(aPngHeader, sizeof(aPngHeader)))
    {
      //file is not a png image
      goto Exit_Load;
    }

    //create png read struct
    pPngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pPngStruct)
    {
      //png struct allocation failed
      goto Exit_Load;
    }

    //create png info struct
    pPngInfo = png_create_info_struct(pPngStruct);
    if (!pPngInfo)
    {
      //png info allocation failed
      goto Exit_Load;
    }

    //required for png library use
    if (setjmp(pPngStruct->jmpbuf))
    {
      //handle png read error
      goto Exit_Load;
    }

    //initialize png io
    png_init_io(pPngStruct, pStream);

    //set offset into the file since png header has been already read 
    png_set_sig_bytes(pPngStruct, sizeof(aPngHeader));

    //read png image info header
    png_read_info(pPngStruct, pPngInfo);
    png_get_IHDR(pPngStruct, pPngInfo, &width, &height, &bpp, &colorType,
      NULL, NULL, NULL);

    DbgMessage("Image %ux%ux%ubpp", width, height, bpp);

    //filter out unsupported images
    if (bpp < PNG_IMAGE_MIN_BPP || bpp > PNG_IMAGE_MAX_BPP)
    {
      goto Exit_Load;
    }

    //convert palette images to rgb
    if (PNG_COLOR_TYPE_PALETTE == colorType)
    {
      png_set_expand(pPngStruct);
    }

    //convert low-bpp images to 8 bpp
    if (bpp < IMAGE_BPP_8)
    {
      png_set_expand(pPngStruct);
    }

    //strip alpha channel
    if ((colorType & PNG_COLOR_MASK_ALPHA) ||
      png_get_valid(pPngStruct, pPngInfo, PNG_INFO_tRNS))
    {
      png_set_strip_alpha(pPngStruct);
    }

    //strip 16 bpp images down to 8 bpp
    if (IMAGE_BPP_16 == bpp)
    {
      png_set_strip_16(pPngStruct);
    }

    //convert grayscale images to rgb
    if (PNG_COLOR_TYPE_GRAY == colorType || PNG_COLOR_TYPE_GRAY_ALPHA == colorType)
    {
      //png_set_expand(pPngStruct);
      png_set_gray_to_rgb(pPngStruct);
    }

    //perform gamma correction if gamma is preset
    /*if (png_get_gAMA(pPngStruct, pPngInfo, &gamma))
    {
      //png_set_gamma(pPngStruct, ???, gamma);
    }*/

    //update image info
    png_read_update_info(pPngStruct, pPngInfo);

    //allocate space for image row offsets
    rowSize = png_get_rowbytes(pPngStruct, pPngInfo);
    pImageData = new ubyte [height * rowSize];
    if (!pImageData)
    {
      //allocation failed
      goto Exit_Load;
    }

    //allocate space for image row pointers
    pImageRows = new png_bytep [height];
    if (!pImageRows)
    {
      //allocation failed
      goto Exit_Load;
    }

    //set individual row pointers to correct offsets
    for (uint index = 0; index < height; ++index)
    {
      pImageRows[index] = pImageData + offset;
      offset += rowSize;
    }

    //read image data
    png_read_image(pPngStruct, pImageRows);
    png_read_end(pPngStruct, NULL);

    //set image properties
    m_pImageData = pImageData;
    m_width  = static_cast<uint>(width);
    m_height = static_cast<uint>(height);
    m_bpp    = static_cast<uint>(bpp);

    //image successfully loaded
    success = true;

Exit_Load:
    delete pImageRows;
    if (pPngStruct && pPngInfo)
    {
      //free png struct and png info
      png_destroy_read_struct(&pPngStruct, &pPngInfo, NULL);
    }
    else if (pPngStruct)
    {
      //free png struct only
      png_destroy_read_struct(&pPngStruct, NULL, NULL);
    }
    if (!success)
    {
      delete pImageData;
    }
    return success;
  }
  return false;
}

bool PngImage::Save(FILE *pStream)
{
  if (pStream && IsValid())
  {
    bool success = false;
    png_structp pPngStruct = NULL;
    png_infop   pPngInfo   = NULL;
    int bpp = IMAGE_BPP_8;
    int colorType = PNG_COLOR_TYPE_RGB;
    png_bytep *pImageRows = NULL;
    uint rowSize;
    uint offset = 0;

    //create png write struct
    pPngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pPngStruct)
    {
      //png struct allocation failed
      goto Exit_Save;
    }

    //create png info struct
    pPngInfo = png_create_info_struct(pPngStruct);
    if (!pPngInfo)
    {
      //png info allocation failed
      goto Exit_Save;
    }

    //required for png library use
    if (setjmp(pPngStruct->jmpbuf))
    {
      //handle png write error
      goto Exit_Save;
    }

    //initialize png io
    png_init_io(pPngStruct, pStream);

    //use paeth filtering
    png_set_filter(pPngStruct, 0, PNG_FILTER_PAETH);

    //set compression level
    png_set_compression_level(pPngStruct, Z_BEST_COMPRESSION);

    //write png image info header
    png_set_IHDR(pPngStruct, pPngInfo, m_width, m_height, bpp, colorType,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(pPngStruct, pPngInfo);

    //allocate space for image row pointers
    pImageRows = new png_bytep [m_height];
    if (!pImageRows)
    {
      //allocation failed
      goto Exit_Save;
    }

    //set individual row pointers to correct offsets
    rowSize = png_get_rowbytes(pPngStruct, pPngInfo);
    for (uint i = 0; i < m_height; ++i)
    {
      pImageRows[i] = m_pImageData + offset;
      offset += rowSize;
    }

    //write image data
    png_write_image(pPngStruct, pImageRows);
    png_write_end(pPngStruct, NULL);

    //image successfully saved
    success = true;

Exit_Save:
    delete pImageRows;
    if (pPngStruct && pPngInfo)
    {
      //free png struct and png info
      png_destroy_write_struct(&pPngStruct, &pPngInfo);
    }
    else if (pPngStruct)
    {
      //free png struct only
      png_destroy_write_struct(&pPngStruct, NULL);
    }
    return success;
  }
  return false;
}

NAMESPACE_END
