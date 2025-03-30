#pragma once
#include "glut.h"
#include <queue>
#include "Cell.h"
#include <iostream>
#include "Room.h"
#include "CompareCells.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Path.h"
#include "Dungeon.h"


using namespace std;


void resetSecurityMap();
void SearchEnemy(Soldier* s, Team* t);
void pickState(Soldier* s, Team* self, Team* enemy);
void transferSupply(Soldier* s, int supplyType, Team* team);
void War();
void resupply(Soldier* s, Team* t);
Room* findClosestSupplyRoom(Soldier* s, Team* t);
void takeCover(Soldier* s, Team* self, Team* enemy);
void RunAStar(Soldier* s, int row, int col);
void placeSoldier(Team** teams);
void moveSoldier(Soldier* s, Cell* c);
Cell* runBFS(Soldier* s, Team* t);
Cell* runLimitedBFS(Soldier* s, Team* self, Team* enemy, int limit);
bool soldierCollide(Soldier* s, Team* self, Team* enemy);
void retreatSoldier(Soldier* s);
Cell* enemiesInRoom(Soldier* s, Team* enemy);
void placeSoldiers(Room* rooms[NUM_ROOMS], Team* teams[2]);
Soldier* findEnemy(int row, int col, Team* enemy);