#pragma once

#include "Soldier.h"
#include "Room.h"


class Supplies
{
private:
	Room* room;
	int medKits; 
	int Bullets;
	int Grenades;

public:
	Supplies(Room* room);
	int getMedKits() { return medKits; }
	int getBullets() { return Bullets; }
	int getGrenades() { return Grenades; }
	void takeMedKit(Soldier* s); 
	void takeBullet(Soldier* s);  
	void takeGrenade(Soldier* s);
	void takeSupplies(Soldier* s,int supply);
    Room* getRoom() { return room; }

	//void show(); // maybe not needed, just give unique value to the cell in maze
};

