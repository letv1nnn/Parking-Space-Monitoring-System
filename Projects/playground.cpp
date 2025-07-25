#include <iostream>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

using namespace cv;

int main(int argc, char** argv) {
    std::cout << "Running the program...\n";

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "ERROR: Could not launch the Web Camera!\n";
        return -1;
    }

    Mat frame, frameHSV, frameMask;

    Scalar lowerLimit(0, 48, 80);    // HSV range for skin (adjust if needed)
    Scalar upperLimit(20, 255, 255);

    while (true) {
        cap.read(frame);
        if (frame.empty()) {
            std::cerr << "Could not read the frame!\n";
            break;
        }

        cvtColor(frame, frameHSV, COLOR_BGR2HSV);
        inRange(frameHSV, lowerLimit, upperLimit, frameMask);

        // Finger counting section
        std::vector<std::vector<Point>> contours;
        std::vector<Vec4i> hierarchy;

        findContours(frameMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
        if (!contours.empty()) {
            size_t largestContourIndex = 0;
            double maxArea = 0;

            for (size_t i = 0; i < contours.size(); i++) {
                double area = contourArea(contours[i]);
                if (area > maxArea) {
                    maxArea = area;
                    largestContourIndex = i;
                }
            }

            if (maxArea > 5000) {  // Filter out small noisy blobs
                std::vector<Point> handContour = contours[largestContourIndex];

                std::vector<int> hullIndices;
                std::vector<Point> hullPoints;

                convexHull(handContour, hullIndices, false, false);
                convexHull(handContour, hullPoints, false, true);

                std::vector<Vec4i> defects;
                int fingerCount = 0;

                // Only attempt defects if input is valid
                if (hullIndices.size() > 3 && handContour.size() > 3) {
                    try {
                        convexityDefects(handContour, hullIndices, defects);
                    } catch (const cv::Exception& e) {
                        std::cerr << "Warning: " << e.what() << "\n";
                        defects.clear();
                    }

                    drawContours(frame, contours, (int)largestContourIndex, Scalar(255, 0, 0), 2);
                    polylines(frame, hullPoints, true, Scalar(0, 255, 0), 2);

                    if (!defects.empty()) {
                        for (size_t i = 0; i < defects.size(); ++i) {
                            Point ptStart = handContour[defects[i][0]];
                            Point ptEnd = handContour[defects[i][1]];
                            Point ptFar = handContour[defects[i][2]];
                            float depth = defects[i][3] / 256.0;

                            double a = norm(ptStart - ptFar);
                            double b = norm(ptEnd - ptFar);
                            double c = norm(ptEnd - ptStart);

                            double angle = acos((a*a + b*b - c*c) / (2*a*b)) * 180.0 / CV_PI;

                            if (angle < 90 && depth > 20) {
                                fingerCount++;
                                circle(frame, ptFar, 5, Scalar(0, 0, 255), -1);
                            }
                        }

                        fingerCount = std::min(fingerCount + 1, 5);
                    }
                }

                putText(frame, "Fingers: " + std::to_string(fingerCount), Point(30, 50), 
                        FONT_HERSHEY_SCRIPT_SIMPLEX, 1, Scalar(0, 255, 255), 2);
            }
        }

        imshow("Playground Program Default", frame);
        // imshow("Playground Program HSV", frameHSV);
        // imshow("Playground Program Mask", frameMask);

        if (waitKey(1) == 'q') {
            std::cout << "Exiting the program...\n";
            break;
        }
    }

    return 0;
}

