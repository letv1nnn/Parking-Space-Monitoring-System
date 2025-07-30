
// Source file with some debug/working steps while I was working on this project.

#if 0 // debugging, drawing rectangles directly.
void spawningParkingLot(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        std::cout << "Left button clicked at (" << x << ", " << y << ")\n";
        clickedPoints.emplace_back(Point(x, y));
    } else if (event == EVENT_RBUTTONDOWN) {
        if (!clickedPoints.empty()) {
            std::cout << "Right button clicked at (" << x << ", " << y << ")\n";
            clickedPoints.pop_back();
        }
    }
    Mat* image = reinterpret_cast<Mat*>(userdata);
    image->release();
    baseImage.copyTo(*image);

    for (const auto& pt : clickedPoints) {
        rectangle(*image, pt, Point(pt.x + WIDTH, pt.y + HEIGHT), Scalar(200, 0, 255), 2);
    }

    imshow("Parking Lots Detection Frame", *image);
}
#endif
