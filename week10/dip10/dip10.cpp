#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

//配列の象限入れ替え用関数の宣言
void ShiftDFT(const cv::Mat &src_arr, cv::Mat &dst_arr);

int main(int argc, const char *argv[]) {
  //①原画像のグレースケール画像を"sourceImage"に格納
  //原画像ファイルを"sourceImage0"に格納
  // cv::Mat sourceImage0 = cv::imread("./nymegami.jpg", cv::IMREAD_ANYCOLOR);
  // if (sourceImage0.data ==
  //     0) { //ファイルが見つからないときはメッセージを表示して終了
  //   printf("Source not found\n");
  //   exit(0);
  // }
  // // sourceImage0のチャンネル数チェック、 sourceImage の生成
  // printf("Channels = %d\n", sourceImage0.channels());
  cv::Mat sourceImage;
  cv::Mat colorSourceImage;
  // if (sourceImage0.channels() == 3) {
  //   cv::cvtColor(sourceImage0, sourceImage, cv::COLOR_BGR2GRAY);
  // } else {
  //   sourceImage = sourceImage0.clone();
  // }

  cv::VideoCapture capture("./dip10-02-k19092.mp4");
  if (!capture.isOpened()) {
    printf("Camera not opened\n");
  }

  cv::Mat frameImage;
  cv::Mat grayFrameImage;

  // //②作業用配列領域、描画用画像領域の宣言
  // cv::Mat cxMatrix(sourceImage.size(),
  //                  CV_64FC2); //複素数用(実数 2 チャンネル)
  // cv::Mat ftMatrix(sourceImage.size(), CV_64FC2); //複素数用(実数 2
  // チャンネル) cv::Mat spcMatrix(sourceImage.size(), CV_64FC1);
  // //スペルトルデータ(実数) cv::Mat spcImage(sourceImage.size(), CV_8UC1);
  // //スペクトル画像(自然数) cv::Mat resultImage(sourceImage.size(), CV_8UC1);
  // //逆変換画像(自然数)

  //ウィンドウ生成
  // //原画像
  // cv::namedWindow("sourceImage");
  // cv::moveWindow("sourceImage", 0, 0);
  // //フーリエスペクトル画像
  // cv::namedWindow("spcImage");
  // cv::moveWindow("spcImage", sourceImage.cols, 0);
  // //逆変換画像
  // cv::namedWindow("Result");
  // cv::moveWindow("Result", sourceImage.cols * 2, 0);

  cv::VideoWriter rec("capture.mp4",
                      cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 30,
                      cv::Size(720, 480), false);

  bool isContinue = true;
  double sigma = 0.0;
  while (isContinue) {
    capture >> frameImage;
    if (frameImage.channels() == 3) {
      cv::cvtColor(frameImage, grayFrameImage, cv::COLOR_RGB2GRAY);
    } else {
      grayFrameImage = frameImage.clone();
    }

    cv::resize(grayFrameImage, sourceImage, cv::Size(720, 480));
    // cv::resize(frameImage, colorSourceImage, cv::Size(720, 480));

    //②原画像を複素数(実数部と虚数部)の 2
    //チャンネル配列(画像)として表現．虚数部はゼロ
    // cv::Mat RealImaginary[] = {cv::Mat_<double>(sourceImage),
    //                            cv::Mat::zeros(sourceImage.size(), CV_64FC1)};
    // //実数部と虚数部を一組にした 2 チャンネル画像 cxMatrix を生成
    // cv::merge(RealImaginary, 2, cxMatrix);

    //③フーリエ変換
    //フーリエ変換の実施（cxMatrix → ftMatrix）
    // cv::dft(cxMatrix, ftMatrix);
    // //配列の象限入れ替え（低周波成分が画像中央部、高周波成分が画像周辺部
    // ShiftDFT(ftMatrix, ftMatrix);

    //課題：ここでftMatrixに対してフィルタリング
    // cv::Vec2d s(0.0, 0.0);
    // int r = 50;

    // cv::Point2d center(ftMatrix.cols / 2, ftMatrix.rows / 2);
    // for (int i = 0; i < ftMatrix.rows; i++) {
    //   for (int j = 0; j < ftMatrix.cols; j++) {
    //     if (sqrt(pow(center.x - j, 2.0) + pow(center.y - i, 2.0)) <= r) {
    //       ftMatrix.at<cv::Vec2d>(i, j) = s;
    //     }
    //   }
    // }

    // for (int i = 0; i < ftMatrix.rows; i++) {
    //   for (int j = 0; j < ftMatrix.cols; j++) {
    //     // −4�*(�* + �*)���9−2�*�*(�* + �*)>
    //     ftMatrix.at<uchar>(i, j) =
    //         (-4) * M_PI * M_PI * ((i * i) + (j * j)) *
    //         exp(((-2) * M_PI * M_PI * sigma * sigma * ((i * i) + (j * j))));
    //   }
    // }

    // //④フーリエスペクトル"spcMatrix"の計算
    // // ftMatrix を実数部 RealImaginary[0] と虚数部 RealImaginary[1] に分解
    // cv::split(ftMatrix, RealImaginary);

    // //フーリエスペクトル各要素を計算して spcMatrix に格納(spc =
    // sqrt(re^2+im^2)) cv::magnitude(RealImaginary[0], RealImaginary[1],
    // spcMatrix);

    // //⑤フーリエスペクトルからフーリエスペクトル画像を生成
    // //表示用にフーリエスペクトル spcMatrix の各要素の対数をとる(log(1+spc))
    // spcMatrix += cv::Scalar::all(1);
    // cv::log(spcMatrix, spcMatrix);
    // //フーリエスペクトルを 0〜1 にスケーリングしてフーリエスペクトル画像
    // // spcImage にコピー
    // cv::normalize(spcMatrix, spcImage, 0, 1, cv::NORM_MINMAX);

    // //⑥フーリエ逆変換
    // //配列の象限入れ替え（低周波成分が画像周辺部、高周波成分が画像中央部）
    // ShiftDFT(ftMatrix, ftMatrix);
    // //フーリエ逆変換の実施（ftMatrix → cxMatrix）
    // cv::idft(ftMatrix, cxMatrix);
    // // cxMatrix を実数部(RealImaginary[0])と虚数部(RealImaginary[1])に分解
    // cv::split(cxMatrix, RealImaginary);
    // //実数部の最大値と最小値を取得
    // double min, max; //配列の最小値と最大値の格納用
    // cv::minMaxLoc(RealImaginary[0], &min, &max);
    // //実数部を resultImage にコピー
    // resultImage = cv::Mat(RealImaginary[0] * 1.0 / max);
    // // resultImage を 0〜255 にスケーリング
    // resultImage.convertTo(resultImage, CV_8U, 255);

    cv::Mat resultImage = sourceImage.clone();

    cv::threshold(sourceImage, resultImage, 20, 255, cv::THRESH_BINARY);

    //⑦各画像を表示
    //原画像
    cv::imshow("sourceImage", sourceImage);
    //フーリエスペクトル画像
    // cv::imshow("spcImage", spcImage);
    //逆変換画像
    cv::imshow("Result", resultImage);

    rec << resultImage;

    //⑧キー入力されたら，ウィンドウと画像格納用領域を破棄して終了
    char key = cv::waitKey(1);

    switch (key) {
    case 'q':
      isContinue = false;
      break;
    case 'w':
      sigma += 0.01;
      if (sigma > 1.0) {
        sigma = 1.0;
      }
      printf("%lf\n", sigma);
      break;
    case 'd':
      sigma -= 0.01;
      if (sigma < 0.0) {
        sigma = 0.0;
      }
      printf("%lf\n", sigma);

      break;
    default:
      break;
    }
  }

  return 0;
}

//画像の象限入れ替え用関数
void ShiftDFT(const cv::Mat &src_arr, cv::Mat &dst_arr) {
  int cx = src_arr.cols / 2;
  int cy = src_arr.rows / 2;

  cv::Mat q1(src_arr, cv::Rect(cx, 0, cx, cy));
  cv::Mat q2(src_arr, cv::Rect(0, 0, cx, cy));
  cv::Mat q3(src_arr, cv::Rect(0, cy, cx, cy));
  cv::Mat q4(src_arr, cv::Rect(cx, cy, cx, cy));

  cv::Mat tmp;
  q1.copyTo(tmp);
  q3.copyTo(q1);
  tmp.copyTo(q3);

  q2.copyTo(tmp);
  q4.copyTo(q2);
  tmp.copyTo(q4);
}
