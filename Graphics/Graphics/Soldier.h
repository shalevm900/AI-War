# pragma once

#include "definitions.h"
//#include "Team.h"
#include "Room.h"
#include "Bullet.h"
#include "Path.h"


class Soldier {

private:
	int x, y; // position of soldier
	int hp; // health points
	int numBullets; 
	int numGrenades; 
	int medKits; // used for medic
	double riskFactor; // used to determine movement on security map based on type || higher = safer
	int role; // Choose what role for soldier
	bool isAlive; // says if soldier is still alive 
	Room* room; // room soldier is in
	bool inCover = false;
	int state;
	int lastX, lastY;
	int lastTile;
	bool inPath = false;
	int missedShots = 0;
	Path* path = nullptr;
	bool retreat = false;
	Room* targetRoom = nullptr;
	//enum role{ SERGEANT, MEDIC, PRIVATE };

public:
	// Constructor
	Soldier(int type);
	// Getters and Setters
	bool getIsAlive(); // check if soldier is alive
	void soldierDied() { isAlive = false; } // set soldier to dead
	int getX() { return x; }
	int getY() { return y; }
	void setX(int x) { this->x = x; }
	void setY(int y) { this->y = y; }
	int getHp() { return hp; }
	int getNumBullets() { return numBullets; }
	int getNumGrenades() { return numGrenades; }
	int getMedKits() { return medKits; }
	double getRiskFactor() { return riskFactor; } 
	int getRole() { return role; }
    void decreaseHp(int damage);
	void setNumBullets(int numBullets) { this->numBullets = numBullets; }
	void setNumGrenades(int numGrenades) { this->numGrenades = numGrenades; }
	void setMedKits(int medKits) { this->medKits = medKits; }
	void giveMedKit(Soldier* s1, Soldier* s2);
	void giveGrenade(Soldier* s1, Soldier* s2);
	void giveBullet(Soldier* s1, Soldier* s2);
	void useMedKit();
	Room* getRoom() { return room; }
	int pickSupplyNeed();
	void setRoom(Room* r) { room = r; }
	bool getInCover() { return inCover; }
	void setInCover(bool inCover) { this->inCover = inCover; }
	int getState() { return state; }
	void setState(int state) { this->state = state; }
	void shoot(Soldier* s);
	void setLastX(int x) { lastX = x; }
	void setLastY(int y) { lastY = y; }
	int getLastX() { return lastX; }
	int getLastY() { return lastY; }
	void setLastTile(int tile) { lastTile = tile; }
	int getLastTile() { return lastTile; }
	void setInPath(bool inPath) { this->inPath = inPath; }
	bool getInPath() { return inPath; }
	void setPath(Path* p) { path = p; }
	Path* getPath() { return path; }
	void resetSoldier();
	void missedShot() { missedShots++; }
	int getMissedShots() { return missedShots; }
	void resetMissedShots() { missedShots = 0; }
	void setRetreat(bool retreat) { this->retreat = retreat; }
	bool getRetreat() { return retreat; }
	void setTargetRoom(Room* r) { targetRoom = r; }
	Room* getTargetRoom() { return targetRoom; }
	//void getSupply(Soldier* s, int supplyType, Team* team);
	//void show(); // TODO

};
