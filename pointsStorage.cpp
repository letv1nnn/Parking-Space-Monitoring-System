#include "common.h"

void PointsStorage::writePoint(const Point pt) {
    std::ofstream PointsStorage("points.txt", std::ios::app);
    if (PointsStorage.is_open()) {
        PointsStorage << pt.x << " " << pt.y << '\n';
        points.emplace_back(pt);
        PointsStorage.close();
    }
}

void PointsStorage::readPoints(const std::string& filename) {
    std::ifstream PointsStorage(filename);
    std::string line;
    while (getline(PointsStorage, line)) {
        std::istringstream iss(line);
        int x, y;
        if (iss >> x >> y) {
            points.emplace_back(x, y);
        }
    }
    PointsStorage.close();
}

std::vector<Point> PointsStorage::getPoints() const {
    return points;
}