#include "Direction.hpp"

Direction getOppositeDirection(Direction direction) {
    switch (direction) {
        case Direction::Left: return Direction::Right;
        case Direction::Right: return Direction::Left;
        case Direction::Up: return Direction::Down;
        case Direction::Down: return Direction::Up;
        case Direction::None: break;
    }
    return direction; // Default case (should never be hit)
}
