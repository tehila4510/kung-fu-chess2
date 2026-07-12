#ifndef MOTION_H
#define MOTION_H
#include "model/Position.h"
#include <string>

struct Motion {
    Position from, to;
    std::string piece;
    long long arrivalTime;
    // Monotonic order in which this motion was started; used to break ties
    // when two motions resolve on the same clock tick (see RealTimeArbiter).
    long long startSeq = 0;
};
#endif