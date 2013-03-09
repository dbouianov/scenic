#ifndef _ALGEBRA_H_
#define _ALGEBRA_H_

#include "common.h"
#include "floatmath.h"
#include "solver.h"
#include "point.h"
#include "vector.h"
#include "plane.h"
#include "matrix.h"
#include "quaternion.h"
#include "color.h"

NAMESPACE_BEGIN

//add a vector to a point
INLINE void PointAddVector(Point a, Vector v, OUT Point p)
{
  p[CX] = a[CX] + v[CX];
  p[CY] = a[CY] + v[CY];
  p[CZ] = a[CZ] + v[CZ];
}

//subtract a vector from a point
INLINE void PointSubtractVector(Point a, Vector v, OUT Point p)
{
  p[CX] = a[CX] - v[CX];
  p[CY] = a[CY] - v[CY];
  p[CZ] = a[CZ] - v[CZ];
}

//subtract a point from another point
INLINE void PointSubtractPoint(Point a, Point b, OUT Vector v)
{
  v[CX] = a[CX] - b[CX];
  v[CY] = a[CY] - b[CY];
  v[CZ] = a[CZ] - b[CZ];
}

//calculate squared distance between two points
INLINE float PointDistanceSquared(Point a, Point b)
{
  Vector v;
  PointSubtractPoint(a, b, v);
  return VectorLengthSquared(v);
}

//calculate distance between two points
INLINE float PointDistance(Point a, Point b)
{
  return (float)sqrt(PointDistanceSquared(a, b));
}

//transform a point by a matrix
INLINE void PointTransform(Point a, Matrix m, OUT Point p)
{
  p[CX] = a[CX] * m[0] + a[CY] * m[4] + a[CZ] * m[8]  + m[12];
  p[CY] = a[CX] * m[1] + a[CY] * m[5] + a[CZ] * m[9]  + m[13];
  p[CZ] = a[CX] * m[2] + a[CY] * m[6] + a[CZ] * m[10] + m[14];
}

//transform a vector by a matrix
INLINE void VectorTransform(Vector a, Matrix m, OUT Vector v)
{
  v[CX] = a[CX] * m[0] + a[CY] * m[4] + a[CZ] * m[8];
  v[CY] = a[CX] * m[1] + a[CY] * m[5] + a[CZ] * m[9];
  v[CZ] = a[CX] * m[2] + a[CY] * m[6] + a[CZ] * m[10];
}

//transform a normal vector by a matrix
INLINE void NormalTransform(Vector a, Matrix m, OUT Vector n)
{
  n[CX] = a[CX] * m[0] + a[CY] * m[1] + a[CZ] * m[2];
  n[CY] = a[CX] * m[4] + a[CY] * m[5] + a[CZ] * m[6];
  n[CZ] = a[CX] * m[8] + a[CY] * m[9] + a[CZ] * m[10];
}

//initialize a plane using a point and a normal vector
//assume unit normal vector
INLINE void PlanePointNormal(OUT Plane p, Point a, Vector n)
{
  p[CX] = n[CX];
  p[CY] = n[CY];
  p[CZ] = n[CZ];
  p[CW] = -VectorDotProduct(a, n);
}

//initialize a plane using three points
INLINE void PlanePoints(OUT Plane p, Point a, Point b, Point c)
{
  Vector v, w, n;
  PointSubtractPoint(a, b, v);
  PointSubtractPoint(a, c, w);
  VectorCrossProduct(v, w, n);
  VectorNormalize(n, n);
  PlanePointNormal(p, a, n);
}

//calculate distance from a plane to a point
//assume plane with unit normal vector
INLINE float PlanePointDistance(Plane p, Point a)
{
  return VectorDotProduct(a, p) + p[CW];
}

//check if a point belongs to a plane
INLINE bool PlaneHasPoint(Plane p, Point a)
{
  return FloatIsZero(PlanePointDistance(p, a));
}

//create a translation matrix
INLINE void MatrixTranslation(OUT Matrix m, Vector d)
{
  m[0] = 1.0f;
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = 1.0f;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8]  = 0.0f;
  m[9]  = 0.0f;
  m[10] = 1.0f;
  m[11] = 0.0f;

  m[12] = d[CX];
  m[13] = d[CY];
  m[14] = d[CZ];
  m[15] = 1.0f;
}

//create a matrix for scaling along xyz-axes
INLINE void MatrixScaling(OUT Matrix m, Vector s)
{
  m[0] = s[CX];
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = s[CY];
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8]  = 0.0f;
  m[9]  = 0.0f;
  m[10] = s[CZ];
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

//create a matrix for scaling along an arbitrary axis
//assume unit axis vector
INLINE void MatrixScalingAxis(OUT Matrix m, Vector u, float s)
{
  float sone = s - 1.0f;
  float sxy = sone * u[CX] * u[CY];
  float syz = sone * u[CY] * u[CZ];
  float szx = sone * u[CZ] * u[CX];

  m[0] = 1.0f + sone * u[CX] * u[CX];
  m[1] = sxy;
  m[2] = szx;
  m[3] = 0.0f;

  m[4] = sxy;
  m[5] = 1.0f + sone * u[CY] * u[CY];
  m[6] = syz;
  m[7] = 0.0f;

  m[8]  = szx;
  m[9]  = syz;
  m[10] = 1.0f + sone * u[CZ] * u[CZ];
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

//create a matrix for rotation about x-axis
INLINE void MatrixRotationAxisX(OUT Matrix m, float deg)
{
  float c = FastCos(deg);
  float s = FastSin(deg);

  m[0] = 1.0f;
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = c;
  m[6] = s;
  m[7] = 0.0f;

  m[8]  = 0.0f;
  m[9]  = -s;
  m[10] = c;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

//create a matrix for rotation about y-axis
INLINE void MatrixRotationAxisY(OUT Matrix m, float deg)
{
  float c = FastCos(deg);
  float s = FastSin(deg);

  m[0] = c;
  m[1] = 0.0f;
  m[2] = -s;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = 1.0f;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8]  = s;
  m[9]  = 0.0f;
  m[10] = c;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

//create a matrix for rotation about z-axis
INLINE void MatrixRotationAxisZ(OUT Matrix m, float deg)
{
  float c = FastCos(deg);
  float s = FastSin(deg);

  m[0] = c;
  m[1] = s;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = -s;
  m[5] = c;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8]  = 0.0f;
  m[9]  = 0.0f;
  m[10] = 1.0f;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

//create a matrix for rotation about an arbitrary axis
//assume unit axis vector
INLINE void MatrixRotationAxis(OUT Matrix m, Vector u, float deg)
{
  float c = FastCos(deg);
  float s = FastSin(deg);
  float onec = 1.0f - c;

  float x = u[CX];
  float y = u[CY];
  float z = u[CZ];

  float xy = x * y;
  float yz = y * z;
  float zx = z * x;

  float xs = x * s;
  float ys = y * s;
  float zs = z * s;

  m[0] = x * x * onec + c;
  m[1] = xy    * onec + zs;
  m[2] = zx    * onec - ys;
  m[3] = 0.0f;

  m[4] = xy    * onec - zs;
  m[5] = y * y * onec + c;
  m[6] = yz    * onec + xs;
  m[7] = 0.0f;

  m[8]  = zx    * onec + ys;
  m[9]  = yz    * onec - xs;
  m[10] = z * z * onec + c;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

//create a rotation matrix using a quaternion
INLINE void MatrixRotationQuaternion(OUT Matrix m, Quaternion q)
{
  float x = q[CX];
  float y = q[CY];
  float z = q[CZ];
  float w = q[CW];

  float x2 = 2.0f * x;
  float y2 = 2.0f * y;
  float z2 = 2.0f * z;

  float xy2 = x2 * y;
  float yz2 = y2 * z;
  float zx2 = z2 * x;

  float wx2 = x2 * w;
  float wy2 = y2 * w;
  float wz2 = z2 * w;

  x2 *= x;
  y2 *= y;
  z2 *= z;

  m[0] = 1.0f - y2 -  z2;
  m[1] =       xy2 + wz2;
  m[2] =       zx2 - wy2;
  m[3] = 0.0f;

  m[4] =       xy2 - wz2;
  m[5] = 1.0f - x2 -  z2;
  m[6] =       yz2 + wx2;
  m[7] = 0.0f;

  m[8]  =       zx2 + wy2;
  m[9]  =       yz2 - wx2;
  m[10] = 1.0f - x2 -  y2;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

//create a trackball rotation matrix
INLINE void MatrixRotationTrackBall(OUT Matrix m, Point a, Point b,
                                    float w, float h, float s)
{
  //calculate trackball radius and scaling parameter
  s = 1.0f / ((w < h ? w : h) * s);

  //calculate coordinates of the center of the screen
  w *= 0.5f;
  h *= 0.5f;

  //vector from the center of the screen to the old mouse position
  Vector o = { (a[CX] - w) * s, (a[CY] - h) * s, 0.0f };

  //vector from the center of the screen to the new mouse position
  Vector n = { (b[CX] - w) * s, (b[CY] - h) * s, 0.0f };

  //caculate square of each mouse position z-coordinate
  w = 1.0f - o[CX] * o[CX] - o[CY] * o[CY];
  h = 1.0f - n[CX] * n[CX] - n[CY] * n[CY];

  //if z-coordinate < 0, mouse point falls outside of the trackball
  if (w < 0.0f)
  {
    s = 1.0f / (float)sqrt(1.0f - w);
    o[CX] *= s;
    o[CY] *= s;
  }
  else
  {
    o[CZ] = (float)sqrt(w);
  }
  if (h < 0.0f)
  {
    s = 1.0f / (float)sqrt(1.0f - h);
    n[CX] *= s;
    n[CY] *= s;
  }
  else
  {
    n[CZ] = (float)sqrt(h);
  }

  //generate an axis vector by taking cross product of old and new
  //mouse position vectors
  Vector axis;
  VectorCrossProduct(o, n, axis);

  //length of the axis vector is the angle of rotation in radians
  w = VectorLength(axis);

  if (FloatIsZero(w))
  {
    MatrixIdentity(m);
  }
  else
  {
    s = 1.0f / w;
    axis[CX] *= s;
    axis[CY] *= s;
    axis[CZ] *= s;

    MatrixRotationAxis(m, axis, RadToDeg(w));
  }
}

//create a shadow projection matrix
INLINE void MatrixShadow(OUT Matrix m, Plane p, Vector light)
{
  //find dot product of plane normal and light vector
  float dot = VectorDotProduct(p, light);

  m[0] = dot  - p[CX] * light[CX];
  m[1] = 0.0f - p[CX] * light[CY];
  m[2] = 0.0f - p[CX] * light[CZ];
  m[3] = 0.0f - p[CX] * light[CW];

  m[4] = 0.0f - p[CY] * light[CX];
  m[5] = dot  - p[CY] * light[CY];
  m[6] = 0.0f - p[CY] * light[CZ];
  m[7] = 0.0f - p[CY] * light[CW];

  m[8]  = 0.0f - p[CZ] * light[CX];
  m[9]  = 0.0f - p[CZ] * light[CY];
  m[10] = dot  - p[CZ] * light[CZ];
  m[11] = 0.0f - p[CZ] * light[CW];

  m[12] = 0.0f - p[CW] * light[CX];
  m[13] = 0.0f - p[CW] * light[CY];
  m[14] = 0.0f - p[CW] * light[CZ];
  m[15] = dot  - p[CW] * light[CW];
}

//create a reflection matrix
//assume plane with unit normal vector
INLINE void MatrixReflection(OUT Matrix m, Plane p)
{
  //pass plane as unit axis vector
  MatrixScalingAxis(m, p, -1.0f);
}

//create a quaternion for rotation about x-axis
INLINE void QuaternionRotationAxisX(OUT Quaternion q, float deg)
{
  deg *= 0.5f;
  q[CW] = FastCos(deg);
  q[CX] = FastSin(deg);
  q[CY] = 0.0f;
  q[CZ] = 0.0f;
}

//create a quaternion for rotation about y-axis
INLINE void QuaternionRotationAxisY(OUT Quaternion q, float deg)
{
  deg *= 0.5f;
  q[CW] = FastCos(deg);
  q[CX] = 0.0f;
  q[CY] = FastSin(deg);
  q[CZ] = 0.0f;
}

//create a quaternion for rotation about z-axis
INLINE void QuaternionRotationAxisZ(OUT Quaternion q, float deg)
{
  deg *= 0.5f;
  q[CW] = FastCos(deg);
  q[CX] = 0.0f;
  q[CY] = 0.0f;
  q[CZ] = FastSin(deg);
}

//create a quaternion for rotation about an arbitrary axis
//assume unit axis vector
INLINE void QuaternionRotationAxis(OUT Quaternion q, Vector v, float deg)
{
  deg *= 0.5f;
  float s = FastSin(deg);
  q[CW] = FastCos(deg);
  q[CX] = v[CX] * s;
  q[CY] = v[CY] * s;
  q[CZ] = v[CZ] * s;
}

///multiply a color by a coefficient vector
INLINE void ColorScale(Color a, Vector k, OUT Color c)
{
  c[CR] = a[CR] * k[CX];
  c[CG] = a[CG] * k[CY];
  c[CB] = a[CB] * k[CZ];
}

NAMESPACE_END

#endif //_ALGEBRA_H_
