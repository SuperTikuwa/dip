#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

cv::Mat createMask(cv::Mat src) {
  cv::Mat mask = cv::Mat(src.size(), CV_8UC1);

  cv::Vec3b s;

  for (int i = 0; i < src.rows; i++) {
    for (int j = 0; j < src.cols; j++) {
      s = src.at<cv::Vec3b>(i, j);
      if (s[0] < 100 && s[1] > 230 && s[2] < 100) {
        mask.at<uchar>(i, j) = 0;
      } else {
        mask.at<uchar>(i, j) = 255;
      }
    }
  }

  return mask;
}

cv::Mat stackImage(cv::Mat back, cv::Mat front) {
  cv::Vec3b s;
  cv::Mat dist = back.clone();
  for (int i = 0; i < front.rows; i++) {
    for (int j = 0; j < front.cols; j++) {
      s = front.at<cv::Vec3b>(i, j);
      if (s[0] != 0 || s[1] != 0 || s[2] != 0) {
        dist.at<cv::Vec3b>(i, j) = s;
      }
    }
  }

  return dist;
}
cv::Mat stackImageCenter(cv::Mat back, cv::Mat front) {
  cv::Vec3b s;
  cv::Mat dist = back.clone();
  cv::Point2i start = cv::Point2i(back.cols / 2 - front.cols / 2,
                                  back.rows / 2 - front.rows / 2);

  for (int i = 0; i < front.rows; i++) {
    for (int j = 0; j < front.cols; j++) {
      s = front.at<cv::Vec3b>(i, j);
      if (s[0] != 0 || s[1] != 0 || s[2] != 0) {
        dist.at<cv::Vec3b>(start.y + i, start.x + j) = s;
      }
    }
  }

  return dist;
}

cv::Mat stackImageDown(cv::Mat back, cv::Mat front) {
  cv::Vec3b s;
  cv::Mat dist = back.clone();
  cv::Point2i start = cv::Point2i(back.cols / 2 - front.cols / 2,
                                  back.rows / 2 - front.rows / 2);

  for (int i = 0; i < front.rows; i++) {
    for (int j = 0; j < front.cols; j++) {
      s = front.at<cv::Vec3b>(i, j);
      if (s[0] != 0 || s[1] != 0 || s[2] != 0) {
        dist.at<cv::Vec3b>(start.y + i, start.x + j) = s;
      }
    }
  }

  return dist;
}

int main(int argc, const char *argv[]) {

  cv::Mat girlsImage = cv::imread("./girls.jpg");
  cv::Mat domeImage = cv::imread("./dome.jpg");
  cv::Mat logoImage = cv::imread("./logo.jpg");

  cv::Mat resultImage = cv::Mat(domeImage.size(), CV_8UC3);
  cv::Mat tmpGirls = cv::Mat(domeImage.size(), CV_8UC3);
  cv::Mat tmpLogo = cv::Mat(domeImage.size(), CV_8UC3);

  cv::VideoWriter rec("rec.mpg", cv::VideoWriter::fourcc('P', 'I', 'M', '1'),
                      30, resultImage.size());

  cv::Mat girlsBack = cv::Mat(domeImage.size(), CV_8UC3);
  girlsBack = cv::Scalar(0, 255, 0);
  girlsImage = stackImageCenter(girlsBack, girlsImage);

  cv::Mat logoBack = cv::Mat(domeImage.size(), CV_8UC3);
  logoBack = cv::Scalar(0, 255, 0);
  logoImage = stackImageCenter(logoBack, logoImage);

  cv::Mat girlsMask;
  cv::Mat logoMask;
  cv::Mat translatedLogo;

  // cv::resize(girlsImage, girlsImage, resultImage.size());

  cv::Point2f center = cv::Point2f(logoImage.cols / 2, logoImage.rows / 2);

  cv::Point2f from[4], to[4];

  from[0] = cv::Point2f(0, 0);
  from[1] = cv::Point2f(girlsImage.cols, 0);
  from[2] = cv::Point2f(girlsImage.cols, girlsImage.rows);
  from[3] = cv::Point2f(0, girlsImage.rows);

  int px = 0;
  double py = 0;
  printf("%f\n", py);

  int theta = 0;
  while (1) {

    px = cos((double)theta / 18.0 * M_PI) * 500;

    py = sin((double)theta / 18.0 * M_PI) * 150;

    // printf("%f\n", py);

    to[0] = cv::Point2f(0 + px, 0 + (float)py);
    to[1] = cv::Point2f(girlsImage.cols + px, 0 + (float)py);
    to[2] = cv::Point2f(girlsImage.cols + px, girlsImage.rows + (float)py);
    to[3] = cv::Point2f(0 + px, girlsImage.rows + (float)py);

    cv::Mat persMat = cv::getPerspectiveTransform(from, to);

    resultImage = domeImage.clone();

    girlsMask = createMask(girlsImage);
    tmpGirls = cv::Scalar(0, 0, 0);
    girlsImage.copyTo(tmpGirls, girlsMask);

    cv::warpPerspective(tmpGirls, tmpGirls, persMat, tmpGirls.size(),
                        cv::INTER_LINEAR, cv::BORDER_CONSTANT,
                        cv::Scalar(0, 0, 0));
    resultImage = stackImageDown(domeImage, tmpGirls);

    cv::Mat logoRotateMat = cv::getRotationMatrix2D(center, theta++, 1);

    cv::warpAffine(logoImage, translatedLogo, logoRotateMat, logoImage.size(),
                   cv::INTER_LINEAR, cv::BORDER_CONSTANT,
                   cv::Scalar(0, 255, 0));

    logoMask = createMask(translatedLogo);

    tmpLogo = cv::Scalar(0, 0, 0);
    translatedLogo.copyTo(tmpLogo, logoMask);
    resultImage = stackImage(resultImage, tmpLogo);

    rec << resultImage;

    cv::imshow("Result", resultImage);
    // cv::imshow("Girls", girlsImage);
    // cv::imshow("Logo", translatedLogo);
    // cv::imshow("Logoback", logoMask);
    // cv::imshow("Tmp", tmpGirls);

    char k = cv::waitKey(1);
    switch (k) {
    case 'q':
      return 0;
      break;

    default:
      break;
    }
  }
}
