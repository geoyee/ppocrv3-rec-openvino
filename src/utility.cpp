#include <iostream>
#include <fstream>
#include <vector>
#include "utility.h"

std::vector<std::string> Utility::ReadDict(const std::string &path)
{
    std::ifstream in(path);
    std::string line;
    std::vector<std::string> m_vec;
    if (in)
    {
        while (getline(in, line))
            m_vec.push_back(line);
    }
    else
    {
        std::cout << "no such label file: " << path << ", exit the program..."
                  << std::endl;
        exit(1);
    }
    return m_vec;
}

std::vector<int> Utility::argsort(const std::vector<float> &array)
{
    const int array_len(array.size());
    std::vector<int> array_index(array_len, 0);
    for (int i = 0; i < array_len; ++i)
        array_index[i] = i;
    std::sort(
        array_index.begin(), array_index.end(),
        [&array](int pos1, int pos2)
        { return (array[pos1] < array[pos2]); });

    return array_index;
}

void Utility::print_result(const std::vector<RecResult> &ocr_result)
{
    for (int i = 0; i < ocr_result.size(); ++i)
    {
        if (ocr_result[i].score != -1.0)
            std::cout << "rec text: " << ocr_result[i].text
                      << " rec score: " << ocr_result[i].score << " ";
        std::cout << std::endl;
    }
}