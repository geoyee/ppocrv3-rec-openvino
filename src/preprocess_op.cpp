#include "preprocess_op.h"

void Normalize::Run(cv::Mat *img,
                    const std::vector<float> &mean,
                    const std::vector<float> &scale,
                    const bool is_scale)
{
    double e = 1.0;
    if (is_scale)
        e /= 255.0;
    (*img).convertTo(*img, CV_32FC3, e);
    std::vector<cv::Mat> bgr_channels(3);
    cv::split(*img, bgr_channels);
    for (auto i = 0; i < bgr_channels.size(); ++i)
        bgr_channels[i].convertTo(bgr_channels[i], CV_32FC1, 1.0 * scale[i], (0.0 - mean[i]) * scale[i]);
    cv::merge(bgr_channels, *img);
}

void PermuteBatch::Run(const std::vector<cv::Mat> imgs, float *data)
{
    for (int j = 0; j < imgs.size(); ++j)
    {
        int rh = imgs[j].rows;
        int rw = imgs[j].cols;
        int rc = imgs[j].channels();
        for (int i = 0; i < rc; ++i)
            cv::extractChannel(imgs[j], cv::Mat(rh, rw, CV_32FC1, data + (j * rc + i) * rh * rw), i);
    }
}

void CrnnResizeImg::Run(const cv::Mat &img,
                        cv::Mat &resize_img,
                        float wh_ratio,
                        const std::vector<int> &rec_image_shape)
{
    int imgC, imgH, imgW;
    imgC = rec_image_shape[0];
    imgH = rec_image_shape[1];
    imgW = rec_image_shape[2];
    imgW = int(imgH * wh_ratio);
    float ratio = float(img.cols) / float(img.rows);
    int resize_w, resize_h;
    if (ceilf(imgH * ratio) > imgW)
        resize_w = imgW;
    else
        resize_w = int(ceilf(imgH * ratio));
    cv::resize(img, resize_img, cv::Size(resize_w, imgH), 0.f, 0.f, cv::INTER_LINEAR);
    cv::copyMakeBorder(resize_img, resize_img, 0, 0, 0,
                       int(imgW - resize_img.cols), cv::BORDER_CONSTANT,
                       {127, 127, 127});
}