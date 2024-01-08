#pragma once

#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

class Normalize
{
public:
    virtual void Run(cv::Mat *img,
                     const std::vector<float> &mean,
                     const std::vector<float> &scale,
                     const bool is_scale = true);
};

class PermuteBatch
{
public:
    virtual void Run(const std::vector<cv::Mat> imgs, float *data);
};

class CrnnResizeImg
{
public:
    virtual void Run(const cv::Mat &img,
                     cv::Mat &resize_img,
                     float wh_ratio,
                     const std::vector<int> &rec_image_shape = {3, 32, 320});
};