#include <cstdio>
#include <cstdlib>
#include <cmath>
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

static float pi = 3.1415926535;

/*
** 店の数
*/
#define POINTS 1000000

//転送用変数
static GLuint valueLocation;

/*
** 実行時間
*/
static int count=1;
static GLint elapsedTimeLocation;
#define CYCLE 5000.0f

/*
**トラックボール
*/
#include "Trackball.h"
static Trackball trackball;
static int pressedButton;

/*
** 変換行列
*/
#include "Matrix.h"
static Matrix projectionMatrix;
static Matrix modelviewMatrix;
static GLint transformMatrixLocation;

/*
** シェーダ
*/
#include "shadersource.h"
static const char vertSource[] = "simple.vert";
static const char fragSource[] = "simple.frag";
static GLuint gl2Program;

/*
** vbo確保
*/
static GLuint buffer[4];
static GLsizei points;
static GLint pointLocation;

/*
** 画面表示
*/
static void display(void)
{
  static int frame = 0;

  //投影変換　視野変換　モデリング変換
  Matrix transformMatrix = projectionMatrix * trackball.get();
  transformMatrix.scale(1.0f, 1.0f, 1.0f);
  transformMatrix.translate(0.0f, 0.0f, 0.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  //シェーダプログラム開始
  glUseProgram(gl2Program);

  //経過時間
  float elapsedTime = (float)glutGet(GLUT_ELAPSED_TIME) / CYCLE;
  
  //シェーダに転送
  glUniform1f(elapsedTimeLocation, elapsedTime);

  glUniformMatrix4fv(transformMatrixLocation, 1, GL_FALSE, transformMatrix.get());

  glEnableVertexAttribArray(pointLocation);

  //位置情報id用のvboを指定
  glBindBuffer(GL_ARRAY_BUFFER, buffer[frame]);

  glVertexAttribPointer(pointLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glEnable(GL_DEPTH_TEST);

  //保存先指定
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer[1 - frame]);

  glEnableVertexAttribArray(valueLocation);
  
  //速度情報&ライフ用のvboを指定
  glBindBuffer(GL_ARRAY_BUFFER, buffer[2 + frame]);
  
  glVertexAttribPointer(valueLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
  
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, buffer[3 - frame]);
  
  // Transform Feedback 開始
  glBeginTransformFeedback(GL_POINTS);

  glDrawArrays(GL_POINTS, 0, POINTS);

  glEndTransformFeedback();

  glDisable(GL_DEPTH_TEST);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(pointLocation);

  glUseProgram(0);

  // buffer入れ替え
  frame = 1 - frame;

  glutSwapBuffers();
}

static void display2(){
  glutSwapBuffers();
}

static void resize(int w, int h)
{
  int x,y=0,z=0;
  w<=h ? (x=w) : (x=h);//小さい方
  w<=h ? (y=h) : (z=w);//大きい方
  x==y ? (y=0) : (y=y);//
  x==z ? (z=0) : (z=z);
  glViewport(0, 0, x+z/4, x+y/4);

  //平行投影変換行列作成
  projectionMatrix.loadIdentity();
  projectionMatrix.orthogonal(-500.0,500.0,-100.0,500.0,-200.0,200.0);
  
  trackball.region(w, h);
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case '\033':
    case 'q':
    case 'Q':
      glDeleteBuffers(4,buffer);
      exit(0);
    case 's':
      glutDisplayFunc(display2);
    default:
      break;
  }
}

static void idle(void)
{
  glutPostRedisplay();
}

static void mouse(int button, int state, int x, int y)
{
  pressedButton = button;
  
  switch (pressedButton) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        trackball.start(x, y);
      }
      else {
        trackball.stop(x, y);
      }
      break;
    default:
      break;
  }
}

static void motion(int x, int y)
{
  switch (pressedButton) {
    case GLUT_LEFT_BUTTON:
      trackball.motion(x, y);
      break;
    default:
      break;
  }
}

static void init(void)
{
  GLint compiled, linked;
  
  GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  
  //シェーダ読み込み
  if (readShaderSource(vertShader, vertSource)) exit(1);
  if (readShaderSource(fragShader, fragSource)) exit(1);
  
  //コンパイル
  glCompileShader(vertShader);
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(vertShader);
  if (compiled == GL_FALSE) {
    fprintf(stderr, "Compile error in vertex shader.\n");
    exit(1);
  }
  
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(fragShader);
  if (compiled == GL_FALSE) {
    fprintf(stderr, "Compile error in fragment shader.\n");
    exit(1);
  }
  
  gl2Program = glCreateProgram();
  
  //シェーダオブジェクをプログラムに登録
  glAttachShader(gl2Program, vertShader);
  glAttachShader(gl2Program, fragShader);
  
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);

  // feedback用のvarying登録
  const static char *varyings[] = { "position","next" };
  glTransformFeedbackVaryings(gl2Program, sizeof varyings / sizeof varyings[0], varyings, GL_SEPARATE_ATTRIBS);  
  
  //リンク
  glLinkProgram(gl2Program);
  glGetProgramiv(gl2Program, GL_LINK_STATUS, &linked);
  printProgramInfoLog(gl2Program);
  if (linked == GL_FALSE) {
    fprintf(stderr, "Link error.\n");
    exit(1);
  }

  elapsedTimeLocation = glGetUniformLocation(gl2Program, "elapsedTime");
  
  transformMatrixLocation = glGetUniformLocation(gl2Program, "transformMatrix");
  
  pointLocation = glGetAttribLocation(gl2Program, "point");

  valueLocation = glGetAttribLocation(gl2Program, "value");
  
  //視野変換行列を求める
  modelviewMatrix.loadIdentity();
  modelviewMatrix.lookat(300.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  
  //位置情報&id用のvboセットの初期設定
  glGenBuffers(sizeof(buffer)/sizeof(buffer[0]), buffer);
  
  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat[4]) * POINTS, 0, GL_STATIC_DRAW);
  
  GLfloat (*point)[4] = (GLfloat (*)[4])glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  
  for (int i = 0; i < POINTS; ++i) {
    float r = (float)rand() / (float)RAND_MAX*70;
    float theta = pi * 2* (float)rand() / (float)RAND_MAX;
    float phi = pi *  (float)rand() / (float)RAND_MAX;

    point[i][0] = r * sin(theta) * cos(phi);//x
    point[i][1] = -r * sin(phi);//y
    point[i][2] = r * cos(theta)* cos(phi);//z

    int g = i/(POINTS/10);//0...9
    point[i][3] = (float)g;//id
  }

  glUnmapBuffer(GL_ARRAY_BUFFER);

  glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat[4]) * POINTS, 0, GL_DYNAMIC_COPY);
  
  //速度情報&ライフ用のvboの初期設定
  glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat[4]) * POINTS, 0, GL_STATIC_DRAW);
  
  GLfloat (*value)[4] = (GLfloat (*)[4])glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  
   for (int i = 0; i < POINTS; ++i) {
    //速度
    value[i][0] = 0.0f;
    value[i][1] = 0.0f;
    value[i][2] = 0.0f;

    //ライフ
    value[i][3] = 1.0;

  }
  
  glUnmapBuffer(GL_ARRAY_BUFFER);
  
  glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat[4]) * POINTS, 0, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  trackball.initialize();

  glutGet(GLUT_ELAPSED_TIME);
  
  glClearColor(0.0, 0.0, 0.0, 1.0);
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutIdleFunc(idle);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  init();
  glutMainLoop();

  return 0;
}
