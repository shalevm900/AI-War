//
// Created by Omer Ishai on 04/03/2025->
//

#include "Soldier.h"
#include "generalFunctions.h"
#include <stdlib.h>

Soldier::Soldier(int type)
{
	isAlive = true;
	hp = 100;
	numBullets = MAX_BULLETS;
	numGrenades = MAX_GRENADES;
	state = SEARCH;
	lastTile = SPACE;
	switch (type)
	{
	case SERGEANT:
		role = SERGEANT;
		medKits = 0;
		riskFactor = 1;
		break;
	case MEDIC:
		role = MEDIC;
		medKits = MAX_MEDKITS;
		riskFactor = 0.5;
		break;
	case PRIVATE:
		role = PRIVATE;
		medKits = 0;
		riskFactor = 2;
		break;
	}

}

bool Soldier::getIsAlive()
{
	if (!isAlive)
	{
		resetSoldier();
	}
	return isAlive;
}

void Soldier::giveMedKit(Soldier* s1, Soldier* s2)
{
	if (s1->getMedKits() > 0)
	{
		s2->setMedKits(s2->getMedKits() + 1);
		s1->setMedKits(s1->getMedKits() - 1);
	}
}

void Soldier::giveGrenade(Soldier* s1, Soldier* s2)
{
	if (s1->getNumGrenades() >= 2)
	{
		s2->setNumGrenades(s2->getNumGrenades() + 2);
		s1->setNumGrenades(s1->getNumGrenades() - 2);
	}
	else
	{
		s2->setNumGrenades(s2->getNumGrenades() + s1->getNumGrenades());
		s1->setNumGrenades(0);
	}
}

void Soldier::giveBullet(Soldier* s1, Soldier* s2)
{
	if (s1->getNumBullets() >= 30)
	{
		s2->setNumBullets(s2->getNumBullets() + 30);
		s1->setNumBullets(s1->getNumBullets() - 30);
	}
	else
	{
		s2->setNumBullets(s2->getNumBullets() + s1->getNumBullets());
		s1->setNumBullets(0);
	}
}

void Soldier::useMedKit()
{
	medKits--;
	hp = 100;
}

int Soldier::pickSupplyNeed()
{
    if (role == MEDIC)
    {
        if (medKits < 2)
        {
            return MEDKIT;
        }
        if (numBullets < 30)
        {
            return BULLET;
        }
        if (numGrenades < 2)
        {
            return GRENADE;
        }
        return -1;
    }
	if (medKits < 1 && hp <= ATTACK_THERSHOLD)
	{
		return MEDKIT;
	}
	if (numBullets < 20)
	{
		return BULLET;
	}
	if (numGrenades < 2)
	{
		return GRENADE;
	}
	return -1;
}

void Soldier::shoot(Soldier* s)
{
	// chance of hit 30%
	int distance = sqrt(pow(y - s->getY(), 2) + pow(x - s->getX(), 2));

	double P_base = 0.9;   
	double k = 0.05;       
	double hitChance = P_base * exp(-k * distance) * 100; // Convert to percentage

	int roll = rand() % 100;
	Bullet* b;

	if (roll < hitChance)
	{
		b = new Bullet((double)x, (double)y, (double)s->getX(), (double)s->getY());
		s->decreaseHp(5/s->getRiskFactor());

	}
	else
	{
		// miss(fire bullet around target)
		double missOffset = (rand() % 20 - 10) * 0.1; // Random small offset
		b = new Bullet((double)x, (double)y, (double)s->getX() + missOffset, (double)s->getY() + missOffset);
		missedShots++;
	}
	bullets[bulletClip] = b;
	bulletClip++;
    numBullets--;
	

}
void Soldier::decreaseHp(int damage)
{
    hp -= damage;
    if (hp <= 0)
    {
        printf("Soldier %d killed in (%d,%d)\n", role, y, x);
        isAlive = false;
    }
}

void Soldier::resetSoldier()
{
	inPath = false;
	inCover = false;
	if (inPath)
	{
		path->setOccupied(false);
		path = nullptr;
	}
	room = nullptr;
}

