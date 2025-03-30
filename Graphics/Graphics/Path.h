#pragma once
#include <algorithm>

class Path
{
private:
    int x, y;
    int target_x, target_y;
    int third_x, third_y;
    bool hasThird = false;
    bool occupied = false;

public:
    Path(int y, int x, int target_y, int target_x);
    Path(int y, int x, int target_y, int target_x, int third_y, int third_x);

    int getThirdX() const { return third_x; }
    int getThirdY() const { return third_y; }
    bool getHasThird() const { return hasThird; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getTargetX() const { return target_x; }
    int getTargetY() const { return target_y; }
    void setX(int new_x) { x = new_x; }
    void setY(int new_y) { y = new_y; }
    void setTargetX(int new_targetX) { target_x = new_targetX; }
    void setTargetY(int new_targetY) { target_y = new_targetY; }
    bool getOccupied() const { return occupied; }
    void setOccupied(bool value) { occupied = value; }

    // Handling T-junctions
    int getMinRow() const { return std::min({y, target_y, hasThird ? third_y : y}); }
    int getMinCol() const { return std::min({x, target_x, hasThird ? third_x : x}); }
    int getMaxRow() const { return std::max({y, target_y, hasThird ? third_y : y}); }
    int getMaxCol() const { return std::max({x, target_x, hasThird ? third_x : x}); }

    // Check if a point is on the path
    bool contains(int row, int col) const {
        return (row == y && col == x) ||
        (row == target_y && col == target_x) ||
        (hasThird && row == third_y && col == third_x);
    }
};
