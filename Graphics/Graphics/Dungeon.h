#pragma once
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <queue>
#include "Cell.h"
#include <iostream>
#include "Room.h"
#include "CompareCells.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Path.h"

using namespace std;

extern int maze[MSZ][MSZ];
extern double security_map[MSZ][MSZ];
extern Room* rooms[NUM_ROOMS];
extern Team* teams[2];
extern Bullet* bullets[6];
extern int bulletClip;
extern Grenade* grenades[6];
extern int grenadeClip;
extern bool runWar;
extern bool greandeThrown;
extern int baseMap[MSZ][MSZ];
extern Path* paths[MAX_PATHS];
extern int numPaths;

void RestorePath(Cell* pCurrent);
void AddNeighbor(int r, int c, Cell* pCurrent,
    priority_queue<Cell*, vector<Cell*>, CompareCells>& pq,
    vector <Cell>& grays, vector <Cell>& black);
void BuildPath(int index1, int index2);
void FindAllPaths();
void ExplorePath(int row, int col, int& minRow, int& maxRow, int& minCol, int& maxCol, bool visited[MSZ][MSZ]);
Path* findPath(int row, int col);
void BuildPathBetweenTheRooms();
void SetupDungeon();
void ShowDungeon();
void createTeams();
void GenerateSecurityMap(Team* enemy);
void throwGrenade(Soldier* s, Team* enemy);