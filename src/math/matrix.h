#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "common.h"

NAMESPACE_BEGIN

enum { DIM_MATRIX = 16 };
typedef float Matrix [DIM_MATRIX];

//common matrices
#define MATRIX_ZERO       {0.0f, 0.0f, 0.0f, 0.0f,\
                           0.0f, 0.0f, 0.0f, 0.0f,\
                           0.0f, 0.0f, 0.0f, 0.0f,\
                           0.0f, 0.0f, 0.0f, 0.0f}
#define MATRIX_IDENTITY   {1.0f, 0.0f, 0.0f, 0.0f,\
                           0.0f, 1.0f, 0.0f, 0.0f,\
                           0.0f, 0.0f, 1.0f, 0.0f,\
                           0.0f, 0.0f, 0.0f, 1.0f}

//macros to create an unnamed union matrix type inside a named type
//useful for index-free access to elements
#define MatrixUnionPrefix(name, prefix) \
union\
{\
  Matrix name;\
  struct\
  {\
    float CONCAT(prefix, 00);\
    float CONCAT(prefix, 10);\
    float CONCAT(prefix, 20);\
    float CONCAT(prefix, 30);\
    float CONCAT(prefix, 01);\
    float CONCAT(prefix, 11);\
    float CONCAT(prefix, 21);\
    float CONCAT(prefix, 31);\
    float CONCAT(prefix, 02);\
    float CONCAT(prefix, 12);\
    float CONCAT(prefix, 22);\
    float CONCAT(prefix, 32);\
    float CONCAT(prefix, 03);\
    float CONCAT(prefix, 13);\
    float CONCAT(prefix, 23);\
    float CONCAT(prefix, 33);\
  };\
}
#define MatrixUnion(name) MatrixUnionPrefix(name, name)

//zero-initialize a matrix
INLINE void MatrixZero(OUT Matrix m)
{
  MemZero(m, sizeof(Matrix));
}

//copy a matrix
INLINE void MatrixCopy(OUT Matrix m, Matrix a)
{
  MemCopy(m,  a, sizeof(Matrix));
}

//initialize a matrix
INLINE void MatrixInit(OUT Matrix m,
                       float m00, float m01, float m02, float m03,
                       float m10, float m11, float m12, float m13,
                       float m20, float m21, float m22, float m23,
                       float m30, float m31, float m32, float m33)
{
  m[0] = m00;
  m[1] = m10;
  m[2] = m20;
  m[3] = m30;

  m[4] = m01;
  m[5] = m11;
  m[6] = m21;
  m[7] = m31;

  m[8]  = m02;
  m[9]  = m12;
  m[10] = m22;
  m[11] = m32;

  m[12] = m03;
  m[13] = m13;
  m[14] = m23;
  m[15] = m33;
}

//create an identity matrix
INLINE void MatrixIdentity(OUT Matrix m)
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

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

//create a translation matrix
INLINE void MatrixTranslation(OUT Matrix m, float dx, float dy, float dz)
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

  m[12] = dx;
  m[13] = dy;
  m[14] = dz;
  m[15] = 1.0f;
}

//create a scaling matrix
INLINE void MatrixScaling(OUT Matrix m, float sx, float sy, float sz)
{
  m[0] = sx;
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = sy;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8]  = 0.0f;
  m[9]  = 0.0f;
  m[10] = sz;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

//multiply a matrix by a scalar
INLINE void MatrixScale(Matrix a, float s, OUT Matrix m)
{
  m[0] = a[0] * s;
  m[1] = a[1] * s;
  m[2] = a[2] * s;
  m[3] = a[3] * s;

  m[4] = a[4] * s;
  m[5] = a[5] * s;
  m[6] = a[6] * s;
  m[7] = a[7] * s;

  m[8]  = a[8]  * s;
  m[9]  = a[9]  * s;
  m[10] = a[10] * s;
  m[11] = a[11] * s;

  m[12] = a[12] * s;
  m[13] = a[13] * s;
  m[14] = a[14] * s;
  m[15] = a[15] * s;
}

//add a matrix to another matrix
INLINE void MatrixAdd(Matrix a, Matrix b, OUT Matrix m)
{
  m[0] = a[0] + b[0];
  m[1] = a[1] + b[1];
  m[2] = a[2] + b[2];
  m[3] = a[3] + b[3];

  m[4] = a[4] + b[4];
  m[5] = a[5] + b[5];
  m[6] = a[6] + b[6];
  m[7] = a[7] + b[7];

  m[8]  = a[8]  + b[8];
  m[9]  = a[9]  + b[9];
  m[10] = a[10] + b[10];
  m[11] = a[11] + b[11];

  m[12] = a[12] + b[12];
  m[13] = a[13] + b[13];
  m[14] = a[14] + b[14];
  m[15] = a[15] + b[15];
}

//subtract a matrix from another matrix
INLINE void MatrixSubtract(Matrix a, Matrix b, OUT Matrix m)
{
  m[0] = a[0] - b[0];
  m[1] = a[1] - b[1];
  m[2] = a[2] - b[2];
  m[3] = a[3] - b[3];

  m[4] = a[4] - b[4];
  m[5] = a[5] - b[5];
  m[6] = a[6] - b[6];
  m[7] = a[7] - b[7];

  m[8]  = a[8]  - b[8];
  m[9]  = a[9]  - b[9];
  m[10] = a[10] - b[10];
  m[11] = a[11] - b[11];

  m[12] = a[12] - b[12];
  m[13] = a[13] - b[13];
  m[14] = a[14] - b[14];
  m[15] = a[15] - b[15];
}

//multiply a matrix by another matrix
INLINE void MatrixMultiply(Matrix a, Matrix b, OUT Matrix m)
{
  m[0] = a[0] * b[0] + a[4] * b[1] + a[8]  * b[2] + a[12] * b[3];
  m[1] = a[1] * b[0] + a[5] * b[1] + a[9]  * b[2] + a[13] * b[3];
  m[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
  m[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];

  m[4] = a[0] * b[4] + a[4] * b[5] + a[8]  * b[6] + a[12] * b[7];
  m[5] = a[1] * b[4] + a[5] * b[5] + a[9]  * b[6] + a[13] * b[7];
  m[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
  m[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];  

  m[8]  = a[0] * b[8] + a[4] * b[9] + a[8]  * b[10] + a[12] * b[11];
  m[9]  = a[1] * b[8] + a[5] * b[9] + a[9]  * b[10] + a[13] * b[11];
  m[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
  m[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];

  m[12] = a[0] * b[12] + a[4] * b[13] + a[8]  * b[14] + a[12] * b[15];
  m[13] = a[1] * b[12] + a[5] * b[13] + a[9]  * b[14] + a[13] * b[15];
  m[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
  m[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
}

//transpose a matrix
INLINE void MatrixTranspose(Matrix a, OUT Matrix m)
{
  m[0] = a[0];
  m[1] = a[4];
  m[2] = a[8];
  m[3] = a[12];

  m[4] = a[1];
  m[5] = a[5];
  m[6] = a[9];
  m[7] = a[13];

  m[8]  = a[2];
  m[9]  = a[6];
  m[10] = a[10];
  m[11] = a[14];

  m[12] = a[3];
  m[13] = a[7];
  m[14] = a[11];
  m[15] = a[15];
}

/*
//multiply a matrix row by a scalar
INLINE void MatrixRowScale(Matrix a, uint r, float s, OUT Matrix m)
{
  //todo: implement
}

//subtract a matrix row multiple from another matrix row
INLINE void MatrixRowScaleSubtract(Matrix a, uint dst, uint src, float s,
                                   OUT Matrix m)
{
  //todo: implement
}

//calculate an inverse of a matrix
INLINE void MatrixInverse(Matrix a, OUT Matrix m)
{
  //todo: implement
}
*/

//get value of a matrix element
INLINE float MatrixGetElement(Matrix a, uint r, uint c)
{
  //a[column * 4 + row] - column-first matrix order
  return a[(c << 2) + r];
}

//set matrix element to a value
INLINE void MatrixSetElement(OUT Matrix a, uint r, uint c, float e)
{
  //a[column * 4 + row] - column-first matrix order
  a[(c << 2) + r] = e;
}

NAMESPACE_END

#endif //_MATRIX_H_
