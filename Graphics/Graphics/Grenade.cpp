#include "Grenade.h"


Grenade::Grenade(Team* enemy)
{
	int r = 0;
	int c = 0;
	int aliveSoldiers = 0;

	for (int teamSize = 0; teamSize < TEAM_SIZE; teamSize++)
	{
		// check if the soldier is alive if not then skip to the next soldier
		if (!(enemy->getSoldier(teamSize)->getIsAlive()))
		{
			continue;
		}
		else
		{
			aliveSoldiers++;
		}
		r += enemy->getSoldier(teamSize)->getY();
		c += enemy->getSoldier(teamSize)->getX();

	}
	if (aliveSoldiers == 0)
	{
		return;
	}
	row = r / aliveSoldiers;
	col = c / aliveSoldiers;
	int i;
	double alpha = 2 * PI / NUM_BULLETS; // bullet separation angle
	for (i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i] = new Bullet(col, row, i * alpha);
	}
}

Grenade::Grenade(double r, double c)
{
	int i;
	row = r;
	col = c;
	double alpha = 2 * PI / NUM_BULLETS; // bullet separation angle
	for (i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i] = new Bullet(c, r, i * alpha);
	}
}


void Grenade::explode()
{
	int i;

	for (i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i]->setIsMoving(true);
	}


}

void Grenade::expand(int maze[MSZ][MSZ])
{
	int i;

	for (i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i]->move(maze);
	}

}

void Grenade::show()
{
	int i;

	for (i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i]->show();
	}

}

void Grenade::SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ])
{
	int i;

	for (i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i]->SimulateExplosion(maze, sm);
	}

}

Bullet** Grenade::getBulletsArray()
{
	return bullets;
}

Grenade::~Grenade() {
	for (int i = 0; i < NUM_BULLETS; i++) {
		if (bullets[i] != nullptr) {
			delete bullets[i];
			bullets[i] = nullptr;
		}
	}
}