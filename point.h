#pragma once

struct Point {
    float x;
    float y;

    Point makeSmallest(Point) const;
    Point makeLargest(Point) const;
    Point operator-(Point) const;
};
