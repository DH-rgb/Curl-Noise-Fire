#ifndef TRACKBALL_H
#define TRACKBALL_H

class Trackball {
  int cx, cy;                   // ドラッグ位置
  float sx, sy;                 // マウスの位置
  float cq[4];                  // 回転の初期値
  float tq[4];                  // ドラッグ中の回転
  float array[16];              // 回転の変換行列
  bool drag;                    
public:
  Trackball() {};
  virtual ~Trackball() {};
  void initialize();            // 初期化
  void region(int w, int h);    // トラックボール処理の範囲
  void start(int x, int y);     
  void motion(int x, int y);    // 回転の変換行列計算
  void stop(int x, int y);      
  const float *get() const { return array; } // 行列取り出し
};

#endif
