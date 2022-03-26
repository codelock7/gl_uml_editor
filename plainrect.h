#pragma once
#include <utility>
#include "point.h"
#include "size.h"
#include "rect.h"

struct PlainRect {
    Point topLeft;
    Point topRight;
    Point bottomRight;
    Point bottomLeft;

    static constexpr size_t getByteSize() {
        return sizeof(PlainRect);
    }

    static constexpr size_t getComponentCount() {
        return 8;
    }

    Size getBoundRectSize(const PlainRect&) const;
    Size getSize() const;
    void fill(Rect);
    PlainRect& operator+=(Point);
    bool isBound(Point) const;
    bool isIntersect(const PlainRect&) const;
    std::pair<float*, size_t> toComponents();
};
