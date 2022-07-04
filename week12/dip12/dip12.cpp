//(OpenCV4) g++ -std=c++11 dip12.cpp -framework OpenGL -framework GLUT
//`pkg-config --cflags --libs opencv4` -Wno-deprecated (OpenCV3) g++ dip07a.cpp
//-framework OpenGL -framework GLUT `pkg-config --cflags --libs opencv`
//-Wno-deprecated

#include <GLUT/glut.h>        //OpenGL関連ヘッダ
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

//関数名の宣言
void initCV(void);     // OpenCVの初期化
void initGL(void);     // OpenGLの初期化
void display(void);    // 描画関数
void timer(int value); //タイマー関数
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void extractEyeArea(const cv::Mat &frame, cv::Mat &eyeArea);

//グローバル変数
int mX, mY, mState, mButton; //マウス座標
int winW, winH;              // ウィンドウサイズ
double fr = 30.0;            //フレームレート
cv::VideoCapture capture;    // 映像キャプチャ用変数
cv::Mat originalImage, frameImage, hsvImage, tempImage;
cv::Size imageSize(720, 405);
cv::CascadeClassifier faceClassifier; // 顔認識用分類器
char *cameraTexture;                  // カメラ画像用配列
float fdata[] = {1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0,
                 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0,
                 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0,
                 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0,
                 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0};
cv::Mat kernel(cv::Size(5, 5), CV_32F, fdata);

int paddingX = 20;
int paddingY = 70;

// 顔色の変更
void change_face_color(cv::Mat &faceImage, cv::Mat &hsvImage, cv::Rect rect) {
  // 色解析しやすいようにHSV色空間に変換
  cv::cvtColor(faceImage, hsvImage, cv::COLOR_BGR2HSV);

  for (int j = rect.y - paddingY; j < rect.y + rect.height + paddingX; j++) {
    for (int i = rect.x - paddingX; i < rect.x + rect.width + paddingX; i++) {
      cv::Vec3b s = hsvImage.at<cv::Vec3b>(j, i);
      // 肌色領域のみ変換
      if (s[0] > 0 && s[0] < 45 && s[1] > 50 && s[1] < 255 && s[2] > 50 &&
          s[2] < 255) {
        // s[0] = 120;
        // hsvImage.at<cv::Vec3b>(j, i) = s;
        cv::Vec3b p = faceImage.at<cv::Vec3b>(j, i);
        p[0] = 255 - p[0];
        p[1] = 255 - p[1];
        p[2] = 255 - p[2];
        faceImage.at<cv::Vec3b>(j, i) = p;
      }
    }
  }
  cv::morphologyEx(hsvImage, hsvImage, cv::MORPH_CLOSE, kernel,
                   cv::Point(-1, -1), 1);
  // cv::cvtColor(faceImage, faceImage, cv::COLOR_HSV2BGR);
}

// main関数
int main(int argc, char *argv[]) {
  // OpenGL初期化
  glutInit(&argc, argv);

  // OpenCV初期設定処理
  initCV();

  // OpenGL初期設定処理
  initGL();

  //イベント待ち無限ループ
  glutMainLoop();

  return 0;
}

// OpenCV初期設定処理
void initCV() {
  //カメラキャプチャの初期化
  capture = cv::VideoCapture(0);
  if (capture.isOpened() == 0) {
    //カメラが見つからないときはメッセージを表示して終了
    printf("Camera not found\n");
    exit(1);
  }
  capture >> originalImage;
  cv::resize(originalImage, frameImage, imageSize);

  // OpenCVウィンドウ生成
  cv::namedWindow("Frame");

  // 3チャンネル画像"hsvImage"と"tempImage"の確保（ビデオと同サイズ）
  hsvImage = cv::Mat(imageSize, CV_8UC3);
  tempImage = cv::Mat(imageSize, CV_8UC3);

  // ①正面顔検出器の読み込み
  faceClassifier.load("haarcascades/haarcascade_frontalface_default.xml");
}

// OpenGL初期設定処理
void initGL(void) {
  //初期設定
  glutInitWindowSize(imageSize.width, imageSize.height); //ウィンドウサイズ指定
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH |
                      GLUT_DOUBLE); //ディスプレイモード設定

  // OpenGLウィンドウ生成
  glutCreateWindow("DIP12");

  //ウィンドウ消去色設定
  glClearColor(0.0, 0.0, 0.2, 1.0);

  //機能有効化
  glEnable(GL_LIGHTING);   //陰影付けの有効化
  glEnable(GL_LIGHT0);     //光源0の有効化
  glEnable(GL_DEPTH_TEST); //デプスバッファの有効化
  glEnable(GL_NORMALIZE);  //法線ベクトル正規化

  //コールバック関数
  glutDisplayFunc(display); //ディスプレイコールバック関数の指定
  glutReshapeFunc(reshape); //リサイズコールバック関数の指定
  glutMouseFunc(mouse); //マウスクリックコールバック関数の指定（"mouse()"）
  glutMotionFunc(motion); //マウスドラッグコールバック関数の指定（"motion()"）
  glutKeyboardFunc(keyboard); //キーボードコールバック関数の指定（"keyboard()"）
  glutTimerFunc(1000 / fr, timer, 0); //タイマーコールバック関数の指定

  // カメラ映像描画用配列
  cameraTexture =
      new char[imageSize.width * imageSize.height * 3]; //３バイトカラー画像
}

//ディスプレイコールバック関数
void display(void) {
  //------------------------------CV------------------------------
  //ビデオキャプチャから1フレーム画像取得
  capture >> originalImage;
  cv::resize(originalImage, frameImage, imageSize);

  //フレーム画像表示
  cv::imshow("Frame", frameImage);
  cv::Mat eyeImage = frameImage.clone();
  extractEyeArea(frameImage, eyeImage);
  cv::imshow("Eye", eyeImage);
  // ②検出情報を受け取るための配列を用意する
  std::vector<cv::Rect> faces;

  // ③画像中から検出対象の情報を取得する
  faceClassifier.detectMultiScale(frameImage, faces, 1.1, 3, 0,
                                  cv::Size(20, 20));

  // ④顔領域の検出
  for (int i = 0; i < faces.size(); i++) {
    // 検出情報から顔の位置情報を取得
    cv::Rect face = faces[i];
    // 大きさによるチェック。
    if (face.width * face.height < 100 * 100) {
      continue; // 小さい矩形は採用しない
    }

    // ⑤画像の加工
    change_face_color(frameImage, hsvImage, face);

    // 取得した顔の位置情報に基づき、矩形描画を行う
    cv::rectangle(frameImage, cv::Point(face.x - paddingX, face.y - paddingY),
                  cv::Point(face.x + face.width + paddingX,
                            face.y + face.height + paddingX),
                  CV_RGB(255, 0, 0), 3, cv::LINE_AA);
  }

  // ⑥カメラ画像を配列cameraTextureに複写（OpenCVとOpenGLでは色の並びが異なることに注意：OpenCV->BGR,
  // OpenGL->RGB）
  for (int j = 0; j < imageSize.height; j++) {
    for (int i = 0; i < imageSize.width; i++) {
      cv::Vec3b s = frameImage.at<cv::Vec3b>(j, i);
      cameraTexture[(j * imageSize.width + i) * 3 + 0] = s[2];
      cameraTexture[(j * imageSize.width + i) * 3 + 1] = s[1];
      cameraTexture[(j * imageSize.width + i) * 3 + 2] = s[0];
    }
  }

  //------------------------------CG------------------------------
  //ウィンドウ内消去
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //行列初期化
  glLoadIdentity();

  // ⑦カメラ画像をテクスチャマッピング
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imageSize.width, imageSize.height,
                    GL_RGB, GL_UNSIGNED_BYTE, cameraTexture);
  /*
   //cameraTexture用の配列を用意して
  frameImageの画素値を１画素ずつ代入する代わりに，下のように，frameImageのデータ格納配列の先頭アドレス
  frameImage.data
  を直接指定しても良い．ただし第５引数をGL_BGRに変えてあることに注意
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imageSize.width, imageSize.height,
  GL_BGR, GL_UNSIGNED_BYTE, frameImage.data);
   */

  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(-imageSize.width / 2, imageSize.height / 2);
  glTexCoord2f(0, 1);
  glVertex2f(-imageSize.width / 2, -imageSize.height / 2);
  glTexCoord2f(1, 1);
  glVertex2f(imageSize.width / 2, -imageSize.height / 2);
  glTexCoord2f(1, 0);
  glVertex2f(imageSize.width / 2, imageSize.height / 2);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);

  //描画実行
  glutSwapBuffers();
}

//タイマーコールバック関数
void timer(int value) {
  glutPostRedisplay(); //ディスプレイイベント強制発生
  glutTimerFunc(1000 / fr, timer, 0); //タイマー再設定
}

//リサイズコールバック関数
void reshape(int w, int h) {
  glViewport(0, 0, w, h);      //ウィンドウ全体が描画対象
  glMatrixMode(GL_PROJECTION); //投影変換行列を計算対象に設定
  glLoadIdentity();            //行列初期化
  gluOrtho2D(-w / 2, w / 2, -h / 2, h / 2); //直投影
  glMatrixMode(GL_MODELVIEW); //モデルビュー変換行列を計算対象に設定

  winW = w;
  winH = h;
}

//マウスクリックコールバック関数
void mouse(int button, int state, int x, int y) {
  //マウスボタンが押された
  if (state == GLUT_DOWN) {
    mX = x;
    mY = y;
    mState = state;
    mButton = button; //マウス情報保持
  }
}

//マウスドラッグコールバック関数
void motion(int x, int y) {
  //マウスの移動量を角度変化量に変換
  if (mButton == GLUT_LEFT_BUTTON) {
  } else if (mButton == GLUT_RIGHT_BUTTON) {
  }

  mX = x;
  mY = y;
}

//キーボードコールバック関数(key:キーの種類，x,y:座標)
void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 'q':
  case 'Q':
  case '\033': // '\033'はESCのASCIIコード
    exit(0);
  default:
    break;
  }
}

// Extract the area around the eyes and fill it with black.
void extractEyeArea(const cv::Mat &frame, cv::Mat &eyeArea) {
  // Find the eyes.
  std::vector<cv::Rect> eyes;
  cv::CascadeClassifier eyeCascade;
  eyeCascade.load("haarcascades/haarcascade_eye_tree_eyeglasses.xml");
  eyeCascade.detectMultiScale(frame, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE,
                              cv::Size(30, 30));
  if (eyes.size() != 2) {
    std::cout << "Could not find both eyes." << std::endl;
    eyeArea = frame;
    return;
  }
  // Find the center of the eyes.
  cv::Point center(0, 0);
  for (int i = 0; i < 2; ++i) {
    center += eyes[i].tl();
    center += eyes[i].br();
  }
  center /= 4;
  std::cout << "center: " << center.x << "," << center.y << std::endl;
  // Extract the area around the eyes.
  cv::Rect eyeRect(center.x - 20, center.y - 20, 40, 40);
  // eyeArea = frame(eyeRect);
  // Fill the area with black.

  cv::Vec3b s;
  for (int i = 0; i < frame.rows; i++) {
    for (int j = 0; j < frame.cols; j++) {
      s = frame.at<cv::Vec3b>(i, j);
      if ((i > center.y - 20 && i < center.y + 20) &&
          (j > center.x - 150 && j < center.x + 150)) {
        s = cv::Vec3b(0, 0, 0);
      }
      eyeArea.at<cv::Vec3b>(i, j) = s;
    }
  }
}
