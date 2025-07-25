#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

// <===> Color Detection <===>

int main(int argc, char** argv) {
    std::cout << "Running the program...\n";

    std::string path = "Resources/lambo.png";
    Mat img = imread(path);
    if (img.empty()) {
        std::cerr << "ERROR: Could not find the image!\n";
        return -1;
    }

    int hmin = 0, smin = 110, vmin = 153, hmax = 19, smax = 240, vmax = 255;

    Mat imgHSV;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);
    Mat mask;

    namedWindow("Trackbars", (640, 200));
    // trackbars
    createTrackbar("Hue Min", "Trackbars", &hmin, 179);
    createTrackbar("Hue Max", "Trackbars", &hmax, 179);
    createTrackbar("Sat Min", "Trackbars", &smin, 255);
    createTrackbar("Sat Max", "Trackbars", &smin, 255);
    createTrackbar("Val Min", "Trackbars", &vmin, 255);
    createTrackbar("Val Max", "Trackbars", &vmin, 255);

    while (true) {
        Scalar lowerLimit(hmin, smin, vmin);
        Scalar upperLimit(hmax, smax, vmax);
        inRange(imgHSV, lowerLimit, upperLimit, mask);

        imshow("Lambo Image", img);
        imshow("HSV Lambo Image", imgHSV);
        imshow("Lambo Image Mask", mask);

        if (waitKey(1) == 'q') {
            std::cout << "Exiting the loop...\n";
            break;
        }
    }
    return 0;
}
