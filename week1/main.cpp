#include <opencv2/opencv.hpp>

int main(void) {
  cv::Mat hoge = cv::imread("./img.jpeg");
  cv::imshow("hoge", hoge);
  cv::waitKey(0);
  return 0;
}