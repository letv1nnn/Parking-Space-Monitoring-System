#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#define DEBUG 0

using namespace cv;

std::vector<std::vector<int>> myColors{
    {124, 48, 117, 143, 170, 255},
    {68, 72, 156, 102, 126, 255}
};

std::vector<Scalar> myColorValues{
    {255, 0, 255}, 
    {0, 255, 0}
};

Mat frame;
std::vector<std::vector<int>> newPoints;

std::vector<std::vector<int>> findColor(Mat frame);
Point getContours(Mat frame);
void canvasDrawing(const std::vector<std::vector<int>>& points, const std::vector<Scalar>& colorValues);

int main(int argc, char** argv) {
    std::cout << "Starting the program...\n";

    VideoCapture camera(0);
    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not launch web camera!\n";
        return -1;
    }

    while (true) {
        if (!camera.read(frame) || frame.empty()) {
            std::cerr << "ERROR: Could not read frame!\n";
            break;
        }

        std::vector<std::vector<int>> newPointsInFrame = findColor(frame);

        newPoints.insert(newPoints.end(), newPointsInFrame.begin(), newPointsInFrame.end());

        canvasDrawing(newPoints, myColorValues);

        imshow("Web Camera", frame);

        if (waitKey(1) == 'q') {
            std::cout << "Exiting the program...\n";
            break;
        }
    }

    return 0;
}

std::vector<std::vector<int>> findColor(Mat frame) {
    Mat frameHSV;
    cvtColor(frame, frameHSV, COLOR_BGR2HSV);

    std::vector<std::vector<int>> localNewPoints;

    for (int i = 0; i < myColors.size(); i++) {
        Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
        Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);

        Mat mask;
        inRange(frameHSV, lower, upper, mask);

#if DEBUG
        imshow("Mask " + std::to_string(i), mask);
#endif

        Point myPoint = getContours(mask);
        if (myPoint.x != 0 || myPoint.y != 0) {
            localNewPoints.push_back({myPoint.x, myPoint.y, i});
        }
    }

    return localNewPoints;
}

Point getContours(Mat frame) {
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;

    findContours(frame, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<Point>> conPoly(contours.size());
    std::vector<Rect> boundRect(contours.size());

    Point myPoint(0, 0);

    for (int i = 0; i < contours.size(); i++) {
        int area = contourArea(contours[i]);

        if (area > 1000) {
            float perim = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * perim, true);

            boundRect[i] = boundingRect(conPoly[i]);

            myPoint.x = boundRect[i].x + boundRect[i].width / 2;
            myPoint.y = boundRect[i].y;

#if DEBUG
            std::cout << "Area: " << area << ", Vertices: " << conPoly[i].size() << '\n';
#endif
        }
    }

    return myPoint;
}

void canvasDrawing(const std::vector<std::vector<int>>& points, const std::vector<Scalar>& colorValues) {
    for (int i = 0; i < points.size(); i++) {
        Point center(points[i][0], points[i][1]);
        Scalar color = colorValues[points[i][2]];
        circle(frame, center, 10, color, FILLED);
    }
}

