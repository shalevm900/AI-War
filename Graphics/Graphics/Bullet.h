#pragma once
#include "definitions.h"
class Bullet
{
private: 
	double x, y;
	double angle;
	double dirX, dirY;
	double speed;
	bool isMoving = false;
public:

	Bullet(double xx, double yy, double target_x, double target_y);
	Bullet(double xx, double yy, double angle);
	void move(int maze[MSZ][MSZ]);
	void show();
	void setIsMoving(bool value) { isMoving = value; }
	bool getIsMoving() { return isMoving; }
	double getAngle() { return angle; }
	void setAngle(double angle) { this->angle = angle; }
	void SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ]);
};

