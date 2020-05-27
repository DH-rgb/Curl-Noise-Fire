#include <cmath>

#include "Matrix.h"

/*
** 行列とベクトルの積
*/
void Matrix::projection(float *c, const float *a, const float *b)
{
  for (int i = 0; i < 4; ++i) {
    c[i] = a[ 0 + i] * b[0]
         + a[ 4 + i] * b[1]
         + a[ 8 + i] * b[2]
         + a[12 + i] * b[3];
  }
}

/*
** 行列の積
*/
void Matrix::multiply(float *c, const float *a, const float *b)
{
  for (int i = 0; i < 16; ++i) {
    int j = i & ~3, k = i & 3;

    c[i] = a[ 0 + k] * b[j + 0]
         + a[ 4 + k] * b[j + 1]
         + a[ 8 + k] * b[j + 2]
         + a[12 + k] * b[j + 3];
  }
}

/*
** 単位行列
*/
Matrix &Matrix::loadIdentity()
{
  array[ 1] = array[ 2] = array[ 3] = array[ 4] =
  array[ 6] = array[ 7] = array[ 8] = array[ 9] =
  array[11] = array[12] = array[13] = array[14] = 0.0f;
  array[ 0] = array[ 5] = array[10] = array[15] = 1.0f;

  return *this;
}

/*
** 平行移動
*/
Matrix &Matrix::translate(float x, float y, float z, float w)
{
  if (w != 0.0f) {
    x /= w;
    y /= w;
    z /= w;
  }
  array[12] += array[ 0] * x + array[ 4] * y + array[ 8] * z;
  array[13] += array[ 1] * x + array[ 5] * y + array[ 9] * z;
  array[14] += array[ 2] * x + array[ 6] * y + array[10] * z;
  array[15] += array[ 3] * x + array[ 7] * y + array[11] * z;

  return *this;
}

/*
** 拡大縮小
*/
Matrix &Matrix::scale(float x, float y, float z, float w)
{
  array[ 0] *= x;
  array[ 1] *= x;
  array[ 2] *= x;
  array[ 3] *= x;
  array[ 4] *= y;
  array[ 5] *= y;
  array[ 6] *= y;
  array[ 7] *= y;
  array[ 8] *= z;
  array[ 9] *= z;
  array[10] *= z;
  array[11] *= z;
  array[12] *= w;
  array[13] *= w;
  array[14] *= w;
  array[15] *= w;

  return *this;
}

/*
** 視点移動
*/
Matrix &Matrix::lookat(float ex, float ey, float ez, float tx, float ty, float tz, float ux, float uy, float uz)
{
  float t[16], l;

  tx = ex - tx;
  ty = ey - ty;
  tz = ez - tz;
  l = sqrt(tx * tx + ty * ty + tz * tz);
  if (l == 0.0f) return *this;
  t[ 2] = tx / l;
  t[ 6] = ty / l;
  t[10] = tz / l;

  tx = uy * t[10] - uz * t[ 6];
  ty = uz * t[ 2] - ux * t[10];
  tz = ux * t[ 6] - uy * t[ 2];
	l = sqrt(tx * tx + ty * ty + tz * tz);
  if (l == 0.0f) return *this;
  t[ 0] = tx / l;
  t[ 4] = ty / l;
  t[ 8] = tz / l;

  t[ 1] = t[ 6] * t[ 8] - t[10] * t[ 4];
  t[ 5] = t[10] * t[ 0] - t[ 2] * t[ 8];
  t[ 9] = t[ 2] * t[ 4] - t[ 6] * t[ 0];

  t[12] = -(ex * t[ 0] + ey * t[ 4] + ez * t[ 8]);
  t[13] = -(ex * t[ 1] + ey * t[ 5] + ez * t[ 9]);
  t[14] = -(ex * t[ 2] + ey * t[ 6] + ez * t[10]);

  t[ 3] = t[ 7] = t[11] = 0.0f;
  t[15] = 1.0f;

  this->multiply(t);

  return *this;
}

/*
** 平行投影
*/
Matrix &Matrix::orthogonal(float left, float right, float bottom, float top, float near, float far)
{
  float dx = right - left;
  float dy = top - bottom;
  float dz = far - near;
  
  if (dx != 0.0f && dy != 0.0f && dz != 0.0f) {
    float t[16];
    
    t[ 0] =  2.0f / dx;
    t[ 5] =  2.0f / dy;
    t[10] = -2.0f / dz;
    t[12] = -(right + left) / dx;
    t[13] = -(top + bottom) / dy;
    t[14] = -(far + near) / dz;
    t[15] =  1.0f;
    t[ 1] = t[ 2] = t[ 3] = t[ 4] =
    t[ 6] = t[ 7] = t[ 8] = t[ 9] = t[11] = 0.0f;
    
    this->multiply(t);
  }
  
  return *this;
}