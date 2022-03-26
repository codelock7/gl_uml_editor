#include "size.h"

Size Size::operator+(Size rhs) {
    return {width + rhs.width, height + rhs.height};
}
