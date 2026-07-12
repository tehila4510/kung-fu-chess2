#ifndef MOTION_H
#define MOTION_H
#include "model/Position.h"
#include <string>

struct Motion {
    Position from, to;
    std::string piece;
    long long arrivalTime;
};
#endif