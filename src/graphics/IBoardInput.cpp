#include "graphics/IBoardInput.h"

#include "bus/GameEvent.h"
#include "protocol/Algebraic.h"

#include <iostream>

namespace graphics {

void NoOpBoardInput::handleClick(const view::MouseClick&, const BoardMapper&) {}

bool NoOpBoardInput::hasActiveSelection() const {
    return false;
}

Position NoOpBoardInput::selectedCell() const {
    return Position{};
}

std::set<Position> NoOpBoardInput::legalMovesFromSelection() const {
    return {};
}

LocalBoardInput::LocalBoardInput(Controller& controller, GameEngine& engine,
                                 EventPublisher publish, PieceLookup pieceAt)
    : controller_(controller),
      engine_(engine),
      publish_(std::move(publish)),
      pieceAt_(std::move(pieceAt)) {}

void LocalBoardInput::handleClick(const view::MouseClick& click,
                                  const BoardMapper& mapper) {
    const std::optional<Position> cell = mapper.pixelToCell(click.x, click.y);
    if (!cell) {
        if (!click.is_double) {
            const bool hadSelection = controller_.hasActiveSelection();
            controller_.clearSelection();
            if (hadSelection) {
                GameEvent cleared;
                cleared.type = GameEventType::SelectionCleared;
                publish_(cleared);
            }
        }
        std::cout << "Click off board at (" << click.x << ", " << click.y << ")\n";
        return;
    }

    if (click.is_double) {
        const std::string piece = pieceAt_(*cell);
        const MoveOutcome result = controller_.jump(*cell);
        if (result.is_accepted) {
            std::cout << "Jump accepted at (" << cell->row << ", " << cell->col
                      << ")\n";
            const std::string square =
                protocol::positionToSquare(*cell, engine_.rowCount());
            GameEvent jumpEvent;
            jumpEvent.type = GameEventType::JumpMade;
            jumpEvent.color =
                (piece.size() == 2 && piece[0] == 'w') ? 'W' : 'B';
            jumpEvent.piece = piece;
            jumpEvent.from = square;
            jumpEvent.to = square;
            publish_(jumpEvent);
        } else {
            std::cout << "Jump rejected: " << result.reason << '\n';
        }
        return;
    }

    const bool hadSelection = controller_.hasActiveSelection();
    const Position from = hadSelection ? controller_.selectedCell() : Position{};
    const std::string mover = hadSelection ? pieceAt_(from) : std::string(".");
    const ClickResult result = controller_.click(*cell);
    switch (result.outcome) {
        case ClickOutcome::Selected: {
            std::cout << "Selected piece at (" << cell->row << ", " << cell->col
                      << ")\n";
            const std::string piece = pieceAt_(*cell);
            GameEvent selected;
            selected.type = GameEventType::PieceSelected;
            selected.color =
                (piece.size() == 2 && piece[0] == 'w') ? 'W' : 'B';
            selected.piece = piece;
            publish_(selected);
            break;
        }
        case ClickOutcome::Cleared: {
            std::cout << "Selection cleared\n";
            GameEvent cleared;
            cleared.type = GameEventType::SelectionCleared;
            publish_(cleared);
            break;
        }
        case ClickOutcome::MoveRequested:
            if (result.moveResult.is_accepted) {
                std::cout << "Move accepted to (" << cell->row << ", " << cell->col
                          << ")\n";
                GameEvent moveEvent;
                moveEvent.type = GameEventType::MoveMade;
                moveEvent.color =
                    (mover.size() == 2 && mover[0] == 'w') ? 'W' : 'B';
                moveEvent.piece = mover;
                moveEvent.from =
                    protocol::positionToSquare(from, engine_.rowCount());
                moveEvent.to =
                    protocol::positionToSquare(*cell, engine_.rowCount());
                publish_(moveEvent);
            } else {
                std::cout << "Move rejected: " << result.moveResult.reason << '\n';
                GameEvent cleared;
                cleared.type = GameEventType::SelectionCleared;
                publish_(cleared);
            }
            break;
        case ClickOutcome::Ignored:
            std::cout << "Click ignored at (" << cell->row << ", " << cell->col
                      << ")\n";
            break;
    }
}

bool LocalBoardInput::hasActiveSelection() const {
    return controller_.hasActiveSelection();
}

Position LocalBoardInput::selectedCell() const {
    return controller_.selectedCell();
}

std::set<Position> LocalBoardInput::legalMovesFromSelection() const {
    if (!controller_.hasActiveSelection()) {
        return {};
    }
    return engine_.legalMovesFrom(controller_.selectedCell());
}

}  // namespace graphics
