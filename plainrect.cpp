#include "plainrect.h"

static const Size& toSize(const Point& point) {
    return *reinterpret_cast<const Size*>(&point);
}

Size PlainRect::getBoundRectSize(const PlainRect& rhs) const {
    const Point b = bottomRight.makeLargest(rhs.bottomRight);
    const Point a = topLeft.makeSmallest(rhs.topLeft);
    return toSize(b - a);
}

Size PlainRect::getSize() const {
    return toSize(bottomRight - topLeft);
}

void PlainRect::fill(Rect rect) {
    topLeft.x = rect.x;
    topLeft.y = rect.y;
    topRight.x = rect.x + rect.width;
    topRight.y = rect.y;
    bottomRight.x = rect.x + rect.width;
    bottomRight.y = rect.y + rect.height;
    bottomLeft.x = rect.x;
    bottomLeft.y = rect.y + rect.height;
}

PlainRect& PlainRect::operator+=(Point rhs) {
    topLeft.x += rhs.x;
    topRight.x += rhs.x;
    bottomRight.x += rhs.x;
    bottomLeft.x += rhs.x;
    topLeft.y += rhs.y;
    topRight.y += rhs.y;
    bottomRight.y += rhs.y;
    bottomLeft.y += rhs.y;
    return *this;
}

bool PlainRect::isBound(Point point) const {
    return (point.x >= topLeft.x) && (point.x < bottomRight.x) &&
           (point.y >= topLeft.y) && (point.y < bottomRight.y);
}

bool PlainRect::isIntersect(const PlainRect& object) const {
    const Size size = getBoundRectSize(object);
    const Size expectedSize = object.getSize() + getSize();
    if (size.width >= expectedSize.width || size.height >= expectedSize.height)
        return false;
    return true;
}

std::pair<float*, size_t> PlainRect::toComponents() {
    return {reinterpret_cast<float*>(this), getComponentCount()};
}
