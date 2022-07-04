//(OpenCV4) g++ -std=c++11 dip07a.cpp `pkg-config --cflags --libs opencv4`
//(OpenCV3) g++ dip07a.cpp `pkg-config --cflags --libs opencv`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, char *argv[]) {
  //①ビデオキャプチャの初期化
  cv::VideoCapture capture("walking.mov"); //ビデオファイルをオープン
  if (capture.isOpened() == 0) {
    printf("Camera not found\n");
    return -1;
  }

  //②画像格納用インスタンス準備
  cv::Size imageSize(720, 405);
  cv::Mat originalImage;
  cv::Mat frameImage(imageSize, CV_8UC3);
  cv::Mat backImage(imageSize, CV_8UC3);
  cv::Mat subImage(imageSize, CV_8UC3);
  cv::Mat resultImage(imageSize, CV_8UC3);
  cv::Mat subBinImage(imageSize, CV_8UC1);

  cv::VideoWriter rec("rec.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'),
                      30, resultImage.size());

  //③画像表示用ウィンドウの生成
  cv::namedWindow("Frame");
  cv::moveWindow("Frame", 0, 0);
  cv::namedWindow("Back");
  cv::moveWindow("Back", 50, 50);
  cv::namedWindow("Subtraction");
  cv::moveWindow("Subtraction", 100, 100);

  //④動画処理用無限ループ
  while (1) {
    //(a)ビデオキャプチャから1フレーム"originalImage"を取り込んで，"frameImage"を生成
    capture >> originalImage;
    //ビデオが終了したら無限ループから脱出
    if (originalImage.data == NULL)
      break;
    //"originalImage"をリサイズして"frameImage"生成
    cv::resize(originalImage, frameImage, imageSize);

    //(b)"frameImage"と"backImage"との差分画像"subImage"の生成
    cv::absdiff(frameImage, backImage, subImage);
    cv::cvtColor(subImage, subBinImage, cv::COLOR_RGB2GRAY);
    cv::threshold(subBinImage, subBinImage, 30, 255, cv::THRESH_BINARY);
    cv::erode(subBinImage, subBinImage, cv::Mat(), cv::Point(-1, -1), 1);
    cv::dilate(subBinImage, subBinImage, cv::Mat(), cv::Point(-1, -1), 10);
    cv::erode(subBinImage, subBinImage, cv::Mat(), cv::Point(-1, -1), 2);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(subBinImage, contours, cv::RETR_LIST,
                     cv::CHAIN_APPROX_NONE);

    resultImage = cv::Scalar(0);
    frameImage.copyTo(resultImage, subBinImage);

    uint area = 0;
    for (int i = 0; i < contours.size(); i++) {
      area = cv::contourArea(contours[i]);
      if (area > 1300) {
        cv::Point p = contours[i][0];
        p.x += 10;
        p.y += 20;
        cv::circle(resultImage, p, 20, cv::Scalar(255, 0, 0), 2);
      }
    }

    // cv::circle(resultImage,
    //            cv::Point(resultImage.cols / 2, resultImage.rows / 2),
    //            area * 0.005, cv::Scalar(0, 0, 255), 3);

    //(c)"frameImage"，"backImage"，"subImage"の表示
    cv::imshow("Frame", frameImage);
    cv::imshow("Back", backImage);
    cv::imshow("Subtraction", subImage);
    cv::imshow("BinarySubtraction", subBinImage);
    cv::imshow("Result", resultImage);

    //(d)"frameImage"で"backImage"を更新
    frameImage.copyTo(backImage);

    rec << resultImage;
    //(e)キー入力待ち
    int key = cv::waitKey(10);
    //[Q]が押されたら無限ループ脱出
    if (key == 'q')
      break;
    if (key == 'c') {
      frameImage.copyTo(backImage);
    }
  }

  //⑤終了処理
  //カメラ終了
  capture.release();
  //メッセージを出力して終了
  printf("Finished\n");
  return 0;
}
