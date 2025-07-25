#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

int displayImage(std::string path);
int displayVideo(std::string path);
int displayWebcamera();

int main(int argc, char **argv) {
    // int imgRes = displayImage("Resources/cards.jpg"); 
    // int videoRes = displayVideo("Resources/test_video.mp4");
    int resWebcam = displayWebcamera();

    return 0;
}

int displayImage(std::string path) {
    std::cout << "Opening the image...\n";
    Mat img = imread(path);
    if (img.empty()) {
        std::cerr << "ERROR: Could not load image!\n";
        return -1;
    }
    imshow("Image", img);
    waitKey(0);

    return 0;
}

int displayVideo(std::string path) {
    std::cout << "Opening the video...\n";
    VideoCapture capture(path);
    if (!capture.isOpened()) {
        std::cerr << "ERROR: Could not load video!\n";
        return -1;
    }
    Mat img;
    while (true) {
        capture.read(img);
        imshow("Image", img);
        waitKey(20);
    }

    return 0;
}

int displayWebcamera() { 
    std::cout << "Opening the webcamera...\n";
    VideoCapture capture(0);
    Mat img;
    while (true) {
        capture.read(img);
        imshow("Image", img);
        if (waitKey(1) == 'q') {
            std::cout << "Shutting down the webcam\nBye!\n";
            break;
        }
    }
    
    return 0;
}

