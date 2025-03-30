#pragma once

#include "Bullet.h"
#include "Team.h"

const int NUM_BULLETS = 20;

class Grenade
{
private:
	double row, col;
	Bullet* bullets[NUM_BULLETS]{};
public:
	Grenade(Team* t);
	~Grenade();
	Grenade(double r, double c);
	void explode();
	void expand(int maze[MSZ][MSZ]);
	void show();
	void SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ]);
	// get bullets array
	Bullet** getBulletsArray();

};
