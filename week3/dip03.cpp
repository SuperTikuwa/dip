// g++ dip03.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, const char *argv[]) {

  //①カメラの初期化
  cv::VideoCapture capture("./dance.mov");
  cv::VideoCapture land("./landscape.mov");

  if (capture.isOpened() == 0) {
    printf("Camera not found.");
  }

  //②画像格納用インスタンス準備
  int width = 640, height = 360; //処理画像サイズ
  cv::Mat captureImage;          //キャプチャ用
  cv::Mat landImage;             //キャプチャ用
  cv::Mat frameImage = cv::Mat(cv::Size(width, height), CV_8UC3); //処理用
  cv::Mat hsvImage = cv::Mat(cv::Size(width, height), CV_8UC3);
  cv::Mat hsvLand; //処理用
  cv::Mat recImage = cv::Mat(cv::Size(width, height), CV_8UC3);
  cv::Mat resultImage;

  cv::Vec3b s;

  //③ウィンドウの生成と移動
  cv::namedWindow("Frame");
  cv::moveWindow("Frame", 0, 0);
  cv::namedWindow("Result");
  cv::moveWindow("Result", 0, height);

  cv::VideoWriter rec("rec.mpg", cv::VideoWriter::fourcc('P', 'I', 'M', '1'),
                      30, recImage.size());

  int frame = 0;
  while (1) {

    //④カメラから1フレーム読み込んでcaptureImageに格納
    capture >> captureImage;
    land >> landImage;

    if (captureImage.data == 0) {
      break;
    }

    //⑤captureImageをframeImageに合わせてサイズ変換して格納
    cv::resize(captureImage, frameImage, frameImage.size());

    //⑥画像処理
    cv::cvtColor(frameImage, hsvImage, cv::COLOR_BGR2HSV);
    cv::cvtColor(landImage, hsvLand, cv::COLOR_BGR2HSV);
    resultImage = frameImage.clone();
    // printf("-----\n");
    for (int i = 0; i < hsvImage.rows; i++) {
      for (int j = 0; j < hsvImage.cols; j++) {
        s = hsvImage.at<cv::Vec3b>(i, j);

        // if ((i == 0 || i == hsvImage.rows - 1 || i == hsvImage.rows / 2) &&
        //     (j == 0 || j == hsvImage.cols - 1 || j == hsvImage.cols / 2)) {

        //   printf("(%d,%d) = %d %d %d\n", j, i, s[0], s[1], s[2]);
        // }

        if ((s[0] >= 40 && s[0] <= 47) && (s[1] >= 155 && s[1] <= 231) &&
            (s[2] >= 151 && s[2] <= 255)) {
          s[0] = 0;
          s[1] = 0;
          s[2] = 0;
        }

        if (s[2] > 170 && 15 < s[0] && s[0] < 165) {
          s[0] += 80;
          s[1] *= 1.5;
          // s[2] /= 1.1;

          // if (s[0] >= 180) {
          //   s[0] -= 180;
          // }
          // if (s[1] >= 255) {
          //   s[1] = 255;
          // }
          // if (s[2] >= 255) {
          //   s[2] = 255;
          // }
        }

        if (s[0] == 0 && s[1] == 0 && s[2] == 0) {
          s = hsvLand.at<cv::Vec3b>(i, j);
        }
        hsvImage.at<cv::Vec3b>(i, j) = s;
      }
    }

    //⑦ウィンドウへの画像の表示
    cv::imshow("Frame", frameImage);

    cv::cvtColor(hsvImage, recImage, cv::COLOR_HSV2BGR);

    rec << recImage;
    cv::imshow("Result", recImage);

    //⑧キー入力待ち
    char key = cv::waitKey(33);
    if (key == 'q') {
      break;
    }
  }

  return 0;
}
