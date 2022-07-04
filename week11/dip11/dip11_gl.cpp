// g++ dip11_gl.cpp -framework OpenGL -framework GLUT -Wno-deprecated
#include <GLUT/glut.h> //OpenGL
#include <iostream>    //入出力関連ヘッダ
#include <math.h>      //数学関数
#include <opencv2/opencv.hpp>

//関数名の宣言
void initGL();
void display();
void reshape(int w, int h);
void timer(int value);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void initCV();
void getVPos(cv::Mat img);

//グローバル変数
double eDist, eDegX, eDegY;        //視点極座標
int mX, mY, mState, mButton;       //マウス座標
int winW, winH;                    //ウィンドウサイズ
double fr = 30.0;                  //フレームレート
cv::VideoCapture capture;          //ビデオキャプチャ
cv::Size imageSize;                //画像サイズ用
cv::Mat originalImage, frameImage; //画像格納用
cv::Point2f vPos;

// main関数
int main(int argc, char *argv[]) {
  // OpenGL初期化
  glutInit(&argc, argv);

  initCV();

  // OpenGL初期設定処理
  initGL();

  //イベント待ち無限ループ
  glutMainLoop();

  return 0;
}

void initCV() {
  capture = cv::VideoCapture(0);
  if (capture.isOpened() == 0) {
    printf("Capture not found\n");
    exit(0);
  }
  int imageWidth = 720, imageHeight = 405;
  imageSize = cv::Size(imageWidth, imageHeight);
  frameImage = cv::Mat(imageSize, CV_8UC3);

  cv::namedWindow("Frame");
  cv::moveWindow("Frame", 0, 0);
}

// OpenGL初期設定処理
void initGL() {
  //初期設定
  glutInitWindowSize(800, 600); //ウィンドウサイズ指定
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH |
                      GLUT_DOUBLE); //ディスプレイモード設定

  // OpenGLウィンドウ生成
  glutCreateWindow("GL");
  glutInitWindowPosition(0, 0);

  //ウィンドウ消去色設定
  glClearColor(0.0, 0.0, 0.2, 1.0);

  //機能有効化
  glEnable(GL_DEPTH_TEST); //デプスバッファ
  glEnable(GL_NORMALIZE);  //法線ベクトル正規化
  glEnable(GL_LIGHTING);   //陰影付け

  //光原設定
  GLfloat col[4];      //パラメータ(RGBA)
  glEnable(GL_LIGHT0); //光源0
  col[0] = 0.9;
  col[1] = 0.9;
  col[2] = 0.9;
  col[3] = 1.0;
  glLightfv(GL_LIGHT0, GL_DIFFUSE, col);  //光源0の拡散反射の強度
  glLightfv(GL_LIGHT0, GL_SPECULAR, col); //光源0の鏡面反射の強度
  col[0] = 0.05;
  col[1] = 0.05;
  col[2] = 0.05;
  col[3] = 1.0;
  glLightfv(GL_LIGHT0, GL_AMBIENT, col); //光源0の環境光の強度

  //コールバック関数
  glutDisplayFunc(display); //ディスプレイコールバック関数の指定
  glutReshapeFunc(reshape); //リシェイプコールバック関数の指定
  glutMouseFunc(mouse); //マウスクリックコールバック関数の指定
  glutMotionFunc(motion); //マウスドラッグコールバック関数の指定
  glutKeyboardFunc(keyboard); //キーボードコールバック関数の指定
  glutTimerFunc(1000 / fr, timer, 0); //タイマーコールバック関数の指定

  //視点極座標初期値
  eDist = 5000;
  eDegX = 0.0;
  eDegY = 0.0;
}

//ディスプレイコールバック関数
void display() {
  double theta;
  double r, g, b;

  capture >> originalImage;
  if (originalImage.data == NULL) {
    exit(0);
  }

  cv::resize(originalImage, frameImage, cv::Size(800, 600));
  cv::Mat scaledImage;

  cv::Mat grayImage;
  cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);
  getVPos(grayImage);

  theta = grayImage.at<uchar>(grayImage.rows / 2, grayImage.cols);

  // int bin[24][32] = {{0}};
  // for (int i = 0; i < 24; i++) {
  //   for (int j = 0; j < 32; j++) {
  //     uchar t = tinyImage.at<uchar>(i, j);
  //     if (t <= 51) {
  //       bin[i][j] = 0;
  //     } else if (t <= 102) {
  //       bin[i][j] = 1;
  //     } else if (t <= 153) {
  //       bin[i][j] = 2;
  //     } else if (t <= 204) {
  //       bin[i][j] = 3;
  //     } else {
  //       bin[i][j] = 4;
  //     }
  //   }
  // }

  cv::imshow("Frame", frameImage);

  GLfloat col[4]; //色設定用

  //ウィンドウ内消去
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //行列初期化
  glLoadIdentity();

  //視点座標の計算
  double ex = eDist * cos(eDegX * M_PI / 180.0) * sin(eDegY * M_PI / 180.0);
  double ey = eDist * sin(eDegX * M_PI / 180.0);
  double ez = eDist * cos(eDegX * M_PI / 180.0) * cos(eDegY * M_PI / 180.0);

  //視点視線の設定
  gluLookAt(ex, ey, ez, 0.0, 0.0, 0.0, 0.0, 1.0,
            0.0); //変換行列に視野変換行列を乗算

  //光源0の位置指定
  GLfloat pos0[] = {vPos.x, 600 - vPos.y, 50.0,
                    1}; //(x, y, z, 0(平行光源)/1(点光源))
  glLightfv(GL_LIGHT0, GL_POSITION, pos0);

  col[0] = 248.0 / 255.0;
  col[1] = 169.0 / 255.0;
  col[2] = 0.0;
  glMaterialfv(GL_FRONT, GL_DIFFUSE, col);
  glMaterialfv(GL_FRONT, GL_AMBIENT, col);
  col[0] = 0.3;
  col[1] = 0.3;
  col[2] = 0.3;
  col[3] = 1.0;
  glMaterialfv(GL_FRONT, GL_SPECULAR, col);
  glMaterialf(GL_FRONT, GL_SHININESS, 64);

  for (int i = 0; i < 24; i++) {
    for (int j = 0; j < 32; j++) {
      glPushMatrix();
      glTranslated(-1550.0 + j * 100.0, 1150.0 - i * 100.0, 0.0);
      // glRotated(30 - 20 * bin[i][j], 1.0, 0.0, 0.0);
      glScaled(95.0, 95.0, 20.0);
      glutSolidCube(1.0);
      glPopMatrix();
    }
  }

  //描画実行
  glutSwapBuffers();
}

//タイマーコールバック関数
void timer(int value) {
  glutPostRedisplay(); //ディスプレイイベント強制発生
  glutTimerFunc(1000 / fr, timer, 0); //タイマー再設定
}

//リシェイプコールバック関数
void reshape(int w, int h) {
  glViewport(0, 0, w, h);      //ウィンドウ全体が描画対象
  glMatrixMode(GL_PROJECTION); //投影変換行列を計算対象に設定
  glLoadIdentity();            //行列初期化
  gluPerspective(30.0, (double)w / (double)h, 1.0,
                 10000.0); //変換行列に透視投影を乗算
  glMatrixMode(GL_MODELVIEW); //モデルビュー変換行列を計算対象に設定
}

//マウスクリックコールバック関数
void mouse(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    //マウス情報格納
    mX = x;
    mY = y;
    mState = state;
    mButton = button;
  }
}

//マウスドラッグコールバック関数
void motion(int x, int y) {
  if (mButton == GLUT_RIGHT_BUTTON) {
    //マウスの移動量を角度変化量に変換
    eDegY = eDegY + (mX - x) * 0.5; //マウス横方向→水平角
    eDegX = eDegX + (y - mY) * 0.5; //マウス縦方向→垂直角
  }

  //マウス座標格納
  mX = x;
  mY = y;
}

//キーボードコールバック関数(key:キーの種類，x,y:座標)
void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 'q':
  case 'Q':
  case 27:
    exit(0);
  }
}

void getVPos(cv::Mat img) {
  cv::Mat binaryImage;
  uchar s;
  cv::threshold(img, binaryImage, 254, 255, cv::THRESH_BINARY);
  erode(binaryImage, binaryImage, cv::noArray(), cv::Point(-1, -1), 3);
  for (int i = 0; i < binaryImage.rows; i++) {
    for (int j = 0; j < binaryImage.cols; j++) {
      s = binaryImage.at<uchar>(i, j);
      if (s == 255) {
        vPos = cv::Point2f(j, i);
      }
    }
  }
  // cv::imshow("hoge", binaryImage);
}
