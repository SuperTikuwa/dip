#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  // cv::Mat sourceImage = cv::imread("./img/図4.jpg");

  // for (int i = 0; i < sourceImage.rows; i++) {
  //   for (int j = 0; j < sourceImage.cols; j++) {
  //     cv::Vec3b pixel = sourceImage.at<cv::Vec3b>(i, j);
  //     if (pixel[0] <= 100 && pixel[1] >= 180 && pixel[2] >= 180) {
  //     } else {
  //       sourceImage.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
  //     }
  //   }
  // }

  // cv::Mat grayImage;
  // cv::Mat binImage;
  // cv::cvtColor(sourceImage, grayImage, cv::COLOR_BGR2GRAY);
  // cv::threshold(grayImage, binImage, 5, 255, cv::THRESH_BINARY);

  // // 輪郭抽出
  // std::vector<std::vector<cv::Point>> contours;
  // cv::findContours(binImage, contours, cv::RETR_EXTERNAL,
  //                  cv::CHAIN_APPROX_SIMPLE);

  // cv::Mat contourImage = cv::Mat::zeros(sourceImage.size(), CV_8UC3);
  // for (int i = 0; i < contours.size(); i++) {
  //   double area = cv::contourArea(contours[i]);
  //   double length = cv::arcLength(contours[i], true);
  //   printf("%lf", 4.0 * M_PI * area / pow(length, 2));
  //   double circularity = 4.0 * M_PI * area / pow(length, 2);
  //   if (circularity > 0.8) {
  //     cv::drawContours(contourImage, contours, i, cv::Scalar(255, 255, 255));
  //   }
  // }

  // cv::imshow("sourceImage", sourceImage);
  // cv::imshow("grayImage", grayImage);
  // cv::imshow("binImage", binImage);

  // cv::waitKey(0);

  return 0;
}