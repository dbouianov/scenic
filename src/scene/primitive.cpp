#include "primitive.h"
#include "algebra.h"

NAMESPACE_BEGIN

LOG_DECLARE(LOG_FLAGS_SCENE, "Primitive",__FILE__);

Primitive::Primitive()
{}

Primitive::~Primitive()
{}

bool Primitive::GetBounds(Bounds &bounds)
{
  //find axis aligned bounding box
  Primitive::GetBoundsBox(bounds);
  //find bounding sphere radius
  Primitive::GetBoundsSphere(bounds);
  return true;
}

//static
void Primitive::GetBoundsBox(OUT Bounds &bounds)
{
  //get modelview matrix from OpenGL
  Matrix matrix;
  //preceding transformations were already applied
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
  //transform center of model coordinates
  Point center = POINT_ZERO;
  PointTransform(center, matrix, bounds.center);
  //transform 2 diagonal corners of a unit cube
  Vector cornerA = { 1.0f,  1.0f, 1.0f, 0.0f };
  Vector cornerB = {-1.0f, -1.0f, 1.0f, 0.0f };
  Vector newCornerA;
  Vector newCornerB;
  VectorTransform(cornerA, matrix, newCornerA);
  VectorTransform(cornerB, matrix, newCornerB);
  //calculate coordinates of 4 corners after transformation
  float x = bounds.centerX;
  float y = bounds.centerY;
  float z = bounds.centerZ;
  float ax = newCornerA[CX];
  float ay = newCornerA[CY];
  float az = newCornerA[CZ];
  float bx = newCornerB[CX];
  float by = newCornerB[CY];
  float bz = newCornerB[CZ];
  float cx = x - ax;
  float cy = y - ay;
  float cz = z - az;
  float dx = x - bx;
  float dy = y - by;
  float dz = z - bz;
  ax += x;
  ay += y;
  az += z;
  bx += x;
  by += y;
  bz += z;
  //take smallest x as bounding box min x coordinate
  bounds.minX = FloatMin4(ax, bx, cx, dx);
  //take smallest y as bounding box min y coordinate
  bounds.minY = FloatMin4(ay, by, cy, dy);
  //take smallest z as bounding box min z coordinate
  bounds.minZ = FloatMin4(az, bz, cz, dz);
  //take largest x as bounding box max x coordinate
  bounds.maxX = FloatMax4(ax, bx, cx, dx);
  //take largest y as bounding box max y coordinate
  bounds.maxY = FloatMax4(ay, by, cy, dy);
  //take largest z as bounding box max z coordinate
  bounds.maxZ = FloatMax4(az, bz, cz, dz);
}

//static
void Primitive::GetBoundsSphere(Bounds &bounds)
{
  //find bounding sphere radius
  Vector corner;
  PointSubtractPoint(bounds.maxCorner, bounds.center, corner);
  bounds.rSquared = VectorLengthSquared(corner);
}

//static
void Primitive::DrawBounds(Bounds &bounds)
{
  float  x = bounds.maxX;
  float  y = bounds.maxY;
  float  z = bounds.maxZ;
  float mx = bounds.minX;
  float my = bounds.minY;
  float mz = bounds.minZ;
  glBegin(GL_QUADS);
    //front face
    glVertex3f(mx, my, z);
    glVertex3f( x, my, z);
    glVertex3f( x,  y, z);
    glVertex3f(mx,  y, z);
    //back face
    glVertex3f(mx,  y, mz);
    glVertex3f( x,  y, mz);
    glVertex3f( x, my, mz);
    glVertex3f(mx, my, mz);
    //left face
    glVertex3f(mx, my, mz);
    glVertex3f(mx, my,  z);
    glVertex3f(mx,  y,  z);
    glVertex3f(mx,  y, mz);
    //right face
    glVertex3f(x, my,  z);
    glVertex3f(x, my, mz);
    glVertex3f(x, my, mz);
    glVertex3f(x, my,  z);
    //top face
    glVertex3f(mx, y,  z);
    glVertex3f( x, y,  z);
    glVertex3f( x, y, mz);
    glVertex3f(mx, y, mz);
    //bottom face
    glVertex3f(mx, my, mz);
    glVertex3f( x, my, mz);
    glVertex3f( x, my,  z);
    glVertex3f(mx, my,  z);
  glEnd();
}

Sphere::Sphere()
: Primitive(),
  m_slices (DEFAULT_NUM_SPHERE_SLICES),
  m_stacks (DEFAULT_NUM_SPHERE_STACKS)
{
#ifdef _DEBUG
  SetName("sphere");
#endif
}

Sphere::Sphere(const Sphere &other)
: Primitive(),
  m_slices (other.m_slices),
  m_stacks (other.m_stacks)
{
#ifdef _DEBUG
  SetName("sphere");
#endif
}

Sphere::~Sphere()
{}

void Sphere::SetLod(uint slices, uint stacks)
{
  //todo: add validation
  m_slices = slices;
  m_stacks = stacks;
}

void Sphere::Walk(ulong flags)
{
  //todo: perform intersection test with bounding volume
  //draw sphere using GLU
  GLUquadric *pQuadric = gluNewQuadric();
  if (pQuadric)
  {
    if (flags & NODE_FLAGS_TEXTURE)
    {
      //generate texture coordinates
      gluQuadricTexture(pQuadric, GL_TRUE);
    }
    uint slices = m_slices;
    uint stacks = m_stacks;
    if (flags & NODE_FLAGS_HALFLOD)
    {
      //reduce level of detail
      slices >>= 1;
      stacks >>= 1;
    }
    gluSphere(pQuadric, UNIT_SIZE, slices, stacks);
    gluDeleteQuadric(pQuadric);
  }
}

Cylinder::Cylinder()
: Primitive(),
  m_slices (DEFAULT_NUM_CYLINDER_SLICES),
  m_stacks (DEFAULT_NUM_CYLINDER_STACKS)
{
#ifdef _DEBUG
  SetName("cylinder");
#endif
}

Cylinder::Cylinder(const Cylinder &other)
: Primitive(),
  m_slices (other.m_slices),
  m_stacks (other.m_stacks)
{
#ifdef _DEBUG
  SetName("cylinder");
#endif
}

Cylinder::~Cylinder()
{}

void Cylinder::SetLod(uint slices, uint stacks)
{
  m_slices = slices;
  m_stacks = stacks;
}

void Cylinder::Walk(ulong flags)
{
  //todo: perform intersection test with bounding volume
  GLUquadric *pQuadric = gluNewQuadric();
  if (pQuadric)
  {
    if (flags & NODE_FLAGS_TEXTURE)
    {
      //generate texture coordinates
      gluQuadricTexture(pQuadric, GL_TRUE);
    }
    //push modelview matrix
    glPushMatrix();
      //position cylinder so its center is at the origin
      glTranslatef(0.0f, 0.0f, -UNIT_SIZE);
      ulong slices = m_slices;
      ulong stacks = m_stacks;
      if (flags & NODE_FLAGS_HALFLOD)
      {
        //reduce level of detail
        slices >>= 1;
        stacks >>= 1;
      }
      gluCylinder(pQuadric, UNIT_SIZE, UNIT_SIZE, 2.0f * UNIT_SIZE, slices, stacks);
      gluDeleteQuadric(pQuadric);
    //restore modelview matrix
    glPopMatrix();
  }
}

Cone::Cone()
: Primitive(),
  m_slices (DEFAULT_NUM_CONE_SLICES),
  m_stacks (DEFAULT_NUM_CONE_STACKS)
{
#ifdef _DEBUG
  SetName("cone");
#endif
}

Cone::Cone(const Cone &other)
: Primitive(),
  m_slices (other.m_slices),
  m_stacks (other.m_stacks)
{
#ifdef _DEBUG
  SetName("cone");
#endif
}

Cone::~Cone()
{}

void Cone::SetLod(uint slices, uint stacks)
{
  m_slices = slices;
  m_stacks = stacks;
}

void Cone::Walk(ulong flags)
{
  //todo: perform intersection test with bounding volume
  //draw cone using GLU
  GLUquadric *pQuadric = gluNewQuadric();
  if (pQuadric)
  {
    if (flags & NODE_FLAGS_TEXTURE)
    {
      //generate texture coordinates
      gluQuadricTexture(pQuadric, GL_TRUE);
    }
    //push modelview matrix
    glPushMatrix();
      //position cone so its center is at the origin
      glTranslatef(0.0f, 0.0f, -UNIT_SIZE);
      uint slices = m_slices;
      uint stacks = m_stacks;
      if (flags & NODE_FLAGS_HALFLOD)
      {
        //reduce level of detail
        slices >>= 1;
        stacks >>= 1;
      }
      gluCylinder(pQuadric, UNIT_SIZE, 0.0f, 2.0f * UNIT_SIZE, slices, stacks);
      gluDeleteQuadric(pQuadric);
    //restore modelview matrix
    glPopMatrix();
  }
}

Cube::Cube()
: Primitive(),
  m_sizeX (DEFAULT_CUBE_SIZE_X),
  m_sizeY (DEFAULT_CUBE_SIZE_Y)
{
#ifdef _DEBUG
  SetName("cube");
#endif
}

Cube::Cube(const Cube &other)
: Primitive(),
  m_sizeX  (other.m_sizeX),
  m_sizeY  (other.m_sizeY)
{
#ifdef _DEBUG
  SetName("cube");
#endif
}

Cube::~Cube()
{}

void Cube::SetLod(uint sizeX, uint sizeY)
{
  m_sizeX = sizeX;
  m_sizeY = sizeY;
}

void Cube::Walk(ulong flags)
{
  //todo: perform intersection test with bounding volume
  uint sizeX = m_sizeX;
  uint sizeY = m_sizeY;
  if (flags & NODE_FLAGS_HALFLOD)
  {
    //reduce level of detail
    sizeX >>= 1;
    sizeY >>= 1;
  }
  if (flags & NODE_FLAGS_TEXTURE)
  {
    //generate texture coordinates
    float texStepX = 1.0f / sizeX;
    float texStepY = 1.0f / sizeY;
    float stepX = 2.0f * UNIT_SIZE * texStepX;
    float stepY = 2.0f * UNIT_SIZE * texStepY;
    float stepZ = stepX;
    float texX, texY, x, y, z;
    glBegin(GL_QUADS);
      //front face
      glNormal3f(0.0f, 0.0f, 1.0f);
      texY = 1.0f;
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        texX = 0.0f;
        for (x = -UNIT_SIZE; x < UNIT_SIZE && !FloatIsZero(x - UNIT_SIZE); x += stepX)
        {
          glTexCoord2f(texX, texY);
          glVertex3f(x, y, UNIT_SIZE);

          glTexCoord2f(texX + texStepX, texY);
          glVertex3f(x + stepX, y, UNIT_SIZE);

          glTexCoord2f(texX + texStepX, texY - texStepY);
          glVertex3f(x + stepX, y + stepY, UNIT_SIZE);

          glTexCoord2f(texX, texY - texStepY);
          glVertex3f(x, y + stepY, UNIT_SIZE);

          texX += texStepX;
        }
        texY -= texStepY;
      }
      //back face
      glNormal3f(0.0f, 0.0f, -1.0f);
      texY = 1.0f;
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        texX = 0.0f;
        for (x = UNIT_SIZE; x > -UNIT_SIZE && !FloatIsZero(x + UNIT_SIZE); x -= stepX)
        {
          glTexCoord2f(texX, texY);
          glVertex3f(x, y, -UNIT_SIZE);

          glTexCoord2f(texX + texStepX, texY);
          glVertex3f(x - stepX, y, -UNIT_SIZE);

          glTexCoord2f(texX + texStepX, texY - texStepY);
          glVertex3f(x - stepX, y + stepY, -UNIT_SIZE);

          glTexCoord2f(texX, texY - texStepY);
          glVertex3f(x, y + stepY, -UNIT_SIZE);

          texX += texStepX;
        }
        texY -= texStepY;
      }
      //left face
      glNormal3f(-1.0f, 0.0f, 0.0f);
      texY = 1.0f;
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        texX = 0.0f;
        for (z = -UNIT_SIZE; z < UNIT_SIZE && !FloatIsZero(z - UNIT_SIZE); z += stepZ)
        {
          glTexCoord2f(texX, texY);
          glVertex3f(-UNIT_SIZE, y, z);

          glTexCoord2f(texX + texStepX, texY);
          glVertex3f(-UNIT_SIZE, y, z + stepZ);

          glTexCoord2f(texX + texStepX, texY - texStepY);
          glVertex3f(-UNIT_SIZE, y + stepY, z + stepZ);

          glTexCoord2f(texX, texY - texStepY);
          glVertex3f(-UNIT_SIZE, y + stepY, z);

          texX += texStepX;
        }
        texY -= texStepY;
      }
      //right face
      glNormal3f(1.0f, 0.0f, 0.0f);
      texY = 1.0f;
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        texX = 0.0f;
        for (z = UNIT_SIZE; z > -UNIT_SIZE && !FloatIsZero(z + UNIT_SIZE); z -= stepZ)
        {
          glTexCoord2f(texX, texY);
          glVertex3f(UNIT_SIZE, y, z);

          glTexCoord2f(texX + texStepX, texY);
          glVertex3f(UNIT_SIZE, y, z - stepZ);

          glTexCoord2f(texX + texStepX, texY - texStepY);
          glVertex3f(UNIT_SIZE, y + stepY, z - stepZ);

          glTexCoord2f(texX, texY - texStepY);
          glVertex3f(UNIT_SIZE, y + stepY, z);

          texX += texStepX;
        }
        texY -= texStepY;
      }
      //top face
      glNormal3f(0.0f, 1.0f, 0.0f);
      texY = 1.0f;
      for (z = UNIT_SIZE; z > -UNIT_SIZE && !FloatIsZero(z + UNIT_SIZE); z -= stepZ)
      {
        texX = 0.0f;
        for (x = -UNIT_SIZE; x < UNIT_SIZE && !FloatIsZero(x - UNIT_SIZE); x += stepX)
        {
          glTexCoord2f(texX, texY);
          glVertex3f(x, UNIT_SIZE, z);

          glTexCoord2f(texX + texStepX, texY);
          glVertex3f(x + stepX, UNIT_SIZE, z);

          glTexCoord2f(texX + texStepX, texY - texStepY);
          glVertex3f(x + stepX, UNIT_SIZE, z - stepZ);

          glTexCoord2f(texX, texY - texStepY);
          glVertex3f(x, UNIT_SIZE, z - stepZ);

          texX += texStepX;
        }
        texY -= texStepY;
      }
      //bottom face
      glNormal3f(0.0f, -1.0f, 0.0f);
      texY = 1.0f;
      for (z = -UNIT_SIZE; z < UNIT_SIZE && !FloatIsZero(z - UNIT_SIZE); z += stepZ)
      {
        texX = 0.0f;
        for (x = -UNIT_SIZE; x < UNIT_SIZE && !FloatIsZero(x - UNIT_SIZE); x += stepX)
        {
          glTexCoord2f(texX, texY);
          glVertex3f(x, -UNIT_SIZE, z);

          glTexCoord2f(texX + texStepX, texY);
          glVertex3f(x + stepX, -UNIT_SIZE, z);

          glTexCoord2f(texX + texStepX, texY - texStepY);
          glVertex3f(x + stepX, -UNIT_SIZE, z + stepZ);

          glTexCoord2f(texX, texY - texStepY);
          glVertex3f(x, -UNIT_SIZE, z + stepZ);

          texX += texStepX;
        }
        texY -= texStepY;
      }
    glEnd();
  }
  else
  {
    //no texture coordinates
    float stepX = 2.0f * UNIT_SIZE / sizeX;
    float stepY = 2.0f * UNIT_SIZE / sizeY;
    float stepZ = stepX;
    float x, y, z;
    glBegin(GL_QUADS);
      //front face
      glNormal3f(0.0f, 0.0f, 1.0f);
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        for (x = -UNIT_SIZE; x < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); x += stepX)
        {
          glVertex3f(x,         y,         UNIT_SIZE);
          glVertex3f(x + stepX, y,         UNIT_SIZE);
          glVertex3f(x + stepX, y + stepY, UNIT_SIZE);
          glVertex3f(x,         y + stepY, UNIT_SIZE);
        }
      }
      //back face
      glNormal3f(0.0f, 0.0f, -1.0f);
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        for (x = UNIT_SIZE; x > -UNIT_SIZE && !FloatIsZero(x - UNIT_SIZE); x -= stepX)
        {
          glVertex3f(x,         y,         -UNIT_SIZE);
          glVertex3f(x - stepX, y,         -UNIT_SIZE);
          glVertex3f(x - stepX, y + stepY, -UNIT_SIZE);
          glVertex3f(x,         y + stepY, -UNIT_SIZE);
        }
      }
      //left face
      glNormal3f(-1.0f, 0.0f, 0.0f);
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        for (z = -UNIT_SIZE; z < UNIT_SIZE && !FloatIsZero(z - UNIT_SIZE); z += stepZ)
        {
          glVertex3f(-UNIT_SIZE, y,         z);
          glVertex3f(-UNIT_SIZE, y,         z + stepZ);
          glVertex3f(-UNIT_SIZE, y + stepY, z + stepZ);
          glVertex3f(-UNIT_SIZE, y + stepY, z);
        }
      }
      //right face
      glNormal3f(1.0f, 0.0f, 0.0f);
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        for (z = UNIT_SIZE; z > -UNIT_SIZE && !FloatIsZero(z + UNIT_SIZE); z -= stepZ)
        {
          glVertex3f(UNIT_SIZE, y,         z);
          glVertex3f(UNIT_SIZE, y,         z - stepZ);
          glVertex3f(UNIT_SIZE, y + stepY, z - stepZ);
          glVertex3f(UNIT_SIZE, y + stepY, z);
        }
      }
      //top face
      glNormal3f(0.0f, 1.0f, 0.0f);
      for (z = UNIT_SIZE; z > -UNIT_SIZE && !FloatIsZero(z + UNIT_SIZE); z -= stepZ)
      {
        for (x = -UNIT_SIZE; x < UNIT_SIZE && !FloatIsZero(x - UNIT_SIZE); x += stepX)
        {
          glVertex3f(x,         UNIT_SIZE, z);
          glVertex3f(x + stepX, UNIT_SIZE, z);
          glVertex3f(x + stepX, UNIT_SIZE, z - stepZ);
          glVertex3f(x,         UNIT_SIZE, z - stepZ);
        }
      }
      //bottom face
      glNormal3f(0.0f, -1.0f, 0.0f);
      for (z = -UNIT_SIZE; z < UNIT_SIZE && !FloatIsZero(z - UNIT_SIZE); z += stepZ)
      {
        for (x = -UNIT_SIZE; x < UNIT_SIZE && !FloatIsZero(x - UNIT_SIZE); x += stepX)
        {
          glVertex3f(x,         -UNIT_SIZE, z);
          glVertex3f(x + stepX, -UNIT_SIZE, z);
          glVertex3f(x + stepX, -UNIT_SIZE, z + stepZ);
          glVertex3f(x,         -UNIT_SIZE, z + stepZ);
        }
      }
    glEnd();
  }
}

Disk::Disk()
: Primitive(),
  m_slices (DEFAULT_NUM_DISK_SLICES),
  m_loops  (DEFAULT_NUM_DISK_LOOPS)
{
#ifdef _DEBUG
  SetName("disk");
#endif
}

Disk::Disk(const Disk &other)
: Primitive(),
  m_slices (other.m_slices),
  m_loops  (other.m_loops)
{
#ifdef _DEBUG
  SetName("disk");
#endif
}

Disk::~Disk()
{}

void Disk::SetLod(uint slices, uint loops)
{
  m_slices = slices;
  m_loops  = loops;
}

void Disk::Walk(ulong flags)
{
  //todo: perform intersection test with bounding volume
  GLUquadric *pQuadric = gluNewQuadric();
  if (pQuadric)
  {
    if (flags & NODE_FLAGS_TEXTURE)
    {
      //generate texture coordinates
      gluQuadricTexture(pQuadric, GL_TRUE);
    }
    uint slices = m_slices;
    uint loops  = m_loops;
    if (flags & NODE_FLAGS_HALFLOD)
    {
      //reduce level of detail
      slices >>= 1;
      loops  >>= 1;
    }
    gluDisk(pQuadric, 0.0f, UNIT_SIZE, slices, loops);
    gluDeleteQuadric(pQuadric);
  }
}

Square::Square()
: Primitive(),
  m_sizeX (DEFAULT_SQUARE_SIZE_X),
  m_sizeY (DEFAULT_SQUARE_SIZE_Y)
{
#ifdef _DEBUG
  SetName("square");
#endif
}

Square::Square(const Square &other)
: Primitive(),
  m_sizeX (other.m_sizeX),
  m_sizeY (other.m_sizeY)
{
#ifdef _DEBUG
  SetName("square");
#endif
}

Square::~Square()
{}

void Square::SetLod(uint sizeX, uint sizeY)
{
  m_sizeX = sizeX;
  m_sizeY = sizeY;
}

void Square::Walk(ulong flags)
{
  //todo: perform intersection test with bounding volume
  uint sizeX = m_sizeX;
  uint sizeY = m_sizeY;
  if (flags & NODE_FLAGS_HALFLOD)
  {
    //reduce level of detail
    sizeX >>= 1;
    sizeY >>= 1;
  }
  if (flags & NODE_FLAGS_TEXTURE)
  {
    //generate texture coordinates
    float texStepX = 1.0f / sizeX;
    float texStepY = 1.0f / sizeY;
    float stepX = 2.0f * UNIT_SIZE * texStepX;
    float stepY = 2.0f * UNIT_SIZE * texStepY;
    float texX, texY, x, y;
    glBegin(GL_QUADS);
      glNormal3f(0.0f, 0.0f, 1.0f);
      texY = 1.0f;
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        texX = 0.0f;
        for (x = -UNIT_SIZE; x < UNIT_SIZE && !FloatIsZero(x - UNIT_SIZE); x += stepX)
        {
          glTexCoord2f(texX, texY);
          glVertex3f(x, y, 0.0f);

          glTexCoord2f(texX + texStepX, texY);
          glVertex3f(x + stepX, y, 0.0f);

          glTexCoord2f(texX + texStepX, texY - texStepY);
          glVertex3f(x + stepX, y + stepY, 0.0f);

          glTexCoord2f(texX, texY - texStepY);
          glVertex3f(x, y + stepY, 0.0f);

          texX += texStepX;
        }
        texY -= texStepY;
      }
    glEnd();
  }
  else
  {
    //no texture coordinates
    float stepX = 2.0f * UNIT_SIZE / sizeX;
    float stepY = 2.0f * UNIT_SIZE / sizeY;
    float x, y;
    glBegin(GL_QUADS);
      glNormal3f(0.0f, 0.0f, 1.0f);
      for (y = -UNIT_SIZE; y < UNIT_SIZE && !FloatIsZero(y - UNIT_SIZE); y += stepY)
      {
        for (x = -UNIT_SIZE; x < UNIT_SIZE && !FloatIsZero(x - UNIT_SIZE); x += stepX)
        {
          glVertex3f(x,         y,         UNIT_SIZE);
          glVertex3f(x + stepX, y,         UNIT_SIZE);
          glVertex3f(x + stepX, y + stepY, UNIT_SIZE);
          glVertex3f(x,         y + stepY, UNIT_SIZE);
        }
      }
    glEnd();
  }
}

Circle::Circle()
: Primitive(),
  m_slices (DEFAULT_NUM_CIRCLE_SLICES)
{
#ifdef _DEBUG
  SetName("circle");
#endif
}

Circle::Circle(const Circle &other)
: Primitive(),
  m_slices (other.m_slices)
{
#ifdef _DEBUG
  SetName("circle");
#endif
}

Circle::~Circle()
{}

void Circle::SetLod(uint slices)
{
  m_slices = slices;
}

void Circle::Walk(ulong flags)
{
  //todo: perform intersection test with bounding volume
  //draw unit circle
  glBegin(GL_LINE_LOOP);
    glNormal3f(0.0f, 0.0f, 1.0f);
    uint slices = m_slices;
    if (flags & NODE_FLAGS_HALFLOD)
    {
      //reduce level of detail
      slices >>= 1;
    }
    float a = TWO_PI / slices;
    for (uint slice = 0; slice < slices; ++slice)
    {
      float d = RadToDeg(slice * a);
      glVertex2f(UNIT_SIZE * FastCos(d), UNIT_SIZE * FastSin(d));
    }
  glEnd();
}

CrossHair::CrossHair()
: m_size   (DEFAULT_CROSSHAIR_SIZE),
  m_slices (DEFAULT_NUM_CROSSHAIR_SLICES)
{
#ifdef _DEBUG
  SetName("crosshair");
#endif
}

CrossHair::~CrossHair()
{}

void CrossHair::SetLod(float size, uint slices)
{
  m_size = size;
  m_slices = slices;
}

void CrossHair::Walk(ulong flags)
{
  glPushMatrix();
    glTranslatef(0.0f, 0.0f, -DEFAULT_NEAR_DISTANCE);
    float sizeInner = m_size * 0.5f;
    glNormal3f(0.0f, 0.0f, 1.0f);
    //crosshair lines
    glBegin(GL_LINES);
      glVertex2f(sizeInner, sizeInner);
      glVertex2f(m_size, m_size);

      glVertex2f(-sizeInner, sizeInner);
      glVertex2f(-m_size, m_size);

      glVertex2f(-sizeInner, -sizeInner);
      glVertex2f(-m_size, -m_size);

      glVertex2f(sizeInner, -sizeInner);
      glVertex2f(m_size, -m_size);
    glEnd();
    //circle in the middle
    sizeInner *= DEFAULT_CROSSHAIR_CIRCLE_FACTOR;
    glBegin(GL_LINE_LOOP);
      uint slices = m_slices;
      if (flags & NODE_FLAGS_HALFLOD)
      {
        //reduce level of detail
        slices >>= 1;
      }
      float a = TWO_PI / slices;
      for (uint slice = 0; slice < slices; ++slice)
      {
        float d = RadToDeg(slice * a);
        glVertex2f(sizeInner * FastCos(d), sizeInner * FastSin(d));
      }
    glEnd();
  glPopMatrix();
}

NAMESPACE_END
