#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

using namespace cv;

int main(int argc, char **argv) {
    std::cout << "Running the program...d\n";

    VideoCapture video;

    if (argc == 1) {
        video.open(0);
    } else if (argc == 2) {
        video.open(argv[1]); // if user add the video file in the param list
    } else {
        std::cerr << "Too many arguments!\n";
        return -1;
    }

    if (!video.isOpened()) {
        std::cerr << "Could not open video source!\n";
        return -1;
    }

    CascadeClassifier facedetect;
    Mat img;
    facedetect.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml");

    while (true) {
        video.read(img);
        
        std::vector<Rect> faces;
        facedetect.detectMultiScale(img, faces, 1.3, 5);

        std::cout << faces.size() << '\n';

        for (size_t i = 0; i < faces.size(); i++) {
            rectangle(img, faces[i].tl(), faces[i].br(), Scalar(50, 50, 255), 3);
        }

        imshow("Frame", img);
        if (waitKey(1) == 'q') {
            std::cout << "Exiting the program...\n";
            break;
        }
    }

    return 0;
}
