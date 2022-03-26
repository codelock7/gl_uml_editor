#include "points.h"
#include "plainrect.h"

void Points::add(Rect rect) {
    PlainRect plainRect;
    plainRect.fill(rect);
    const auto [ptr, size] = plainRect.toComponents();
    for (size_t i = 0; i < size; ++i)
        mPoints.push_back(ptr[i]);
}

float& Points::getItem(size_t index) {
    return mPoints[index];
}

size_t Points::getCount() const {
    return mPoints.size();
}

size_t Points::getByteCount() const {
    return mPoints.size() * sizeof(float);
}

float* Points::getData() {
    return mPoints.data();
}

void Points::swap(size_t i, size_t j) {
    std::swap(mPoints[i], mPoints[j]);
}
