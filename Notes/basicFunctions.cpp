#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

// <===> Basic functions <===>

using namespace cv;

int main(int argc, char **argv) {
    std::cout << "Running the program...\n";

    std::string path = "Resources/lambo.png";
    Mat img = imread(path);
    imshow("Image", img);

    // converting to specific colour
    Mat imgGray;
    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    imshow("Image Gray", imgGray);

    // bluring the Image
    Mat imgBlur;
    GaussianBlur(img, imgBlur, Size(7, 7), 5, 0);
    imshow("Image Blur", imgBlur);

    // finding esdges in an image using the Canny algorithm
    // it's gonna be a black-white image with white edges and black background initially.
    Mat imgCanny;
    Canny(img, imgCanny, 50, 150);
    imshow("Image Canny", imgCanny);

    // Dilating an image
    Mat imgDilate;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(imgCanny, imgDilate, kernel);
    imshow("Image Dilation", imgDilate);

    // Eroding an image
    Mat imgErode;
    erode(imgDilate, imgErode, kernel);
    imshow("Image Erodion", imgErode);

    waitKey(0);

    return 0;
}

