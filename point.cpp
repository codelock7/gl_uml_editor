#include "point.h"
#include <algorithm>

Point Point::makeSmallest(Point rhs) const {
    Point result;
    result.x = std::min(x, rhs.x);
    result.y = std::min(y, rhs.y);
    return result;
}

Point Point::makeLargest(Point rhs) const {
    Point result;
    result.x = std::max(x, rhs.x);
    result.y = std::max(y, rhs.y);
    return result;
}

Point Point::operator-(Point rhs) const {
    Point result;
    result.x = x - rhs.x;
    result.y = y - rhs.y;
    return result;
}
