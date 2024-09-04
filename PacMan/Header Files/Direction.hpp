#pragma once

enum class Direction {
    Left, Right, Up, Down, None
};

Direction getOppositeDirection(Direction direction);
