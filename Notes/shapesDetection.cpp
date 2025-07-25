#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

void getContours(Mat imgDil, Mat img) {
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    std::vector<std::vector<Point>> conPoly(contours.size());
   
    for (int i = 0; i < contours.size(); i++) {
        int area = contourArea(contours[i]);
        std::cout << "Area: " << area;
        if (area > 1000) {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
            drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
            std::cout << ", Angles: " << conPoly[i].size() << "  ===>  Approved\n";
        } else 
            std::cout << '\n';
    } 
}

int main(int argc, char** argv) {
    std::cout << "Running the program...\n";

    std::string path = "../Resources/shapes.png";
    Mat img = imread(path);
    Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

    if (img.empty()) {
        std::cerr << "ERROR: Could not find the file!\n";
        return -1;
    }

    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
    Canny(imgBlur, imgCanny, 25, 75);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(imgCanny, imgDil, kernel);

    getContours(imgDil, img);

    imshow("Shapes", img);

    if (waitKey(0) == 'q') {
        std::cout << "Exiting the program...\n";
        return 0;
    }

    return 0;
}

