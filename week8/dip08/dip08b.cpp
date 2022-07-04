#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, char *argv[]) {
  //ビデオキャプチャを初期化して，映像を取り込む
  cv::VideoCapture capture(0); //指定したビデオファイルをオープン
  //ビデオファイルがオープンできたかどうかをチェック
  if (capture.isOpened() == 0) {
    printf("Camera not found\n");
    return -1;
  }

  //画像格納用インスタンス準備
  int width =
      capture.get(cv::CAP_PROP_FRAME_WIDTH); //ビデオファイルのフレーム幅
  int height =
      capture.get(cv::CAP_PROP_FRAME_HEIGHT); //ビデオファイルのフレーム高

  width = 720;
  height = 480;
  printf("(w, h) = (%d, %d)\n", width, height);
  cv::Size imageSize(width, height); //フレームと同じ画像サイズ定義
  cv::Mat frameImage = cv::Mat(imageSize, CV_8UC3); //フレーム画像用
  cv::Mat originalImage = cv::Mat(imageSize, CV_8UC3);
  cv::Mat translateImage = cv::Mat(imageSize, CV_8UC3);

  cv::Mat maskImage = cv::Mat(imageSize, CV_8UC1);

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (j < width / 2) {
        maskImage.at<uchar>(i, j) = 0;
      } else {
        maskImage.at<uchar>(i, j) = 255;
      }
    }
  }

  //オプティカルフロー準備
  cv::TermCriteria criteria = cv::TermCriteria(
      cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.01); //終了条件
  cv::Mat presentImage(imageSize, CV_8UC1),
      priorImage(imageSize, CV_8UC1); //現フレーム濃淡画像，前フレーム濃淡画像
  std::vector<cv::Point2f> presentFeature,
      priorFeature; //現フレーム対応点，前フレーム追跡点
  std::vector<unsigned char> status; //処理用
  std::vector<float> errors;         //処理用

  //船の初期位置
  cv::Point2f shipPoint(130, 190);

  //画像表示用ウィンドウの生成
  cv::namedWindow("Frame");
  cv::moveWindow("Frame", 0, 50);

  //動画像処理無限ループ
  while (1) {
    //ビデオキャプチャ"capture"から1フレームを取り込んで，"frameImage"に格納
    capture >> originalImage;
    cv::resize(originalImage, frameImage, frameImage.size());

    cv::Point2f original[4], translate[4];
    original[0] = cv::Point2f(0, 0);
    original[1] = cv::Point2f(0, height);
    original[2] = cv::Point2f(width, height);
    original[3] = cv::Point2f(width, 0);

    translate[0] = cv::Point2f(width, 0);
    translate[1] = cv::Point2f(width, height);
    translate[2] = cv::Point2f(0, height);
    translate[3] = cv::Point2f(0, 0);

    //ビデオが終了したら無限ループから脱出
    if (frameImage.data == NULL)
      break;

    cv::Mat persMat = cv::getPerspectiveTransform(original, translate);
    cv::warpPerspective(frameImage, translateImage, persMat,
                        translateImage.size(), cv::INTER_LINEAR,
                        cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

    translateImage.copyTo(frameImage, maskImage);

    //"frameImage"から現フレーム濃淡画像"presentImage"を生成
    // cv::cvtColor(frameImage, presentImage, cv::COLOR_BGR2GRAY);

    //前フレーム追跡点"priorFeature"生成
    // cv::goodFeaturesToTrack(priorImage, priorFeature, 1000, 0.01, 1);
    // priorFeature.clear();  //"priorFeature"リセット
    // priorFeature.push_back(cv::Point2f(width/2, height/2));
    // //"priorFeature"に点を追加(画像中心)

    int opCnt = priorFeature.size(); //"priorFeature"に含まれる追跡点の個数

    if (opCnt > 0) { //追跡点が存在する場合
      //前フレーム濃淡画像"priorImage"，現フレーム濃淡画像"presentImage"を用いて
      //前フレーム追跡点"priorFeature"に対応する現フレーム点"presentFeature"を取得
      cv::calcOpticalFlowPyrLK(priorImage, presentImage, priorFeature,
                               presentFeature, status, errors, cv::Size(10, 10),
                               4, criteria);
      //オプティカルフロー描画
      for (int i = 0; i < opCnt; i++) {
        cv::Point pt1 = cv::Point(priorFeature[i]);   //前フレーム追跡点
        cv::Point pt2 = cv::Point(presentFeature[i]); //現フレーム対応点
        cv::line(frameImage, pt1, pt2, cv::Scalar(0, 255, 0), 3,
                 8); // pt1とpt2を結ぶ直線を描画
      }
    }

    //前フレーム濃淡画像"priorImage"を現フレーム濃淡画像"presentImage"で更新
    presentImage.copyTo(priorImage);

    //船の位置に円を描画
    // cv::circle(frameImage, shipPoint, 10, cv::Scalar(0, 255, 255), -1);

    //ウィンドウに画像表示
    cv::imshow("Frame", frameImage);
    cv::imshow("Translate", translateImage);
    cv::imshow("Mask", maskImage);

    //[q]キーが押されたら無限ループから脱出
    int key = cv::waitKey(1);
    if (key == 'q')
      break;
  }

  //メッセージを出力して終了
  printf("Finished\n");
  return 0;
}
