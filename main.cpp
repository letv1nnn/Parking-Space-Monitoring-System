#include "common.h"

int main(int argc, char **argv) {
    std::cout << "Running the program...\n";

    std::string path = argc == 2 ? argv[1] : "test/parking_lots_video.mp4";
    if (argc > 2)
        std::cout << "Too many arguments!\n";
    std::cout << "Path: " << path << "\n";

    VideoCapture cap(path);
    if (!cap.isOpened()) {
        std::cerr << "Unable to open camera\n";
        return -1;
    }

    PointsStorage points;

#if CALIBRATE
    std::cout << "Calibration mode: Click to mark slots. Press 'q' to quit.\n";
#else
    std::cout << "Detection mode: Reading saved points.\n";
    points.readPoints("points.txt");
#endif

    namedWindow("Parking", WINDOW_AUTOSIZE);
#if CALIBRATE
    setMouseCallback("Parking", displayParkingLot, &points);
#endif

    Mat frame;
    double fps = cap.get(cv::CAP_PROP_FPS);

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Empty frame\n";
            break;
        }

        resize(frame, frame, Size(), 0.4, 0.4);

        Mat fullFrame(frame.rows + EXTRA_GUI_HEIGHT, frame.cols, frame.type(), Scalar(0, 0, 0));
        frame.copyTo(fullFrame(cv::Rect(0, 0, frame.cols, frame.rows)));

#if CALIBRATE
        for (const Point& pt : points.getPoints()) {
            rectangle(fullFrame, pt, Point(pt.x + WIDTH, pt.y + HEIGHT), Scalar(200, 0, 255), 2);
        }
        putText(fullFrame, "Calibration Mode", Point(20, fullFrame.rows - 20),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
#else
        int freeLots = checkLots(fullFrame, points);
        std::string text = "Free Parking Lots: " + std::to_string(freeLots);
        putText(fullFrame, text, Point(20, fullFrame.rows - 20),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
#endif

        std::string fpsStr = std::format("FPS: {}", fps);
        putText(fullFrame, fpsStr, Point(fullFrame.cols - 170, fullFrame.rows - 20),
            FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        imshow("Parking", fullFrame);

        int key = waitKey(1);
        if (key == 'q') {
            std::cout << "Quitting...\n";
            return 0;
        }
    }

    return 0;
}
