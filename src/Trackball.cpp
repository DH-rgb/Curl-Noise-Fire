#include <cmath>

#include "Trackball.h"

/*
** r <- p x q
*/
static void qmul(float r[], const float p[], const float q[])
{
  r[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
  r[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
  r[2] = p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1];
  r[3] = p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0];
}

/*
** 回転変換行列　r <-クォータニオン q
*/
static void qrot(float r[], float q[])
{
  float x2 = q[1] * q[1] * 2.0f;
  float y2 = q[2] * q[2] * 2.0f;
  float z2 = q[3] * q[3] * 2.0f;
  float xy = q[1] * q[2] * 2.0f;
  float yz = q[2] * q[3] * 2.0f;
  float zx = q[3] * q[1] * 2.0f;
  float xw = q[1] * q[0] * 2.0f;
  float yw = q[2] * q[0] * 2.0f;
  float zw = q[3] * q[0] * 2.0f;
  
  r[ 0] = 1.0f - y2 - z2;
  r[ 1] = xy + zw;
  r[ 2] = zx - yw;
  r[ 4] = xy - zw;
  r[ 5] = 1.0f - z2 - x2;
  r[ 6] = yz + xw;
  r[ 8] = zx + yw;
  r[ 9] = yz - xw;
  r[10] = 1.0f - x2 - y2;
  r[ 3] = r[ 7] = r[11] = r[12] = r[13] = r[14] = 0.0f;
  r[15] = 1.0f;
}

/*
トラックボール処理の初期化
*/
void Trackball::initialize()
{
  drag = false;

  /* 単位クォータニオン */
  cq[0] = 1.0f;
  cq[1] = cq[2] = cq[3] = 0.0f;
  
  /* 回転行列の初期化 */
  array[ 1] = array[ 2] = array[ 3] = array[ 4] =
  array[ 6] = array[ 7] = array[ 8] = array[ 9] =
  array[11] = array[12] = array[13] = array[14] = 0.0f;
  array[ 0] = array[ 5] = array[10] = array[15] = 1.0f;
}

/*
トラックボールする領域
*/
void Trackball::region(int w, int h)
{
  sx = 1.0f / (float)w;
  sy = 1.0f / (float)h;
}

void Trackball::start(int x, int y)
{
  drag = true;

  cx = x;
  cy = y;
}

void Trackball::motion(int x, int y)
{
  if (drag) {
    float dx, dy, a;
    
    /* ポインタ位置からの変位 */
    dx = (x - cx) * sx;
    dy = (y - cy) * sy;
    
    a = sqrt(dx * dx + dy * dy);
    
    if (a != 0.0f) {
      float ar = a * 3.141593f;
      float as = sin(ar) / a;
      float dq[4] = { cos(ar), dy * as, dx * as, 0.0f };
      
      /* クォータニオンをかけて回転を合成 */
      qmul(tq, dq, cq);
      
      /* クォータニオンから回転の変換行列を求める */
      qrot(array, tq);
    }
  }
}

void Trackball::stop(int x, int y)
{
  /* ドラッグ終了点における回転を求める */
  motion(x, y);

  /* 保存 */
  cq[0] = tq[0];
  cq[1] = tq[1];
  cq[2] = tq[2];
  cq[3] = tq[3];

  drag = false;
}
