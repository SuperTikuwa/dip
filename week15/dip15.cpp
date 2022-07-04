//(OpenCV4) g++ -std=c++11 dip15.cpp -framework OpenGL -framework GLUT
//`pkg-config --cflags --libs opencv4` -Wno-deprecated (OpenCV3) g++ dip15.cpp
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

//グローバル変数
double eDist, eDegX, eDegY;  //視点極座標
int mX, mY, mState, mButton; //マウス座標
int winW, winH;              //ウィンドウサイズ
double fr = 30.0;            //フレームレート
cv::VideoCapture capture;    // 映像キャプチャ用変数
cv::Mat originalImage, frameImage, hsvImage, tempImage;
cv::Size imageSize(640, 360);
cv::CascadeClassifier faceClassifier, leyeClassifier, reyeClassifier,
    mouthClassifier, noseClassifier; // 顔認識用分類器

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

  //顔検出器の読み込み
  faceClassifier.load("haarcascades/haarcascade_frontalface_default.xml");
  leyeClassifier.load("haarcascades/haarcascade_lefteye_2splits.xml");
  reyeClassifier.load("haarcascades/haarcascade_righteye_2splits.xml");
  mouthClassifier.load("haarcascades/haarcascade_mcs_mouth.xml");
  noseClassifier.load("haarcascades/haarcascade_mcs_nose.xml");
}

// OpenGL初期設定処理
void initGL(void) {
  //初期設定
  glutInitWindowSize(imageSize.width, imageSize.height); //ウィンドウサイズ指定
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH |
                      GLUT_DOUBLE); //ディスプレイモード設定

  // OpenGLウィンドウ生成
  glutCreateWindow("CG");

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
  eDist = 1200;
  eDegX = 0.0;
  eDegY = 0.0;
}

//ディスプレイコールバック関数
void display(void) {
  //------------------------------CV------------------------------
  //ビデオキャプチャから1フレーム画像取得
  capture >> originalImage;
  cv::resize(originalImage, frameImage, imageSize);

  //検出情報を受け取るための配列を用意する
  std::vector<cv::Rect> faces, leyes, reyes, mouths, noses;

  //画像中から検出対象の情報を取得する
  faceClassifier.detectMultiScale(frameImage, faces, 1.1, 3, 0,
                                  cv::Size(20, 20)); //顔
  leyeClassifier.detectMultiScale(frameImage, leyes, 1.1, 3, 0,
                                  cv::Size(10, 10)); //左目
  reyeClassifier.detectMultiScale(frameImage, reyes, 1.1, 3, 0,
                                  cv::Size(10, 10)); //右目
  mouthClassifier.detectMultiScale(frameImage, mouths, 1.1, 3, 0,
                                   cv::Size(10, 10)); //口
  noseClassifier.detectMultiScale(frameImage, noses, 1.1, 3, 0,
                                  cv::Size(10, 10)); //鼻

  std::map<std::string, cv::Point> facePartsPoints;
  std::map<std::string, cv::Point> facePartsPointsCV;

  facePartsPoints["face"] = cv::Point(0, 0);
  facePartsPoints["left_eye"] = cv::Point(50, 50);
  facePartsPoints["right_eye"] = cv::Point(-50, 50);
  facePartsPoints["mouth"] = cv::Point(0, -50);
  facePartsPoints["nose"] = cv::Point(0, 0);

  //顔
  for (int i = 0; i < faces.size(); i++) {
    //検出情報から位置情報を取得
    cv::Rect face = faces[i];
    //大きさによるチェック。
    if (face.width * face.height < 100 * 100) {
      continue; //小さい矩形は採用しない
    }
    //取得した位置情報に基づき矩形描画
    cv::rectangle(frameImage, cv::Point(face.x, face.y),
                  cv::Point(face.x + face.width, face.y + face.height),
                  CV_RGB(255, 0, 0), 2, 8);

    facePartsPoints["face"] =
        cv::Point(((face.width / 2) + face.x) - (imageSize.width / 2),
                  -((face.height / 2) + face.y) + (imageSize.height / 2));
    facePartsPointsCV["face"] =
        cv::Point(((face.width / 2) + face.x), ((face.height / 2) + face.y));

    break;
  }

  //左目
  for (int i = 0; i < leyes.size(); i++) {
    //検出情報から位置情報を取得
    cv::Rect leye = leyes[i];
    //大きさによるチェック。
    if (leye.width * leye.height < 10 * 10) {
      continue; //小さい矩形は採用しない
    }

    facePartsPointsCV["left_eye"] =
        cv::Point(((leye.width / 2) + leye.x), ((leye.height / 2) + leye.y));

    if (facePartsPointsCV["left_eye"].y > facePartsPointsCV["face"].y) {
      continue;
    }

    facePartsPoints["left_eye"] =
        cv::Point(((leye.width / 2) + leye.x) - (imageSize.width / 2),
                  -((leye.height / 2) + leye.y) + (imageSize.height / 2));

    //取得した位置情報に基づき矩形描画
    cv::rectangle(frameImage, cv::Point(leye.x, leye.y),
                  cv::Point(leye.x + leye.width, leye.y + leye.height),
                  CV_RGB(0, 0, 255), 2, 8);
    break;
  }

  //右目
  for (int i = 0; i < reyes.size(); i++) {
    //検出情報から顔の位置情報を取得
    cv::Rect reye = reyes[i];
    //大きさによるチェック。
    if (reye.width * reye.height < 10 * 10) {
      continue; //小さい矩形は採用しない
    }

    facePartsPointsCV["right_eye"] =
        cv::Point(((reye.width / 2) + reye.x), ((reye.height / 2) + reye.y));

    if (facePartsPointsCV["right_eye"].y > facePartsPointsCV["face"].y ||
        facePartsPointsCV["left_eye"].x < facePartsPointsCV["right_eye"].x) {
      continue;
    }
    facePartsPoints["right_eye"] =
        cv::Point(((reye.width / 2) + reye.x) - (imageSize.width / 2),
                  -((reye.height / 2) + reye.y) + (imageSize.height / 2));
    // 取得した位置情報に基づき矩形描画
    cv::rectangle(frameImage, cv::Point(reye.x, reye.y),
                  cv::Point(reye.x + reye.width, reye.y + reye.height),
                  CV_RGB(0, 255, 255), 2, 8);
    break;
  }

  facePartsPoints["middle_eyes"] = cv::Point(
      (facePartsPoints["left_eye"].x + facePartsPoints["right_eye"].x) / 2,
      (facePartsPoints["left_eye"].y + facePartsPoints["right_eye"].y) / 2);
  facePartsPointsCV["middle_eyes"] = cv::Point(
      (facePartsPointsCV["left_eye"].x + facePartsPointsCV["right_eye"].x) / 2,
      (facePartsPointsCV["left_eye"].y + facePartsPointsCV["right_eye"].y) / 2);

  //口
  for (int i = 0; i < mouths.size(); i++) {
    //検出情報から顔の位置情報を取得
    cv::Rect mouth = mouths[i];
    //大きさによるチェック。
    if (mouth.width * mouth.height < 10 * 10) {
      continue; //小さい矩形は採用しない
    }

    facePartsPointsCV["mouth"] = cv::Point(((mouth.width / 2) + mouth.x),
                                           ((mouth.height / 2) + mouth.y));

    if (facePartsPointsCV["mouth"].y < facePartsPointsCV["face"].y ||
        facePartsPointsCV["mouth"].x < facePartsPointsCV["right_eye"].x ||
        facePartsPointsCV["mouth"].x > facePartsPointsCV["left_eye"].x) {
      continue;
    }

    facePartsPoints["mouth"] =
        cv::Point(((mouth.width / 2) + mouth.x) - (imageSize.width / 2),
                  -((mouth.height / 2) + mouth.y) + (imageSize.height / 2));

    //取得した位置情報に基づき矩形描画
    cv::rectangle(frameImage, cv::Point(mouth.x, mouth.y),
                  cv::Point(mouth.x + mouth.width, mouth.y + mouth.height),
                  CV_RGB(0, 255, 0), 1, 8);

    break;
  }

  //鼻
  for (int i = 0; i < noses.size(); i++) {
    //検出情報から顔の位置情報を取得
    cv::Rect nose = noses[i];
    //大きさによるチェック。
    if (nose.width * nose.height < 10 * 10) {
      continue; //小さい矩形は採用しない
    }

    facePartsPointsCV["nose"] =
        cv::Point(((nose.width / 2) + nose.x), ((nose.height / 2) + nose.y));

    if (facePartsPointsCV["nose"].y < facePartsPointsCV["middle_eyes"].y) {
      continue;
    }

    facePartsPoints["nose"] =
        cv::Point(((nose.width / 2) + nose.x) - (imageSize.width / 2),
                  -((nose.height / 2) + nose.y) + (imageSize.height / 2));
    //取得した位置情報に基づき矩形描画
    cv::rectangle(frameImage, cv::Point(nose.x, nose.y),
                  cv::Point(nose.x + nose.width, nose.y + nose.height),
                  CV_RGB(255, 255, 0), 1, 8);
    break;
  }

  //フレーム画像表示
  cv::imshow("Frame", frameImage);

  //------------------------------CG------------------------------
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
  GLfloat pos0[] = {200.0, 700.0, 200.0,
                    1.0}; //(x, y, z, 0(平行光源)/1(点光源))
  glLightfv(GL_LIGHT0, GL_POSITION, pos0);

  facePartsPoints["face"].x *= 1.2;
  facePartsPoints["face"].y *= 1.2;
  facePartsPoints["left_eye"].x *= 1.2;
  facePartsPoints["left_eye"].y *= 1.2;
  facePartsPoints["right_eye"].x *= 1.2;
  facePartsPoints["right_eye"].y *= 1.2;
  facePartsPoints["mouth"].x *= 1.2;
  facePartsPoints["mouth"].y *= 1.2;
  facePartsPoints["nose"].x *= 1.2;
  facePartsPoints["nose"].y *= 1.2;

  //--------------------直方体--------------------
  //色設定
  col[0] = 1.0;
  col[1] = 0.8;
  col[2] = 0.5;
  glMaterialfv(GL_FRONT, GL_DIFFUSE, col); //拡散反射係数
  glMaterialfv(GL_FRONT, GL_AMBIENT, col); //環境光反射係数
  col[0] = 0.5;
  col[1] = 0.5;
  col[2] = 0.5;
  col[3] = 1.0;
  glMaterialfv(GL_FRONT, GL_SPECULAR, col);
  glMaterialf(GL_FRONT, GL_SHININESS, 64); //ハイライト係数
  glPushMatrix();                          //行列一時保存
  glTranslated(facePartsPoints["face"].x, facePartsPoints["face"].y,
               0.0);            //中心座標
  glScaled(250.0, 250.0, 60.0); //拡大縮小
  glutSolidCube(1.0);           //立方体の配置
  glPopMatrix();                //行列復帰

  //--------------------左目--------------------
  //色設定
  col[0] = 1.0;
  col[1] = 1.0;
  col[2] = 1.0;
  glMaterialfv(GL_FRONT, GL_DIFFUSE, col); //拡散反射係数
  glMaterialfv(GL_FRONT, GL_AMBIENT, col); //環境光反射係数
  col[0] = 0.0;
  col[1] = 0.0;
  col[2] = 0.0;
  col[3] = 1.0;
  glMaterialfv(GL_FRONT, GL_SPECULAR, col);
  glMaterialf(GL_FRONT, GL_SHININESS, 64); //ハイライト係数
  glPushMatrix();                          //行列一時保存
  glTranslated(facePartsPoints["left_eye"].x, facePartsPoints["left_eye"].y,
               25.0);           //中心座標
  glScaled(25.0, 25.0, 50.0);   //拡大縮小
  glutSolidSphere(1.0, 50, 50); //球の配置
  glPopMatrix();                //行列復帰

  //--------------------右目--------------------
  //色設定
  col[0] = 1.0;
  col[1] = 1.0;
  col[2] = 1.0;
  glMaterialfv(GL_FRONT, GL_DIFFUSE, col); //拡散反射係数
  glMaterialfv(GL_FRONT, GL_AMBIENT, col); //環境光反射係数
  col[0] = 0.0;
  col[1] = 0.0;
  col[2] = 0.0;
  col[3] = 1.0;
  glMaterialfv(GL_FRONT, GL_SPECULAR, col);
  glMaterialf(GL_FRONT, GL_SHININESS, 64); //ハイライト係数
  glPushMatrix();                          //行列一時保存
  glTranslated(facePartsPoints["right_eye"].x, facePartsPoints["right_eye"].y,
               25.0);
  glScaled(25.0, 25.0, 50.0);   //拡大縮小
  glutSolidSphere(1.0, 50, 50); //球の配置
  glPopMatrix();                //行列復帰

  //--------------------鼻--------------------
  //色設定
  col[0] = 0.0;
  col[1] = 1.0;
  col[2] = 0.0;
  glMaterialfv(GL_FRONT, GL_DIFFUSE, col); //拡散反射係数
  glMaterialfv(GL_FRONT, GL_AMBIENT, col); //環境光反射係数
  col[0] = 0.0;
  col[1] = 0.0;
  col[2] = 0.0;
  col[3] = 1.0;
  glMaterialfv(GL_FRONT, GL_SPECULAR, col);
  glMaterialf(GL_FRONT, GL_SHININESS, 64); //ハイライト係数
  glPushMatrix();                          //行列一時保存
  glTranslated(facePartsPoints["nose"].x, facePartsPoints["nose"].y,
               40.0);         //中心座標
  glScaled(30.0, 70.0, 10.0); //拡大縮小
  glutSolidCube(1.0);
  glPopMatrix(); //行列復帰

  // --------------------口--------------------
  //色設定
  col[0] = 1.0;
  col[1] = 0.0;
  col[2] = 0.0;
  glMaterialfv(GL_FRONT, GL_DIFFUSE, col); //拡散反射係数
  glMaterialfv(GL_FRONT, GL_AMBIENT, col); //環境光反射係数
  col[0] = 0.0;
  col[1] = 0.0;
  col[2] = 0.0;
  col[3] = 1.0;
  glMaterialfv(GL_FRONT, GL_SPECULAR, col);
  glMaterialf(GL_FRONT, GL_SHININESS, 64); //ハイライト係数
  glPushMatrix();                          //行列一時保存
  glTranslated(facePartsPoints["mouth"].x, facePartsPoints["mouth"].y,
               40.0);          //中心座標
  glScaled(150.0, 40.0, 10.0); //拡大縮小
  glutSolidCube(1.0);
  glPopMatrix(); //行列復帰

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
