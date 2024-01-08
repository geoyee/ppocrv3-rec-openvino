#include "ocr_rec.h"

Recognizer::Recognizer(std::string model_path, const std::string &label_path)
{
    ov::Core core;
    model_path_ = model_path;
    model_ = core.read_model(model_path_);
    // reshape the model for dynamic batch size and sentence width
    model_->reshape({{ov::Dimension(1, 6), rec_image_shape_[0], rec_image_shape_[1], -1}});
    compiled_model_ = core.compile_model(model_, "CPU");
    infer_request_ = compiled_model_.create_infer_request();
    label_list_ = Utility::ReadDict(label_path);
    label_list_.insert(label_list_.begin(), "#"); // blank char for ctc
    label_list_.push_back(" ");
}

std::vector<RecResult> Recognizer::Run(std::vector<cv::Mat> image_list)
{
    std::vector<std::string> rec_texts(image_list.size(), "");
    std::vector<float> rec_text_scores(image_list.size(), 0);
    int img_num = image_list.size();
    std::vector<float> width_list;
    for (int i = 0; i < img_num; ++i)
        width_list.push_back(float(image_list[i].cols) / image_list[i].rows);
    std::vector<int> indices = Utility::argsort(width_list);
    for (int beg_img_no = 0; beg_img_no < img_num; beg_img_no += rec_batch_num_)
    {
        int end_img_no = std::min(img_num, beg_img_no + rec_batch_num_);
        int batch_num = end_img_no - beg_img_no;
        int imgH = rec_image_shape_[1];
        int imgW = rec_image_shape_[2];
        float max_wh_ratio = imgW * 1.0 / imgH;
        for (int ino = beg_img_no; ino < end_img_no; ++ino)
        {
            int h = image_list[indices[ino]].rows;
            int w = image_list[indices[ino]].cols;
            float wh_ratio = w * 1.0 / h;
            max_wh_ratio = std::max(max_wh_ratio, wh_ratio);
        }
        int batch_width = imgW;
        std::vector<cv::Mat> norm_img_batch;
        for (int ino = beg_img_no; ino < end_img_no; ++ino)
        {
            cv::Mat srcimg;
            image_list[indices[ino]].copyTo(srcimg);
            cv::Mat resize_img;
            // preprocess
            resize_op_.Run(srcimg, resize_img, max_wh_ratio, rec_image_shape_);
            normalize_op_.Run(&resize_img, mean_, scale_, is_scale_);
            norm_img_batch.push_back(resize_img);
            batch_width = std::max(resize_img.cols, batch_width);
        }
        // prepare input tensor
        std::vector<float> input(batch_num * 3 * imgH * batch_width, 0.0f);
        ov::Shape intput_shape = {batch_num, 3, imgH, batch_width};
        permute_op_.Run(norm_img_batch, input.data());
        auto input_port = compiled_model_.input();
        ov::Tensor input_tensor(input_port.get_element_type(), intput_shape, input.data());
        infer_request_.set_input_tensor(input_tensor);
        // start inference
        infer_request_.infer();
        auto output = infer_request_.get_output_tensor();
        const float *out_data = output.data<const float>();
        auto predict_shape = output.get_shape();
        // predict_batch is the result of Last FC with softmax
        for (int m = 0; m < predict_shape[0]; ++m)
        {
            std::string str_res;
            int argmax_idx;
            int last_index = 0;
            float score = 0.f;
            int count = 0;
            float max_value = 0.0f;
            for (int n = 0; n < predict_shape[1]; ++n)
            {
                // get idx
                argmax_idx = int(Utility::argmax(
                    &out_data[(m * predict_shape[1] + n) * predict_shape[2]],
                    &out_data[(m * predict_shape[1] + n + 1) * predict_shape[2]]));
                // get score
                max_value = float(*std::max_element(
                    &out_data[(m * predict_shape[1] + n) * predict_shape[2]],
                    &out_data[(m * predict_shape[1] + n + 1) * predict_shape[2]]));

                if (argmax_idx > 0 && (!(n > 0 && argmax_idx == last_index)))
                {
                    score += max_value;
                    count += 1;
                    str_res += label_list_[argmax_idx];
                }
                last_index = argmax_idx;
            }
            score /= count;
            if (std::isnan(score))
                continue;
            rec_texts[indices[beg_img_no + m]] = str_res;
            rec_text_scores[indices[beg_img_no + m]] = score;
        }
    }
    // sort boex from top to bottom, from left to right
    std::vector<RecResult> ocr_results;
    for (int i = 0; i < rec_texts.size(); ++i)
    {
        RecResult rr;
        rr.text = rec_texts[i];
        rr.score = rec_text_scores[i];
        ocr_results.emplace_back(rr);
    }
    return ocr_results;
}