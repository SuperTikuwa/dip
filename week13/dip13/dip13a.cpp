//(OpenCV4) g++ dip13a.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
//(OpenCV3) g++ dip13a.cpp `pkg-config --cflags --libs opencv`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, char *argv[]) {
  //ビデオキャプチャの初期化
  cv::VideoCapture capture("pantora.mov"); //ビデオファイルをオープン
  // cv::VideoCapture capture("senro.mov");  //ビデオファイルをオープン
  if (capture.isOpened() == 0) { //オープンに失敗した場合
    printf("Capture not found\n");
    return -1;
  }

  //画像格納用インスタンス準備
  cv::Mat frameImage; //ビデオキャプチャ用
  int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
  int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
  cv::Size imageSize(width, height);               //ビデオ画像サイズ
  printf("imageSize = (%d, %d)\n", width, height); //ビデオ画像サイズ表示

  //画像表示用ウィンドウの生成
  cv::namedWindow("Frame");

  //動画処理用無限ループ
  while (1) {
    //ビデオキャプチャから1フレーム"frameImage"に取り込み
    capture >> frameImage;
    //ビデオが終了したら無限ループから脱出
    if (frameImage.data == NULL) {
      break;
    }

    //画像表示
    cv::imshow("Frame", frameImage);

    // Convert flameImage to gray scale image
    cv::Mat grayImage;
    cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);
    cv::imshow("Gray", grayImage);

    // Convert grayImage to binary image
    cv::Mat binaryImage;
    cv::threshold(grayImage, binaryImage, 190, 255, cv::THRESH_BINARY_INV);
    cv::imshow("Binary", binaryImage);

    // remove noise
    cv::medianBlur(binaryImage, binaryImage, 5);

    // Erode and dilate binary image to remove noise
    cv::Mat dilateImage;
    cv::dilate(binaryImage, dilateImage, cv::Mat(), cv::Point(-1, -1), 6);
    cv::imshow("Dilate", dilateImage);
    cv::Mat erodeImage;
    cv::erode(dilateImage, erodeImage, cv::Mat(), cv::Point(-1, -1), 14);
    cv::imshow("Erode", erodeImage);
    cv::dilate(erodeImage, dilateImage, cv::Mat(), cv::Point(-1, -1), 8);

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(dilateImage, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    // Draw contours and fill with white to binaryImage
    cv::drawContours(binaryImage, contours, -1, cv::Scalar(255, 255, 255), -1);
    cv::imshow("Binary2", binaryImage);

    std::map<int, double> circularityMap;
    for (int i = 0; i < contours.size(); i++) {
      // Check contour points
      bool isNotBread = false;
      std::vector<cv::Point> points = contours[i];
      for (int j = 0; j < points.size(); j++) {
        if (points[j].x == 0 || points[j].y == 0 || points[j].x == width - 1 ||
            points[j].y == height - 1) {
          isNotBread = true;
          break;
        }
      }

      if (isNotBread) {
        continue;
      }

      // Calculate circularity
      double area = cv::contourArea(contours[i], false);
      double perimeter = cv::arcLength(contours[i], false);
      double circularity = 4 * M_PI * area / (perimeter * perimeter);
      circularityMap[i] = circularity;
    }

    // Draw contours
    for (int i = 0; i < contours.size(); i++) {
      // Check contour points
      bool isNotBread = false;
      std::vector<cv::Point> points = contours[i];
      for (int j = 0; j < points.size(); j++) {
        if (points[j].x == 0 || points[j].y == 0 || points[j].x == width - 1 ||
            points[j].y == height - 1) {
          isNotBread = true;
          break;
        }
      }

      if (isNotBread) {
        continue;
      }

      bool isScorn = true;
      bool isCheese = true;
      bool isChocolate = true;
      bool isBanana = true;
      bool isLemon = true;

      cv::Scalar color = cv::Scalar(0, 0, 0);

      // If the color of inside contour is yellow, contour color is red
      for (int j = 0; j < contours[i].size(); j++) {
        cv::Vec3b s = frameImage.at<cv::Vec3b>(i, j);
        int count = 0;
        if (s[0] >= 200 && s[1] >= 200) {
          count++;
        }

        if (count > cv::contourArea(contours[i]) / 2) {
          color = cv::Scalar(0, 0, 255);
          break;
        }
        printf("%d %lf\n", count, cv::contourArea(contours[i]) / 2);
      }

      // Draw circularity inside of contours
      cv::circle(frameImage, points[0], 2, color);
      cv::putText(frameImage, std::to_string(circularityMap[i]), points[0],
                  cv::FONT_HERSHEY_PLAIN, 1, color);

      // Draw contours to frameImage
      cv::drawContours(frameImage, contours, i, color, 2, 8,
                       std::vector<cv::Vec4i>(), 0, cv::Point());
    }
    cv::imshow("Contours", frameImage);

    //キー入力待ち
    int key = cv::waitKey(1);
    //[Q]が押されたら無限ループ脱出
    if (key == 'q')
      break;
  }

  //終了処理
  //カメラ終了
  capture.release();
  //メッセージを出力して終了
  printf("Finished\n");
  return 0;
}
