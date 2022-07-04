// g++ dip02.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
// g++ dip02.cpp `pkg-config --cflags --libs opencv`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, const char *argv[]) {

  //①画像ファイルの読み込み
  cv::Mat sourceImage = cv::imread("color2.jpg", cv::IMREAD_COLOR);
  if (sourceImage.data == 0) { //画像ファイルが読み込めなかった場合
    printf("File not found\n");
    exit(0);
  }
  printf("Width=%d, Height=%d\n", sourceImage.cols, sourceImage.rows);

  //②画像格納用オブジェクト"resultImage"の生成
  // cv::Mat resultImage = cv::Mat(sourceImage.size(), CV_8UC3);

  // CV_8UC3 1画素3チャンネルで，各チャンネル8bit
  // CV_8UC1  1画素1チャンネルで，各チャンネル8bit
  // CV_32FC3 1画素3チャンネルで，各チャンネル32bit浮動小数(float)
  // CV_64FC1 1画素1チャンネルで，各チャンネル64bit浮動小数(double)

  //③ウィンドウの生成と移動
  cv::namedWindow("Source");
  cv::moveWindow("Source", 0, 0);
  cv::namedWindow("Result");
  cv::moveWindow("Result", 400, 0);

  cv::Mat hsvImage;

  //④画像の画素単位の読み込み・処理・書き込み

  cv::Mat resultImage = sourceImage.clone();
  cv::cvtColor(sourceImage, resultImage, cv::COLOR_RGB2HSV);

  // int my = sourceImage.rows / 2;
  // int mx = sourceImage.cols / 2;

  // float length = 300;

  cv::Vec3b s; //画素値格納用変数("s[0]":B, "s[1]":G, "s[2]":R)
  for (int i = 0; i < sourceImage.rows; i++) {
    for (int j = 0; j < sourceImage.cols; j++) {
      s = resultImage.at<cv::Vec3b>(i, j);

      // s[0] *= 0.6;
      // s[1] *= 0.8;
      // s[2] *= 1.2;

      if (s[0] >= 30 && s[0] <= 70 && s[1] >= 10) {

      } else {
        s[0] = 0;
        s[1] = 0;
        s[2] = 0;
      }
      // printf("%d, ", s[0]);
      resultImage.at<cv::Vec3b>(i, j) = s;
      cv::cvtColor(resultImage, resultImage, cv::COLOR_HSV2RGB);
      s = resultImage.at<cv::Vec3b>(i, j);
      if (s[0] < 100 && s[1] > 100 && s[2] < 100) {

      } else {
        s[0] = 0;
        s[1] = 0;
        s[2] = 0;
      }
    }
    // printf("\n");
  }

  //⑤ウィンドウへの画像の表示
  cv::imshow("Source", sourceImage);
  cv::imshow("Result", resultImage);

  //⑥キー入力待ち
  cv::waitKey(0);

  //⑦画像の保存
  cv::imwrite("result.jpg", resultImage);

  return 0;
}
