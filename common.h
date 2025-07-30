#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <format>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

Mat preprocessingFrame(const Mat& frame); // function to configure the frame
Mat parkingLotLinesDetection(const Mat& frame);
// void spawningParkingLot(int event, int x, int y, int flags, void* userdata);
int checkLots(const Mat& frame);
double calculateWhiteRatio(const cv::Mat& binaryImage);

// During the calibration, change the width and the height that determine the parking lot size!
#define WIDTH 60 // this width suits the training example
#define HEIGHT 26 // this height suits the training example
#define EXTRA_HEIGHT 50 // extra height for the gui to add a text there.

// For calibration, this vector is used to store all rectangle's points
// std::vector<Point> clickedPoints;
Mat baseImage;

// All 90 parking lots for the calibrated car parking
const Point clickedPoints[] = {
    {233, 57},
    {28, 2},
    {88, 2},
    {25, 31},
    {89, 31},
    {26, 60},
    {88, 62},
    {88, 92},
    {23, 90},
    {24, 121},
    {89, 122},
    {89, 151},
    {23, 151},
    {23, 180},
    {84, 180},
    {85, 210},
    {22, 210},
    {20, 239},
    {20, 271},
    {85, 241},
    {84, 271},
    {20, 301},
    {83, 301},
    {83, 332},
    {82, 364},
    {18, 332},
    {17, 364},
    {16, 395},
    {81, 396},
    {16, 426},
    {80, 428},
    {225, 428},
    {290, 428},
    {290, 396},
    {291, 364},
    {291, 336},
    {225, 365},
    {226, 396},
    {291, 303},
    {291, 270},
    {291, 239},
    {226, 241},
    {227, 272},
    {227, 303},
    {226, 334},
    {291, 122},
    {292, 151},
    {292, 181},
    {293, 211},
    {227, 211},
    {227, 180},
    {227, 151},
    {228, 121},
    {228, 32},
    {292, 33},
    {292, 63},
    {293, 93},
    {227, 62},
    {227, 91},
    {228, 2},
    {291, 3},
    {427, 5},
    {493, 6},
    {492, 35},
    {493, 63},
    {428, 34},
    {430, 64},
    {430, 94},
    {493, 94},
    {429, 123},
    {492, 122},
    {492, 152},
    {430, 152},
    {429, 182},
    {493, 182},
    {495, 213},
    {430, 211},
    {431, 240},
    {496, 242},
    {496, 272},
    {496, 302},
    {498, 334},
    {432, 272},
    {432, 303},
    {433, 334},
    {434, 365},
    {434, 396},
    {436, 427},
    {499, 366},
    {500, 395},
    {500, 428}
};


#endif