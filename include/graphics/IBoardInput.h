#ifndef IBOARD_INPUT_H
#define IBOARD_INPUT_H

#include "bus/GameEvent.h"
#include "engine/GameEngine.h"
#include "input/BoardMapper.h"
#include "input/Controller.h"
#include "model/Position.h"
#include "view/Img.h"

#include <functional>
#include <set>
#include <string>

namespace graphics {

// Board click handling. Online injects NoOp (Play button is separate).
class IBoardInput {
public:
    virtual ~IBoardInput() = default;

    virtual void handleClick(const view::MouseClick& click,
                             const BoardMapper& mapper) = 0;
    virtual bool hasActiveSelection() const = 0;
    virtual Position selectedCell() const = 0;
    // Empty for online / NoOp — legal-move overlays omitted when empty.
    virtual std::set<Position> legalMovesFromSelection() const = 0;
};

class NoOpBoardInput : public IBoardInput {
public:
    void handleClick(const view::MouseClick& click,
                     const BoardMapper& mapper) override;
    bool hasActiveSelection() const override;
    Position selectedCell() const override;
    std::set<Position> legalMovesFromSelection() const override;
};

class LocalBoardInput : public IBoardInput {
public:
    using EventPublisher = std::function<void(const GameEvent&)>;
    using PieceLookup = std::function<std::string(const Position&)>;

    LocalBoardInput(Controller& controller, GameEngine& engine,
                    EventPublisher publish, PieceLookup pieceAt);

    void handleClick(const view::MouseClick& click,
                     const BoardMapper& mapper) override;
    bool hasActiveSelection() const override;
    Position selectedCell() const override;
    std::set<Position> legalMovesFromSelection() const override;

private:
    Controller& controller_;
    GameEngine& engine_;
    EventPublisher publish_;
    PieceLookup pieceAt_;
};

}  // namespace graphics

#endif
