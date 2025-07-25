#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

// <===> Drawing Shapes and Text <===>

int main(int argc, char** argv) {

    std::cout << "Running the program...\n";

    Mat img(512, 512, CV_8UC3, Scalar(255, 255, 255));
    circle(img, Point(256, 256), 155, Scalar(0, 69, 255), FILLED);
    rectangle(img, Point(130, 226), Point(382, 286), Scalar(255, 255, 255), FILLED);
    // line() - method to draw a line
    putText(img, "STOP", Point(190, 275), FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 0), 4);

    imshow("Image", img);

    waitKey(0);

    return 0;
}

