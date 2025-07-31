#include "common.h"


void displayParkingLot(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        auto* ps = static_cast<PointsStorage*>(userdata);
        Point pt(x, y);
        std::cout << "Added point (" << pt.x << ", " << pt.y << ")\n";
        ps->writePoint(pt);
    }
}