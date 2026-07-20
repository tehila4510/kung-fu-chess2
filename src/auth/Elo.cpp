#include "auth/Elo.h"

#include <cmath>

namespace elo {

double expectedScore(int ratingA, int ratingB) {
    return 1.0 / (1.0 + std::pow(10.0, static_cast<double>(ratingB - ratingA) / 400.0));
}

int updateRating(int ratingA, int ratingB, double scoreA, int k) {
    const double expected = expectedScore(ratingA, ratingB);
    return static_cast<int>(std::lround(ratingA + k * (scoreA - expected)));
}

}  // namespace elo
