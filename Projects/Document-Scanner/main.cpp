#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <iostream>
#include <bits/stdc++.h>

using namespace cv;

Mat preprocessing(const Mat& img);
std::vector<Point> getContours(const Mat& imgDil, Mat& img);
Mat warpPerspective(const Mat& img, std::vector<Point> corners);
std::vector<Point2f> reorderPoints(const std::vector<Point>& points);

int main(int argc, char** argv) {
    std::cout << "Running the file scanner...\n";

    std::string path;
    if (argc == 1) {
        path = "test/paper.jpg";
        std::cout << "Using test file \"" << path << "\"\n";
    } else if (argc == 2) {
        std::cout << "Using the given file \"" << argv[1] << "\"\n";
        path = argv[1];
    } else {
        std::cerr << "Too many arguments!\n";
        return -1;
    }

    Mat imgInitial = imread(path);
    if (imgInitial.empty()) {
        std::cerr << "Failed to load image!\n";
        return -1;
    }

    resize(imgInitial, imgInitial, Size(), 0.5, 0.5);

    Mat imgDil = preprocessing(imgInitial);
    std::vector<Point> corners = getContours(imgDil, imgInitial);

    // imshow("Edges", imgDil);
    // imshow("Contours", imgInitial);

    if (corners.size() == 4) {
        Mat imgWarp = warpPerspective(imgInitial, corners);
        imshow("Warped Image", imgWarp);
    } else {
        std::cout << "No valid quadrilateral detected to warp.\n";
    }

    if (waitKey(0) == 'q') {
        std::cout << "Quitting...\n";
        return 0;
    }

    return 0;
}

Mat warpPerspective(const Mat& img, std::vector<Point> corners) {
    if (corners.size() != 4) {
        std::cerr << "Cannow warp perspective: there are " << corners.size() << "corners, so it's not a rectangle!\n";
        return img.clone();
    }

    std::vector<Point2f> src = reorderPoints(corners);

    // A4 paper size
    float width = 420, height = 596;
    std::vector<Point2f> dst {
        Point2f(0, 0),
        Point2f(width, 0),
        Point2f(width, height),
        Point2f(0, height)
    };

    Mat matrix = getPerspectiveTransform(src, dst);
    Mat imgWarp;
    warpPerspective(img, imgWarp, matrix, Size(width, height));

    return imgWarp;
}

std::vector<Point> getContours(const Mat& imgDil, Mat& img) {
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(imgDil.clone(), contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<Point>> conPoly(contours.size());
    std::vector<Point> biggest;
    double maxArea = 0;

    for (int i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);

        if (area > 1000) {
            double peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], peri * 0.02, true);

            if (area > maxArea && conPoly[i].size() == 4) {
                biggest = conPoly[i];
                maxArea = area;
            }
        }
    }

    if (!biggest.empty()) {
        std::cout << "Detected corneers.\n";
        for (int i = 0; i < biggest.size(); i++) {
            circle(img, biggest[i], 8, Scalar(255, 0, 255), FILLED);
            std::cout << "Corner " << i << ": ("<< biggest[i].x << ", " << biggest[i].y << ")\n";
        }

        drawContours(img, std::vector<std::vector<Point>>{biggest}, -1, Scalar(0, 255, 0), 3);
    }

    return biggest;
}

Mat preprocessing(const Mat& img) {
    Mat imgGray, imgDil, imgBlur, imgCanny;

    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    GaussianBlur(imgGray, imgBlur, Size(3, 3), 3 ,0); // try to blur more intensive!
    Canny(imgBlur, imgCanny, 25, 75);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(imgCanny, imgDil, kernel);

    return imgDil;
}

std::vector<Point2f> reorderPoints(const std::vector<Point>& points) {
    std::vector<Point2f> pts;
    for (const Point& p : points) pts.push_back(p);

    std::vector<Point2f> ordered(4);

    sort(pts.begin(), pts.end(), [](Point2f a, Point2f b) { return a.x + a.y < b.x + b.y; });
    ordered[0] = pts[0]; // top-left
    ordered[2] = pts[3]; // bottom-right

    sort(pts.begin(), pts.end(), [](Point2f a, Point2f b) { return a.y - a.x < b.y - b.x; });
    ordered[1] = pts[0]; // top-right
    ordered[3] = pts[3]; // bottom-left

    return ordered;
}

