#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

// <===> Warp Perspective <===>

int main(int argc, char** argv) {
    std::cout << "Running the program...\n";

    std::string path = "Resources/cards.jpg";
    Mat img = imread(path);
    if (img.empty()) {
        std::cerr << "ERROR: Could not find the image!\n";
        return -1;
    }

    const float w = 250, h = 350;

    const Point2f src[4] = { {529, 142}, {771, 190}, {405, 395}, {674, 457} };
    Point2f dest[4] = { {0.0f, 0.0f}, {w, 0.0f}, {0.0f, h}, {w, h} };

    // transformation matrix
    Mat matrix, imgWarp;
    matrix = getPerspectiveTransform(src, dest);
    warpPerspective(img, imgWarp, matrix, Point(w, h));
    
    // plotting the pointes
    for (int i = 0; i < 4; i++) {
        circle(img, src[i], 10, Scalar(0, 69, 255), FILLED);
        circle(img, src[i], 2, Scalar(0, 0, 0), FILLED);
    }

    imshow("Cards", img);
    imshow("Corresponding Card", imgWarp);

    waitKey(0);

    return 0;
}

