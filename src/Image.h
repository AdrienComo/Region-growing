#ifndef TP_ANALYSE_IMAGE_H
#define TP_ANALYSE_IMAGE_H

#include <set>
#include <cmath>
#include <deque>
#include <string>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Fast unsigned int
using uif8  = uint_fast8_t;
using uif16 = uint_fast16_t;
using uif32 = uint_fast32_t;

class Image {
public :
    // Gets parameters from main.cpp
    Image(const uif16, const uif8, const uif8, const uif8, const uif8, const uif8, const uif8, const std::string, const std::string);

    // Initialize all arrayx, deque, size of image, cv::mat, opens image, creates random seeds..
    void init();

    // Main controller of the algorithm
    void kernel();

    // 4 connectivity Region Growing
    bool growConn4(const uif16);

    // Compares colors of pixels with region seed color
    bool simpleAbsDist(const uif16, const uif16);

    // Creates seeds at index of the image where no region have passed over this pixel
    void growShadows();

    // Fuses regions together applying random colors to regions
    void fusionSystem1();

    // Fuses regions together keeping average color of regions
    void fusionSystem2();

    // Draws the output image
    void draw();

    // Finds and draws contour
    void contour();

    // Clear arrays, deque... to avoid memory leaks
    void clear();

    // Writes image to ".png" format
    void write() const;

private :
    uif16 seed;
    uif8 blur;
    uif8 threshold, threshold2, fusionThreshold, min, max;
    std::string path;
    std::string output;

    cv::Mat img, img2; //img : image of regions, img2 : image of contours
    cv::Size imgS, imgS2, imgS3; // Different size for the image, flag array

    uif32 ** flag;

    std::deque<uif16> seedsCoord; //even (x), odd (y)
    std::deque<uif8> col; //0:r, 1:g, 2:b (increment by 3 to get rgb of region) 
    std::deque<uif16> process; // Coordinates of all pixel to process and apply Region Growing : even(x), odd(y)
    std::deque<std::set<uif32>> color; // Adjacent regions for each region

    // Kernel default parameters
    uif16 c, rI; // color index, region index
    uif8 r, g, b; // r,g,b per region
};

#endif //TP_ANALYSE_IMAGE_H