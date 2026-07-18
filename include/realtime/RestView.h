#ifndef REST_VIEW_H
#define REST_VIEW_H

#include "model/Position.h"

#include <string>

enum class RestKind {
    Short,
    Long
};

struct RestView {
    std::string piece;
    Position at;
    RestKind kind = RestKind::Long;
    // 1.0 just after landing, 0.0 when rest ends.
    double remaining = 0.0;
};

#endif
