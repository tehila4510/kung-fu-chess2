#ifndef VIEW_IMG_H
#define VIEW_IMG_H

#include <opencv2/opencv.hpp>
#include <string>
#include <utility>

class Img {
public:
    Img();

    
    Img& read(const std::string& path,
              const std::pair<int, int>& size = {0, 0},
              bool keep_aspect = false,
              int interpolation = cv::INTER_AREA);

    void draw_on(Img& other_img, int x, int y);

    void put_text(const std::string& txt, int x, int y, double font_size,
                  const cv::Scalar& color = cv::Scalar(255, 255, 255, 255),
                  int thickness = 1);

    void show() const;

    const cv::Mat& get_mat() const;
    bool is_loaded() const;

private:
    cv::Mat img;
};

#endif
