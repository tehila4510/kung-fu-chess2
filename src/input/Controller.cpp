#include "input/Controller.h"

Controller::Controller(GameEngine& engine, int cellSize)
    : engine(engine),
      mapper(cellSize, engine.columnCount(), engine.rowCount()) {}

Controller::Controller(GameEngine& engine, BoardMapper mapper)
    : engine(engine), mapper(mapper) {}

bool Controller::hasActiveSelection() const {
    return hasSelection;
}

Position Controller::selectedCell() const {
    return selection;
}

void Controller::clearSelection() {
    hasSelection = false;
    selection = Position{};
}

// Guards snapshot access; a cell beyond the actual board holds no piece.
static bool cellHasPiece(const GameSnapshot& snap, const Position& cell) {
    if (cell.row < 0 || cell.row >= static_cast<int>(snap.cells.size())) {
        return false;
    }
    const std::vector<std::string>& row = snap.cells[static_cast<size_t>(cell.row)];
    if (cell.col < 0 || cell.col >= static_cast<int>(row.size())) {
        return false;
    }
    return row[static_cast<size_t>(cell.col)] != ".";
}

ClickResult Controller::click(int x, int y) {
    const std::optional<Position> mapped = mapper.pixelToCell(x, y);

    // Off-board click means "never mind": cancel any pending selection.
    if (!mapped) {
        clearSelection();
        return { ClickOutcome::Cleared, { false, "" } };
    }
    const Position cell = *mapped;

    if (!hasSelection) {
        if (!cellHasPiece(engine.snapshot(), cell)) {
            return { ClickOutcome::Ignored, { false, "" } };
        }
        selection = cell;
        hasSelection = true;
        return { ClickOutcome::Selected, { false, "" } };
    }

    if (cell == selection) {
        clearSelection();
        return { ClickOutcome::Cleared, { false, "" } };
    }

    const Position from = selection;
    clearSelection();
    const MoveResult result = engine.requestMove(from, cell);
    return { ClickOutcome::MoveRequested, result };
}
