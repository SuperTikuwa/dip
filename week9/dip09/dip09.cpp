/*
g++ dip09.cpp -std=c++11 `pkg-config --cflags --libs opencv opencv4`
g++ dip09.cpp `pkg-config --cflags --libs opencv`
 */

#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, char *argv[]) {
  //①ビデオキャプチャの初期化
  cv::VideoCapture capture("swingcar.mov"); //ビデオファイルをオープン
  if (capture.isOpened() == 0) {
    printf("Capture not found\n");
    return -1;
  }

  //②画像格納用インスタンス準備
  int w = capture.get(cv::CAP_PROP_FRAME_WIDTH); // captureから動画横サイズ取得
  int h = capture.get(cv::CAP_PROP_FRAME_HEIGHT); // captureから動画縦サイズ取得
  cv::Size imageSize(w, h);
  cv::Mat originalImage;
  cv::Mat frameImage(imageSize, CV_8UC3);  // 3チャンネル
  cv::Mat grayImage(imageSize, CV_8UC1);   // 1チャンネル
  cv::Mat edgeImage(imageSize, CV_8UC1);   // 1チャンネル
  cv::Mat resultImage(imageSize, CV_8UC3); // 1チャンネル

  cv::VideoWriter rec("rec.mpg", cv::VideoWriter::fourcc('P', 'I', 'M', '1'),
                      30, imageSize);

  //③画像表示用ウィンドウの生成
  cv::namedWindow("Frame");
  cv::moveWindow("Frame", 0, 0);
  cv::namedWindow("Edge");
  cv::moveWindow("Edge", 100, 100);

  cv::Point2f center = cv::Point2f(w / 2, h / 2);

  //⑤動画処理用無限ループ
  while (1) {
    //④ハフ変換用変数
    std::vector<cv::Point2f> lines;
    std::vector<cv::Point3f> circles;

    //(a)ビデオキャプチャから1フレーム"originalImage"を取り込んで，"frameImage"を生成
    capture >> originalImage;
    //ビデオが終了したら巻き戻し
    if (originalImage.data == NULL) {
      capture.set(cv::CAP_PROP_POS_FRAMES, 0);
      continue;
    }
    //"originalImage"をリサイズして"frameImage"生成
    cv::resize(originalImage, frameImage, imageSize);

    //(b)"frameImage"からグレースケール画像"grayImage"を生成
    cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);

    //(c)"grayImage"からエッジ画像"edgeImage"を生成
    cv::Canny(grayImage, edgeImage, 120, 160, 3);

    //(d)"edgeImage"に直線検出ハフ変換を施して，閾値(250)以上の投票数を得た直線群(ρ,θ)を"lines"に格納
    cv::HoughLines(edgeImage, lines, 1, M_PI / 180, 200);
    // cv::HoughCircles(grayImage, circles, cv::HOUGH_GRADIENT, 1, 30, 20, 10,
    // 10,
    //                  22);
    float sumTheta = 0;
    //(e)ハフ変換結果表示
    //検出された直線の数("lines.size()")と閾値(100)の小さい方の数だけ繰り返し
    for (int i = 0; i < MIN(lines.size(), 200); i++) {
      cv::Point2f line = lines[i];
      float rho = line.x;
      float theta = line.y;
      sumTheta += theta;
      double a = cos(theta);
      double b = sin(theta);
      double x0 = a * rho;
      double y0 = b * rho;
      cv::Point p1, p2;
      p1.x = x0 - 1000 * b;
      p1.y = y0 + 1000 * a;
      p2.x = x0 + 1000 * b;
      p2.y = y0 - 1000 * a;
      cv::line(frameImage, p1, p2, cv::Scalar(0, 0, 255), 2, 8, 0);
    }

    sumTheta /= MIN(lines.size(), 200);
    printf("%lf\n", sumTheta);

    cv::Mat rotateMat =
        cv::getRotationMatrix2D(center, sumTheta * 180 / M_PI - 90, 1);
    cv::warpAffine(frameImage, resultImage, rotateMat, frameImage.size(), 1, 0,
                   cv::Scalar(0, 0, 0));

    // for (int i = 0; i < MIN(circles.size(), 200); i++) {
    //   cv::Point3f circle = circles[i];
    //   float x0 = circle.x;
    //   float y0 = circle.y;
    //   float r = circle.z;

    //   cv::circle(frameImage, cv::Point(x0, y0), 3, cv::Scalar(0, 255, 0), -1,
    //              8);
    //   cv::circle(frameImage, cv::Point(x0, y0), r, cv::Scalar(0, 0, 255), 2,
    //   8);
    // }

    //(f)"frameImage"，"edgeImage"の表示
    rec << resultImage;
    cv::imshow("Frame", frameImage);
    cv::imshow("Edge", edgeImage);
    cv::imshow("Result", resultImage);

    //(g)キー入力待ち
    int key = cv::waitKey(10);
    //[Q]が押されたら無限ループ脱出
    if (key == 'q')
      break;
  }

  //⑥終了処理
  //カメラ終了
  capture.release();
  //メッセージを出力して終了
  printf("Finished\n");
  return 0;
}
