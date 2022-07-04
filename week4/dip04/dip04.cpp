//(OpenCV4) g++ -std=c++11 dip04.cpp `pkg-config --cflags --libs opencv4`
//(OpenCV3) g++ dip04.cpp `pkg-config --cflags --libs opencv`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, const char *argv[]) {
  int width = 720, height = 405; //処理時の画像サイズ

  //①ビデオキャプチャの初期化
  cv::VideoCapture capture(0); //
  //ビデオキャプチャがオープンできたかどうかをチェック
  if (capture.isOpened() == 0) {
    printf("Camera not found\n");
    return -1;
  }

  //②画像格納用インスタンス準備
  cv::Mat captureImage;
  cv::Mat frameImage(cv::Size(width, height), CV_8UC3);   // 3チャンネル
  cv::Mat grayImage(cv::Size(width, height), CV_8UC1);    // 1チャンネル
  cv::Mat resultGImage(cv::Size(width, height), CV_8UC1); // 1チャンネル
  cv::Mat recImage(cv::Size(width, height), CV_8UC3);     // 3チャンネル
  cv::Mat hsvImage;
  cv::Mat resultHSVImage(cv::Size(width, height), CV_8UC3);

  //③ウィンドウの生成と移動
  cv::namedWindow("Frame");
  cv::moveWindow("Frame", 0, 0);
  cv::namedWindow("Gray");
  cv::moveWindow("Gray", width, 0);
  cv::namedWindow("Result");
  cv::moveWindow("Result", width, height);

  //④ルックアップテーブルの作成

  unsigned char lookupTable[256];
  for (int i = 0; i < 256; i++) {
    lookupTable[i] = 255 - i;
  }

  int thresh = 128;
  unsigned char binTable[256];
  for (int i = 0; i < 256; i++) {
    if (thresh < i) {
      binTable[i] = 255;
    } else {
      binTable[i] = 0;
    }
  }

  unsigned char contrastTable[256];
  for (int i = 0; i < 256; i++) {
    if (i < 50) {
      contrastTable[i] = 0;
    } else if (i > 150) {
      contrastTable[i] = 255;
    } else {
      contrastTable[i] = 2.55 * i - 127.5;
    }
  }

  unsigned char postTable[256];
  for (int i = 0; i < 256; i++) {
    if (i <= 32) {
      postTable[i] = 0;
    } else if (i <= 64) {
      postTable[i] = 36;
    } else if (i <= 96) {
      postTable[i] = 72;
    } else if (i <= 128) {
      postTable[i] = 108;
    } else if (i <= 160) {
      postTable[i] = 144;
    } else if (i <= 192) {
      postTable[i] = 180;
    } else if (i <= 224) {
      postTable[i] = 226;
    } else if (i <= 255) {
      postTable[i] = 255;
    }
  }

  // unsigned char colorPostTable[180];
  cv::Vec3b colorPostTable[256];
  int level_h = 6;
  int level_sv = 3;
  int count_h = 0;
  int count_sv = 0;

  for (int i = 0; i < 256; i++) {

    colorPostTable[1][i] = (256 / level_sv) * count_sv;
    colorPostTable[2][i] = (256 / level_sv) * count_sv;
    if (i % (256 / level_sv) == 0) {
      count_sv++;
    }

    if (i > 180) {
      continue;
    } else if (i % (180 / level_h) == 0) {
      count_h++;
    }
    colorPostTable[0][i] = (180 / level_h) * count_h;

    // colorPostTable[0][i] = i;
    // colorPostTable[1][i] = i;
    // colorPostTable[2][i] = i;
  }

  //⑤ビデオライタ生成(ファイル名，コーデック，フレームレート，フレームサイズ)
  cv::VideoWriter rec("rec.mpg", cv::VideoWriter::fourcc('P', 'I', 'M', '1'),
                      30, recImage.size());

  int processFlag = 5;
  //⑥動画像処理無限ループ
  while (1) {
    //(a)カメラから1フレームを"captureImage"に読み込み
    capture >> captureImage;
    if (captureImage.data == NULL)
      break;

    //"captureImage"をリサイズして"frameImage"に格納
    cv::resize(captureImage, frameImage, frameImage.size());

    //(b)"frameImage"をグレースケールに変換して"grayImage"に格納
    cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);
    cv::cvtColor(frameImage, hsvImage, cv::COLOR_BGR2HSV);

    //(c)"grayImage"の各画像を走査して，ルックアップテーブルに基づいて画素値変換して"resultGImage"に格納
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        // unsigned char s = grayImage.at<unsigned char>(i, j);
        cv::Vec3b s = hsvImage.at<cv::Vec3b>(i, j);
        unsigned char t;

        // switch (processFlag) {
        // case 0:
        //   t = s;
        //   break;
        // case 1:
        //   t = lookupTable[s];
        //   break;
        // case 2:
        //   t = binTable[s];
        //   break;
        // case 3:
        //   t = contrastTable[s];
        //   break;
        // case 4:
        //   t = lookupTable[s];
        //   t = binTable[t];
        //   break;
        // case 5:
        //   t = postTable[s];
        //   break;
        // default:
        //   break;
        // }

        // if (colorPostTable[0][s[0]] != s[0]) {

        //   printf("%d %d\n", colorPostTable[0][s[0]], s[0]);
        // }

        // resultGImage.at<unsigned char>(i, j) = t;
        resultHSVImage.at<cv::Vec3b>(i, j)[0] = colorPostTable[0][s[0]];
        resultHSVImage.at<cv::Vec3b>(i, j)[1] = colorPostTable[1][s[1]];
        resultHSVImage.at<cv::Vec3b>(i, j)[2] = colorPostTable[2][s[2]];
      }
    }

    //(d)ウィンドウへの画像の表示
    cv::imshow("Frame", frameImage);
    cv::imshow("Gray", grayImage);
    cv::cvtColor(resultHSVImage, resultHSVImage, cv::COLOR_HSV2BGR);
    cv::imshow("Result", resultHSVImage);

    //(e)動画ファイル書き出し
    // cv::cvtColor(resultGImage, recImage, cv::COLOR_GRAY2BGR);
    // //動画用3チャンネル画像生成 rec << recImage;  //ビデオライタに画像出力
    rec << resultHSVImage;

    //(f)キー入力待ち
    char key = cv::waitKey(30); // 30ミリ秒待機
    if (key == 'q') {
      break;
    }

    if (key >= '0' && key <= '9') {
      processFlag = key - '0';
    }
  }

  return 0;
}
