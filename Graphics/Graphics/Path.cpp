#include "Path.h"

Path::Path(int y, int x, int target_y, int target_x)
{
	this->x = x;
	this->y = y;
	this->target_x = target_x;
	this->target_y = target_y;
}

Path::Path(int y, int x, int target_y, int target_x, int third_y, int third_x)
{
    this->x = x;
    this->y = y;
    this->target_x = target_x;
    this->target_y = target_y;
    this->third_x = third_x;
    this->third_y = third_y;
    hasThird = true;
}
