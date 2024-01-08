#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "include/ocr_rec.h"

int main(int argc, char **argv)
{
    cv::Mat src_img = cv::imread("./test.jpg");
    Recognizer ppocrv3_rec("./ch_PP-OCRv3_rec_infer.onnx", "./ppocr_keys_v1.txt");
    std::vector<RecResult> rec_result = ppocrv3_rec.Run({src_img});
    Utility::print_result(rec_result);
    return 0;
}