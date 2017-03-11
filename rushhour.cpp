#include <iostream>
#include "rushhour.h"

// Keep this
std::ostream& operator<<( std::ostream& os, Direction const& d ) {
    switch ( d ) {
        case up:    os << "up "; break;
        case left:  os << "left "; break;
        case down:  os << "down "; break;
        case right: os << "right "; break;
        default:    os << "undefined"; break;
    }
    return os;
}

