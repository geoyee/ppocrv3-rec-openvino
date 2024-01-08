#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <openvino/openvino.hpp>
#include "utility.h"
#include "preprocess_op.h"

class Recognizer
{
public:
    explicit Recognizer(std::string model_path, const std::string &label_path);
    std::vector<RecResult> Run(std::vector<cv::Mat> image_list);

private:
    ov::InferRequest infer_request_;
    std::string model_path_;
    std::shared_ptr<ov::Model> model_;
    ov::CompiledModel compiled_model_;

    std::vector<float> mean_ = {0.5f, 0.5f, 0.5f};
    std::vector<float> scale_ = {1 / 0.5f, 1 / 0.5f, 1 / 0.5f};
    bool is_scale_ = true;
    std::vector<std::string> label_list_;
    int rec_batch_num_ = 6;
    int rec_img_h_ = 48;
    int rec_img_w_ = 320;
    std::vector<int> rec_image_shape_ = {3, rec_img_h_, rec_img_w_};

    CrnnResizeImg resize_op_;
    Normalize normalize_op_;
    PermuteBatch permute_op_;
};