#ifndef ELO_H
#define ELO_H

namespace elo {

constexpr int kDefaultK = 32;
constexpr int kInitialRating = 1200;

double expectedScore(int ratingA, int ratingB);
int updateRating(int ratingA, int ratingB, double scoreA, int k = kDefaultK);

}  // namespace elo

#endif
