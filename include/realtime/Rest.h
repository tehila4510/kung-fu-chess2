#ifndef REST_H
#define REST_H

#include "model/Position.h"
#include "realtime/RestView.h"

#include <string>

struct Rest {
    Position at;
    std::string piece;
    RestKind kind = RestKind::Long;
    long long endTime = 0;
    long long durationMs = 0;
};

#endif
