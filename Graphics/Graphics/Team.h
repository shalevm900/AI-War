#pragma once

#include "Supplies.h"
#include "Room.h"

class Team
{
private:
	Soldier* soldiers[TEAM_SIZE];
	Supplies* supplies[SUPPLIES_SIZE];
	int numSoldiers = TEAM_SIZE;
	int numSupplies = SUPPLIES_SIZE;

public:
	Team(Room** roomList);
	Soldier* getSoldier(int i) { return soldiers[i]; }
	Supplies* getSupplies(int i) { return supplies[i]; }
	int getNumSoldiers() { return numSoldiers; }
	void soldierKilled() { numSoldiers--; }
};

