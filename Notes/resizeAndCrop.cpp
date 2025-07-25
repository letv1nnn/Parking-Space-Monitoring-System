#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

// <===> Resize and Crop <===>

int main(int argc, char **argv) {
    std::cout << "Running the program...\n";

    std::string path = "Resources/test.jpg";
    Mat img = imread(path), imgResized;
    
    // resizeing an image
    std::cout << img.size() << '\n';
    //               you can define the size or write the scalers down.
    resize(img, imgResized, Size(), 0.5, 0.5);

    // crop an image
    Rect roi(100, 100, 300, 250);
    Mat imgCrop = img(roi);
    

    imshow("Image", img);
    imshow("Image Resized", imgResized);
    imshow("Image Cropped", imgCrop);

    waitKey(0);

    return 0;
}

