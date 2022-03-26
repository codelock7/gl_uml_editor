#pragma once
#include <vector>
#include "rect.h"

class Points {
public:
    template<typename T>
    bool hasItem(size_t index) const {
        constexpr size_t itemCount = sizeof(T) / sizeof(float);
        return index + itemCount <= mPoints.size();
    }

    template<typename T>
    T& getItem(size_t index) {
        return *reinterpret_cast<T*>(&mPoints[index]);
    }

    static constexpr size_t getInvalidIndex() {
        return static_cast<size_t>(-1);
    }

    void add(Rect);
    float& getItem(size_t index);
    size_t getCount() const;
    size_t getByteCount() const;
    float* getData();
    void swap(size_t, size_t);

private:
    std::vector<float> mPoints;
};
