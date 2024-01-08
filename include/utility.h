#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

struct RecResult
{
    std::string text;
    float score = -1.0;
};

class Utility
{
public:
    static std::vector<std::string> ReadDict(const std::string &path);

    template <class ForwardIterator>
    inline static size_t argmax(ForwardIterator first, ForwardIterator last)
    {
        return std::distance(first, std::max_element(first, last));
    }

    static std::vector<int> argsort(const std::vector<float> &array);

    static void print_result(const std::vector<RecResult> &ocr_result);
};