#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <format>

#define EXTRA_GUI_HEIGHT 60
// Change width and height to be relevant to your parking lot size.
#define WIDTH 43
#define HEIGHT 15
// Calibration flag, set to 1 for the first use, then 0.
#define CALIBRATE 0
// White Ratio Scaler, set it up so it would work appropriate for your parking
#define WHITE_RATIO 9.0

using namespace cv;

class PointsStorage {
private:
    std::vector<Point> points;
public:
    PointsStorage() = default;
    void writePoint(const Point pt);
    void readPoints(const std::string& filename);
    std::vector<Point> getPoints() const;
};

void displayParkingLot(int event, int x, int y, int flags, void* userdata);
Mat preprocessingFrame(const Mat& frame);
double calculateWhiteRatio(const Mat& binaryImage);
int checkLots(const Mat& frame, PointsStorage points);

#endif