#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "scenenode.h"

NAMESPACE_BEGIN

#define UNIT_SIZE         DEFAULT_UNIT_SIZE
#define UNIT_HALF_SIZE    DEFAULT_UNIT_HALF_SIZE

enum CubeCorner
{
  CUBE_CORNER_FRONT_TOP_LEFT = 0,
  CUBE_CORNER_FRONT_TOP_RIGHT,
  CUBE_CORNER_FRONT_BOTTOM_LEFT,
  CUBE_CORNER_FRONT_BOTTOM_RIGHT,
  CUBE_CORNER_BACK_TOP_LEFT,
  CUBE_CORNER_BACK_TOP_RIGHT,
  CUBE_CORNER_BACK_BOTTOM_LEFT,
  CUBE_CORNER_BACK_BOTTOM_RIGHT,
  CUBE_CORNER_FIRST   = CUBE_CORNER_FRONT_TOP_LEFT,
  CUBE_CORNER_LAST    = CUBE_CORNER_BACK_BOTTOM_RIGHT,
  NUM_CUBE_CORNERS    = CUBE_CORNER_LAST + 1,
};

enum CubeFace
{
  CUBE_FACE_FRONT = 0,
  CUBE_FACE_LEFT,
  CUBE_FACE_BACK,
  CUBE_FACE_RIGHT,
  CUBE_FACE_TOP,
  CUBE_FACE_BOTTOM,
  CUBE_FACE_FIRST   = CUBE_FACE_FRONT,
  CUBE_FACE_LAST    = CUBE_FACE_BOTTOM,
  NUM_CUBE_FACES    = CUBE_FACE_LAST + 1,
};

class Primitive : public SceneNode
{
protected:
  Primitive();

public:
  virtual ~Primitive() = 0;

  //find bounding volume
  virtual bool GetBounds(OUT Bounds &bounds);

  //find axis aligned bounding box
  static void GetBoundsBox(OUT Bounds &bounds);

  //find bounding sphere radius
  static void GetBoundsSphere(OUT Bounds &bounds);

  //draw bounding volume
  static void DrawBounds(Bounds &bounds);
};

class Sphere : public Primitive
{
public:
  Sphere();
  Sphere(const Sphere &other);
  virtual ~Sphere();

  void SetLod(uint slices, uint stacks);

  virtual void Walk(ulong flags = NODE_FLAGS_PRIMITIVE);

private:
  uint m_slices;
  uint m_stacks;
};

class Cylinder : public Primitive
{
public:
  Cylinder();
  Cylinder(const Cylinder &other);
  virtual ~Cylinder();

  void SetLod(uint slices, uint stacks);

  virtual void Walk(ulong flags = NODE_FLAGS_PRIMITIVE);

private:
  uint m_slices;
  uint m_stacks;
};

class Cone : public Primitive
{
public:
  Cone();
  Cone(const Cone &other);
  virtual ~Cone();

  void SetLod(uint slices, uint stacks);

  virtual void Walk(ulong flags = NODE_FLAGS_PRIMITIVE);

private:
  uint m_slices;
  uint m_stacks;
};

class Cube : public Primitive
{
public:
  Cube();
  Cube(const Cube &other);
  virtual ~Cube();

  void SetLod(uint sizeX, uint sizeY);

  virtual void Walk(ulong flags = NODE_FLAGS_PRIMITIVE);

private:
  //number of face divisions in x and y directions
  uint m_sizeX;
  uint m_sizeY;
};

class Disk : public Primitive
{
public:
  Disk();
  Disk(const Disk &other);
  virtual ~Disk();

  void SetLod(uint slices, uint loops);

  virtual void Walk(ulong flags = NODE_FLAGS_PRIMITIVE);

private:
  uint m_slices;
  uint m_loops;
};

class Square : public Primitive
{
public:
  Square();
  Square(const Square &other);
  virtual ~Square();

  void SetLod(uint sizeX, uint sizeY);

  virtual void Walk(ulong flags = NODE_FLAGS_PRIMITIVE);

private:
  //number of face divisions in x and y directions
  uint m_sizeX;
  uint m_sizeY;
};

class Circle : public Primitive
{
public:
  Circle();
  Circle(const Circle &other);
  virtual ~Circle();

  void SetLod(uint slices);

  virtual void Walk(ulong flags = NODE_FLAGS_PRIMITIVE);

private:
  uint m_slices;
};

class CrossHair : public SceneNode
{
public:
  CrossHair();
  virtual ~CrossHair();

  void SetLod(float size, uint slices);

  virtual void Walk(ulong flags = NODE_FLAGS_PRIMITIVE);

private:
  float m_size;
  uint  m_slices;
};

NAMESPACE_END

#endif //_PRIMITIVE_H_
