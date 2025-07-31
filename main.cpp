// +--------------------------------------------+
// |    Free Parking Slot Detection program     |
// | Primarily, it's going to detect free lots  |
// |   from images, then via camera support.    |
// +--------------------------------------------+

#include "common.h"

int main(int argc, char **argv) {
    std::cout << "Running the program...\n";

    std::string path;

    if (argc == 1) {
        path = "test/parking_lots_2.jpg";
        std::cout << "Using default path: " << path << '\n';
    } else if (argc == 2) {
        path = argv[1];
        std::cout << "Using path: " << path << '\n';
    } else {
        std::cout << "Too many arguments!\n";
    }

    const Mat frame = imread(path);
    if (frame.empty()) {
        std::cerr << "Failed to load image!\n";
        return -1;
    }

    const Mat lotsFrame = frame.clone();
    // you can replace the line above with the line below to look at the parking lots configured by preprocessing function
    // const Mat lotsFrame = parkingLotLinesDetection(frame);
    baseImage = lotsFrame.clone();
    Mat displayFrame = lotsFrame.clone();

    int freeParkingLots = checkLots(displayFrame);
    std::cout << freeParkingLots << '\n';

#if 0 // part from debug.cpp file.
    namedWindow("Parking Lots Detection Frame", WINDOW_AUTOSIZE);
    setMouseCallback("Parking Lots Detection Frame", spawningParkingLot, &displayFrame);

    putText(displayFrame, std::format("Free Slots: {}", freeParkingLots), Point(50, 20), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 40, 0), 2);
    imshow("Parking Lots Detection Frame", displayFrame);
#endif

    cv::Mat displayWithFooter(displayFrame.rows + EXTRA_HEIGHT, displayFrame.cols, displayFrame.type(), Scalar(0, 0, 0));

    displayFrame.copyTo(displayWithFooter(cv::Rect(0, 0, displayFrame.cols, displayFrame.rows)));

    std::string text = std::format("Free Car Parking Slots: {}", freeParkingLots);
    cv::putText(displayWithFooter, text, Point(20, displayFrame.rows + 35), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

    cv::imshow("Parking Lots Detection Frame", displayWithFooter);

    if (waitKey(0) == 'q') {
        std::cout << "Quitting...\n";
        return 0;
    }

    return 0;
}

int checkLots(const Mat& frame) {
    int parkingLotNumber = 1;
    int freeSpace = 0;

    for (const auto& pt : clickedPoints) {

        Rect roi(pt.x, pt.y, WIDTH, HEIGHT);
        Mat lot = frame(roi);

        Mat preprocessedLot = preprocessingFrame(lot);
#if 0
        imshow("Parking Lots Detection Frame", preprocessedLot);
        waitKey(0);
#endif
        double whiteRatio = calculateWhiteRatio(preprocessedLot);

        std::cout << "Parking space number: " << parkingLotNumber++ << ". White Ratio: " << whiteRatio << '\n';

        std::string ratio = std::format("Ratio: {}", std::round(whiteRatio * 100.0) / 100.0);

        // I need to determine whether the given parking lot is free and then draw a rectangle with appropriate color.
        if (whiteRatio > 5.5) {
            rectangle(frame, pt, Point(pt.x + WIDTH, pt.y + HEIGHT), Scalar(0, 0, 255), 2);
            putText(frame, ratio, Point(pt.x + 5, pt.y + 15), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 0, 255), 1);
        } else {
            rectangle(frame, pt, Point(pt.x + WIDTH, pt.y + HEIGHT), Scalar(0, 255, 0), 2);
            freeSpace++;
            putText(frame, ratio, Point(pt.x + 5, pt.y + 15), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 255, 0), 1);
        }
    }

    return freeSpace;
}

double calculateWhiteRatio(const Mat& binaryImage) {
    CV_Assert(binaryImage.type() == CV_8UC1);

    int totalPixels = binaryImage.rows * binaryImage.cols;
    int whitePixels = countNonZero(binaryImage); // Non-zero = white in binary

    double whiteRatio = static_cast<double>(whitePixels) / totalPixels;
    return whiteRatio * 100.0; // return as percentage
}

Mat parkingLotLinesDetection(const Mat& frame) {
    Mat preprocessedFrame = preprocessingFrame(frame);
    Mat colorFrame = frame.clone();

    std::vector<Vec4i> lines;
    HoughLinesP(preprocessedFrame, lines, 1, CV_PI / 180, 50, 30, 5);

    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];
        line(colorFrame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 50, 100), 2, LINE_AA);
    }

    return colorFrame;
}

Mat preprocessingFrame(const Mat& frame) {
    Mat preprocessedFrame;

    cvtColor(frame, preprocessedFrame, cv::COLOR_BGR2GRAY);
    GaussianBlur(preprocessedFrame, preprocessedFrame, Size(3, 3), 0);
    Canny(preprocessedFrame, preprocessedFrame, 50, 150, 3);

    return preprocessedFrame;
}
