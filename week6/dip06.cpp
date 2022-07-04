#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ
#include <stdlib.h>

int main(int argc, const char *argv[]) {
  //①ルートディレクトリの画像ファイル"col.jpg"を読み込んで"sourceImage"に格納
  cv::Mat sourceImage = cv::imread("kadai.jpg", cv::IMREAD_COLOR);
  if (sourceImage.data == 0) { //画像ファイルが読み込めなかった場合
    printf("File not found\n");
    exit(0);
  }
  printf("Width=%d, Height=%d\n", sourceImage.cols, sourceImage.rows);

  //②画像格納用インスタンスの生成
  cv::Mat grayImage(sourceImage.size(),
                    CV_8UC1); //グレースケール画像用（1チャンネル）
  cv::Mat binImage(sourceImage.size(), CV_8UC1); //２値画像用（1チャンネル）
  cv::Mat contourImage(sourceImage.size(),
                       CV_8UC3); //輪郭表示画像用（3チャンネル）

  //③原画像をグレースケール画像に、グレースケール画像を２値画像に変換
  //"sourceImage"をグレースケール画像に変換して"grayImage"に出力
  cv::cvtColor(sourceImage, grayImage, cv::COLOR_BGR2GRAY);
  //"grayImage"を2値化して"grayImage"に出力
  cv::threshold(grayImage, binImage, 50, 255, cv::THRESH_BINARY);
  //"sourceImage"のコピーを"contourImage"に出力
  contourImage = sourceImage.clone();

  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
  // cv::erode(contourImage, contourImage, element, cv::Point(-1, -1), 1);

  //④輪郭点格納用配列、輪郭の階層用配列の確保

  std::vector<std::vector<cv::Point>> contours;

  //⑤"binImage"からの輪郭抽出処理

  cv::Mat tapImage = binImage.clone();
  cv::imwrite("bin1.png", binImage);
  cv::dilate(binImage, binImage, element, cv::Point(-1, -1), 2);
  cv::imwrite("bin2.png", binImage);
  cv::erode(binImage, binImage, element, cv::Point(-1, -1), 10);
  cv::imwrite("bin3.png", binImage);
  cv::dilate(binImage, binImage, element, cv::Point(-1, -1), 8);
  cv::imwrite("bin4.png", binImage);
  cv::findContours(binImage, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
  int circleCount = 1;

  for (int i = contours.size() - 1; i > 0; i--) {
    double length = cv::arcLength(contours[i], true);
    double area = cv::contourArea(contours[i]);

    double circularity = (4 * M_PI) * area / (length * length);
    if (circularity > 0.82) {
      printf("円%d & %.2lf & Area=%.2lf\\\\\n", circleCount, length, area);
      cv::drawContours(contourImage, contours, i, cv::Scalar(255, 255, 255), 2,
                       8);
      cv::Point p = contours[i][0];
      p.y += 50;
      cv::putText(contourImage, std::to_string(circleCount++), p,
                  cv::FONT_HERSHEY_COMPLEX, 1.2, cv::Scalar(255, 100, 100), 2);
    }
  }

  //⑥ウィンドウを生成して各画像を表示
  //原画像
  cv::namedWindow("Source");         //ウィンドウの生成
  cv::moveWindow("Source", 0, 50);   //ウィンドウの表示位置の指定
  cv::imshow("Source", sourceImage); //ウィンドウに画像を表示
                                     //グレースケール(2値化)
  cv::namedWindow("Gray");           //ウィンドウの生成
  cv::moveWindow("Gray", 150, 50);   //ウィンドウの表示位置の指定
  cv::imshow("Gray", binImage);      //ウィンドウに画像を表示
                                     //輪郭画像(原画像に輪郭を追加)
  cv::namedWindow("Contour");        //ウィンドウの生成
  cv::moveWindow("Contour", 300, 50);  //ウィンドウの表示位置の指定
  cv::imshow("Contour", contourImage); //ウィンドウに画像を表示

  //⑦キー入力があるまでここでストップ
  cv::waitKey(0);

  cv::imwrite("dist.png", contourImage);

  //メッセージを出力して終了
  printf("Finished\n");
  return 0;
}
