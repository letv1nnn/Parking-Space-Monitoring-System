#include "common.h"


int checkLots(const Mat& frame, PointsStorage points) {
    int freeSpace = 0;

    for (const auto& pt : points.getPoints()) {
        Rect roi(pt.x, pt.y, WIDTH, HEIGHT);
        Mat lot = frame(roi);

        Mat preprocessedLot = preprocessingFrame(lot);
        double whiteRatio = calculateWhiteRatio(preprocessedLot);
        std::string ratio = std::format("{}", std::round(whiteRatio * 100.0) / 100.0);

        // I need to determine whether the given parking lot is free and then draw a rectangle with appropriate color.
        if (whiteRatio > WHITE_RATIO) {
            rectangle(frame, pt, Point(pt.x + WIDTH, pt.y + HEIGHT), Scalar(0, 0, 255), 2);
            putText(frame, ratio, Point(pt.x + 2, pt.y + 10), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 0, 255), 1);
        } else {
            rectangle(frame, pt, Point(pt.x + WIDTH, pt.y + HEIGHT), Scalar(0, 255, 0), 2);
            freeSpace++;
            putText(frame, ratio, Point(pt.x + 2, pt.y + 10), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 255, 0), 1);
        }
    }

    return freeSpace;
}

Mat preprocessingFrame(const Mat& frame) {
    Mat preprocessedFrame = frame.clone();

    cvtColor(frame, preprocessedFrame, COLOR_BGR2GRAY);
    GaussianBlur(preprocessedFrame, preprocessedFrame, Size(3, 3), 0);
    Canny(preprocessedFrame, preprocessedFrame, 50, 150, 3);

    return preprocessedFrame;
}

double calculateWhiteRatio(const Mat& binaryImage) {
    CV_Assert(binaryImage.type() == CV_8UC1);

    int totalPixels = binaryImage.rows * binaryImage.cols;
    int whitePixels = countNonZero(binaryImage);

    double whiteRatio = static_cast<double>(whitePixels) / totalPixels;
    return whiteRatio * 100.0; // return as percentage
}

