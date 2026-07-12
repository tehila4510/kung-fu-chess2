#include "rules/PieceRules.h"
#include <array>
#include <utility>

namespace {

using Step = std::pair<int, int>;

bool inBounds(const Board& board, int row, int col) {
    return board.isWithinBounds(Position{row, col});
}

void addSlidingMoves(const Board& board, const Piece& piece,
                     const std::array<Step, 4>& dirs, std::set<Position>& out) {
    for (const Step& d : dirs) {
        int row = piece.position.row + d.first;
        int col = piece.position.col + d.second;
        while (inBounds(board, row, col)) {
            Position p{row, col};
            if (board.isEmpty(p)) {
                out.insert(p);
            } else {
                if (!board.isFriendly(p, piece.color)) {
                    out.insert(p);
                }
                break;
            }
            row += d.first;
            col += d.second;
        }
    }
}

template <std::size_t N>
void addStepMoves(const Board& board, const Piece& piece,
                  const std::array<Step, N>& offsets, std::set<Position>& out) {
    for (const Step& o : offsets) {
        int row = piece.position.row + o.first;
        int col = piece.position.col + o.second;
        if (!inBounds(board, row, col)) {
            continue;
        }
        Position p{row, col};
        if (!board.isFriendly(p, piece.color)) {
            out.insert(p);
        }
    }
}

constexpr std::array<Step, 4> kRookDirs{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
constexpr std::array<Step, 4> kBishopDirs{{{-1, -1}, {-1, 1}, {1, -1}, {1, 1}}};

bool isPawnInitialRow(int row, char color, int rows) {
    if (color == 'w') {
        if (row == 1) {
            return true;
        }
        return rows >= 4 && row == rows - 1;
    }
    if (row == 6) {
        return true;
    }
    return rows >= 4 && row == 0;
}

class NullRule : public IPieceRule {
public:
    std::set<Position> legalDestinations(const Board&, const Piece&) const override {
        return {};
    }
};

} // namespace

std::set<Position> RookRule::legalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> out;
    addSlidingMoves(board, piece, kRookDirs, out);
    return out;
}

std::set<Position> BishopRule::legalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> out;
    addSlidingMoves(board, piece, kBishopDirs, out);
    return out;
}

std::set<Position> QueenRule::legalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> out;
    addSlidingMoves(board, piece, kRookDirs, out);
    addSlidingMoves(board, piece, kBishopDirs, out);
    return out;
}

std::set<Position> KnightRule::legalDestinations(const Board& board, const Piece& piece) const {
    constexpr std::array<Step, 8> offsets{{
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    }};
    std::set<Position> out;
    addStepMoves(board, piece, offsets, out);
    return out;
}

std::set<Position> KingRule::legalDestinations(const Board& board, const Piece& piece) const {
    constexpr std::array<Step, 8> offsets{{
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
        {0, 1}, {1, -1}, {1, 0}, {1, 1}
    }};
    std::set<Position> out;
    addStepMoves(board, piece, offsets, out);
    return out;
}

std::set<Position> PawnRule::legalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> out;
    const int forward = piece.isWhite() ? -1 : 1;
    const int rows = board.getRowCount();
    const int row = piece.position.row;
    const int col = piece.position.col;

    Position oneForward{row + forward, col};
    if (inBounds(board, oneForward.row, oneForward.col) && board.isEmpty(oneForward)) {
        out.insert(oneForward);

        Position twoForward{row + 2 * forward, col};
        if (isPawnInitialRow(row, piece.color, rows)
            && inBounds(board, twoForward.row, twoForward.col)
            && board.isEmpty(twoForward)) {
            out.insert(twoForward);
        }
    }

    for (int dc : {-1, 1}) {
        Position capture{row + forward, col + dc};
        if (inBounds(board, capture.row, capture.col)
            && !board.isEmpty(capture)
            && !board.isFriendly(capture, piece.color)) {
            out.insert(capture);
        }
    }

    return out;
}

const IPieceRule& ruleFor(char kind) {
    static const RookRule rook;
    static const BishopRule bishop;
    static const QueenRule queen;
    static const KnightRule knight;
    static const KingRule king;
    static const PawnRule pawn;
    static const NullRule none;

    switch (kind) {
        case 'R': return rook;
        case 'B': return bishop;
        case 'Q': return queen;
        case 'N': return knight;
        case 'K': return king;
        case 'P': return pawn;
        default:  return none;
    }
}
