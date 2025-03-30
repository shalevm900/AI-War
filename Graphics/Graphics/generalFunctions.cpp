#include "generalFunctions.h"


bool checkSoldier(int x, int y, Team* t)
{
	for (int i = 0; i < TEAM_SIZE; i++)
	{
		if (t->getSoldier(i)->getX() == x && t->getSoldier(i)->getY() == y)
			return true;
	}
	return false;
}

Cell* runLimitedBFS(Soldier* s, Team* self, Team* enemy, int limit)
{

	int risk = s->getRiskFactor() * 10;
	double security_map_copy[MSZ][MSZ] = { 0 };
	// Check all 4 directions (up, right, down, left)
	int directions[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };

	for (int i = 0; i < MSZ; i++)
	{
		for (int j = 0; j < MSZ; j++)
		{
			if (baseMap[i][j] == WALL)
			{
				security_map_copy[i][j] = WALL;
			}

			else if (baseMap[i][j] == PATH)
			{
				security_map_copy[i][j] = (security_map[i][j]) * (500 * risk);
			}
			else
			{
				security_map_copy[i][j] = (security_map[i][j]) * (risk * 100);
			}
		}
	}

	for (int i = 0; i < numPaths; i++)
	{
		Path* p = paths[i];
		for (auto dir : directions)
		{
			int r = p->getY() + dir[0];
			int c = p->getX() + dir[1];
			int tr = p->getTargetY() + dir[0];
			int tc = p->getTargetX() + dir[1];
			if (maze[r][c] == SPACE)
			{
				security_map_copy[r][c] += 100;
			}
			if (maze[tr][tc] == SPACE)
			{
				security_map_copy[tr][tc] += 100;
			}
		}
	}
	// make areas around enemies dangerous
	for (int i = 0; i < TEAM_SIZE; i++)
	{
		Soldier* enemySoldier = enemy->getSoldier(i);
		if (enemySoldier->getIsAlive())
		{
			for (auto dir : directions)
			{
				int r = enemySoldier->getY() + dir[0];
				int c = enemySoldier->getX() + dir[1];
				if (maze[r][c] == SPACE)
				{
					security_map_copy[r][c] += 100;
				}
			}
		}
	}
	int value = security_map[s->getY()][s->getX()];
	int value2 = security_map_copy[s->getY()][s->getX()];

	// get soldier location in room
	int r = s->getY();
	int c = s->getX();

	Cell* pCurrent = new Cell(r, c, nullptr);
	queue<Cell*> grays;
	grays.push(pCurrent);

	double bestCover = security_map_copy[r][c];
	int bestCoverX = c;
	int bestCoverY = r;



	// set last position to black
	security_map[s->getLastY()][s->getLastX()] = BLACK;
	// set current position to black
	security_map[r][c] = BLACK;

	// create sm copy

	// make enemy soldiers dangerous


	for (int step = 0; step < limit; step++) {
		int cellsInCurrentLevel = grays.size();

		// Process all cells at current BFS level
		for (int i = 0; i < cellsInCurrentLevel; i++) {
			pCurrent = grays.front();
			grays.pop();


			for (int j = 0; j < 4; j++) {
				int newR = pCurrent->getRow() + directions[j][0];
				int newC = pCurrent->getCol() + directions[j][1];

				// Check if valid cell and not a wall
				if (security_map_copy[newR][newC] != BLACK && maze[newR][newC] != WALL && security_map_copy[newR][newC] != GRAY) {

					// Check if this position offers better cover

					if (security_map_copy[newR][newC] < bestCover) {
						bestCover = security_map_copy[newR][newC];
						bestCoverX = newC;
						bestCoverY = newR;
					}

					// Create new cell and add to queue
					Cell* pNew = new Cell(newR, newC, pCurrent);
					grays.push(pNew);

					// Mark as visited
					security_map_copy[newR][newC] = GRAY;
				}
			}
			// Free memory of processed cell
			delete pCurrent;
		}
	}
	// get bestCover x + y from temp to original maze
	//bestCoverX = (bestCoverX - s->getX()) + s->getX();
	//bestCoverY = (bestCoverY - s->getY()) + s->getY();
	return new Cell(bestCoverY, bestCoverX, nullptr);
}

void takeCover(Soldier* s, Team* self, Team* enemy)
{
	Cell* target = runLimitedBFS(s, self, enemy, 5);
	if (target->getRow() != s->getY() || target->getCol() != s->getX())
	{
		RunAStar(s, target->getRow(), target->getCol());
	}
	else
	{
		s->setInCover(true);
	}
}

void moveSoldier(Soldier* s, Cell* cell)
{
	// reach first cell in path
	Cell* prev = cell;
	while (prev->getParent() != nullptr && (prev->getParent())->getParent() != nullptr)
	{
		prev = prev->getParent();
	}


	s->setLastX(s->getX());
	s->setLastY(s->getY());
	maze[s->getY()][s->getX()] = baseMap[s->getY()][s->getX()];
	s->setX(prev->getCol());
	s->setY(prev->getRow());
	maze[s->getY()][s->getX()] = SOLDIER;

	// exit
	if (baseMap[s->getY()][s->getX()] == SPACE && s->getInPath())
	{
		Path* p = s->getPath();

		p->setOccupied(false);
		s->setPath(nullptr);
		s->setInPath(false);
	}
	//enter
	if (baseMap[s->getY()][s->getX()] == PATH && !s->getInPath())
	{
		Path* p = findPath(s->getY(), s->getX());
		if (p != nullptr)
		{
			// resetRoom	
			s->setRoom(nullptr);
			s->setPath(p);
			p->setOccupied(true);
			s->setInPath(true);
			return;
		}
	}
	if (s->getInPath())
		return;


	//update soldier room
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		//get room center and dimensions
		int roomCenterX = rooms[i]->getCenterX();
		int roomCenterY = rooms[i]->getCenterY();
		int roomWidth = rooms[i]->getWidth();
		int roomHeight = rooms[i]->getHeight();
		bool inRoom = (s->getX() >= roomCenterX - roomWidth / 2 && s->getX() <= roomCenterX + roomWidth / 2 &&
			s->getY() >= roomCenterY - roomHeight / 2 && s->getY() <= roomCenterY + roomHeight / 2);
		if (inRoom) {
			s->setRoom(rooms[i]);
			break;
		}
	}
}

Cell* runBFS(Soldier* s, Team* t)
{
	int r = s->getY();
	int c = s->getX();
	Cell* pCurrent = new Cell(r, c, nullptr);
	queue<Cell*> grays;
	grays.push(pCurrent);
	//create a copy of the maze
	int tempMaze[MSZ][MSZ];
	for (int i = 0; i < MSZ; i++)
		for (int j = 0; j < MSZ; j++)
		{
			tempMaze[i][j] = maze[i][j];
			if (tempMaze[i][j] == SOLDIER)
			{
				security_map[i][j] = GRAY;
			}
		}

	tempMaze[r][c] = BLACK;
	// set Soldiers locations as GRAY?


	// set enemy locations
	for (int i = 0; i < TEAM_SIZE; i++)
	{
		Soldier* enemy = t->getSoldier(i);
		if (enemy->getIsAlive())
		{
			tempMaze[enemy->getY()][enemy->getX()] = ENEMY;
		}

	}

	bool isFound = false;
	while (!grays.empty() && !isFound)
	{
		pCurrent = grays.front();
		grays.pop();
		r = pCurrent->getRow();
		c = pCurrent->getCol();

		//check UP
		if (tempMaze[r - 1][c] != WALL && tempMaze[r - 1][c] != GRAY && tempMaze[r - 1][c] != BLACK)
		{
			if (tempMaze[r - 1][c] == ENEMY)
			{
				isFound = true;

			}
			else
			{
				tempMaze[r - 1][c] = GRAY;
				grays.push(new Cell(r - 1, c, pCurrent));
			}
		}
		// check DOWN
		if (tempMaze[r + 1][c] != WALL && tempMaze[r + 1][c] != GRAY && tempMaze[r + 1][c] != BLACK)
		{
			if (tempMaze[r + 1][c] == ENEMY)
			{
				isFound = true;
			}
			else
			{
				tempMaze[r + 1][c] = GRAY;
				grays.push(new Cell(r + 1, c, pCurrent));
			}
		}
		// check LEFT
		if (tempMaze[r][c - 1] != WALL && tempMaze[r][c - 1] != GRAY && tempMaze[r][c - 1] != BLACK)
		{
			if (tempMaze[r][c - 1] == ENEMY)
			{
				isFound = true;
			}
			else
			{
				tempMaze[r][c - 1] = GRAY;
				grays.push(new Cell(r, c - 1, pCurrent));
			}
		}
		// check RIGHT
		if (tempMaze[r][c + 1] != WALL && tempMaze[r][c + 1] != GRAY && tempMaze[r][c + 1] != BLACK)
		{
			if (tempMaze[r][c + 1] == ENEMY)
			{
				isFound = true;
			}
			else
			{
				tempMaze[r][c + 1] = GRAY;
				grays.push(new Cell(r, c + 1, pCurrent));
			}
		}

	}
	// free memory
	while (!grays.empty())
	{
		delete grays.front();
		grays.pop();
	}
	return pCurrent;
}

// searches enemy using BFS if not in same room
void SearchEnemy(Soldier* s, Team* t)
{
	Cell* enemy = runBFS(s, t);
	RunAStar(s, enemy->getRow(), enemy->getCol());
}

void pickState(Soldier* s, Team* self, Team* enemy)
{
	int state = s->getState();
	int hp = s->getHp();
	int numBullets = s->getNumBullets();
	int numGrenades = s->getNumGrenades();
	int medKits = s->getMedKits();
	int role = s->getRole();
	int lowestEnemyHP = 100;
	bool enemyInRoom = false;
	int countDead = 0;


	for (int i = 0; i < TEAM_SIZE; i++)
	{
		Soldier* enemySoldier = enemy->getSoldier(i);
		if (!enemySoldier->getIsAlive())
		{
			countDead++;
			continue;
		}
		if (enemySoldier->getRoom() == s->getRoom() && s->getRoom() != nullptr)
		{
			enemyInRoom = true;

		}
		if (enemySoldier->getHp() < lowestEnemyHP)
			lowestEnemyHP = enemy->getSoldier(i)->getHp();
	}

	if (countDead == 3)
	{
		runWar = false;

		return;
	}

	if (s->getRole() == MEDIC)
	{
		if (medKits < 2 || numBullets <= MIN_BULLETS || numGrenades <= MIN_GRENADES)
		{
			s->setState(RESUPPLY);
			return;
		}
	}

	switch (state)
	{
	case SEARCH:
	{
		if (enemyInRoom)
		{
			throwGrenade(s, enemy);
			s->setState(ATTACK);
		}
		break;
	}
	case ATTACK:
	{
		if (s->pickSupplyNeed() != -1) // need supply
		{
			throwGrenade(s, enemy);
			s->resetMissedShots();
			s->setState(DEFEND);
			return;
		}
		else {
			if (!enemyInRoom)
			{
				s->setState(SEARCH);
				s->resetMissedShots();
				s->setInCover(false);
				return;
			}
		}

		if ((hp >= lowestEnemyHP && lowestEnemyHP < ATTACK_THERSHOLD) ||
			(hp > ATTACK_THERSHOLD && numBullets > MIN_BULLETS && numGrenades > MIN_GRENADES))
		{
			return;
		}
		else
		{
			s->setInCover(false);
			s->resetMissedShots();
			s->setState(DEFEND);
		}

		break;
	}
	case DEFEND:
	{
		if (s->pickSupplyNeed() == -1) {
			throwGrenade(s, enemy);
			s->setState(ATTACK);
		}
		else
		{
			if (!self->getSoldier(MEDIC)->getIsAlive())
				s->setState(RESUPPLY);
		}
		break;
	}
	case RESUPPLY:
	{
		if (s->pickSupplyNeed() == -1)
		{
			s->setState(SEARCH);
		}
		break;

	}

	}
}

void transferSupply(Soldier* s, int supplyType, Team* team)
{

	printf("MEDIC is transferring supply: ", s->getRole());
	switch (supplyType)
	{
	case BULLET:
		s->giveBullet(team->getSoldier(MEDIC), s);
		printf("bullets\n");
		break;
	case GRENADE:
		s->giveGrenade(team->getSoldier(MEDIC), s);
		printf("grenades\n");
		break;
	case MEDKIT:
		s->giveMedKit(team->getSoldier(MEDIC), s);
		printf("medkit\n");
		break;

	default:
		break;

	}
}

Soldier* pickTarget(Soldier* s, Team* t)
{
	//check if enemy in line of sight with no WALLS in between
	int r = s->getY();
	int c = s->getX();
	for (int i = 0; i < TEAM_SIZE; i++)
	{
		if ((t->getSoldier(i)->getIsAlive()) == false || t->getSoldier(i)->getRoom() != s->getRoom())
			continue;
		int enemyR = t->getSoldier(i)->getY();
		int enemyC = t->getSoldier(i)->getX();

		// calc diagonal distance
		int distance = sqrt(pow(r - enemyR, 2) + pow(c - enemyC, 2));
		for (int j = 0; j <= distance; j++)
		{
			// calc point on line
			if (distance == 0)
				distance = 1;
			int y = r + (j * (enemyR - r)) / distance;
			int x = c + (j * (enemyC - c)) / distance;
			for (int j = 0; j <= distance; j++)
			{
				// calc point on line
				int y = r + (j * (enemyR - r)) / distance;
				int x = c + (j * (enemyC - c)) / distance;
				if (maze[y][x] == WALL)

				{
					int directions[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
					for (auto dir : directions) {
						int newRow = r + dir[0];
						int newCol = c + dir[1];
						if (maze[newRow][newCol] == SPACE)
							moveSoldier(s, new Cell(newRow, newCol, nullptr));
					}
					s->missedShot();
					return t->getSoldier(i);

				}
				if (x == enemyC && y == enemyR)
					return t->getSoldier(i);

			}
		}
	}
	//move soldier to a better attack position
	//if no enemy in line of sight return nullptr
	return nullptr;
}

void resetSecurityMap()
{
	for (int i = 0; i < MSZ; i++)
	{
		for (int j = 0; j < MSZ; j++)
		{
			if (maze[i][j] == WALL)
			{
				security_map[i][j] = WALL;
			}
			else
			{
				security_map[i][j] = SPACE;
			}
		}
	}
}

Soldier* findEnemy(int row, int col, Team* enemy) {
	for (int i = 0; i < TEAM_SIZE; i++) {
		if (enemy->getSoldier(i)->getX() == col && enemy->getSoldier(i)->getY() == row) {
			return enemy->getSoldier(i);
		}
	}
	return nullptr;

}

bool soldierCollide(Soldier* s, Team* self, Team* enemy)
{
	for (int i = 0; i < TEAM_SIZE; i++)
	{
		if (!self->getSoldier(i)->getIsAlive() || !enemy->getSoldier(i)->getIsAlive())
		{
			continue;
		}
		if (self->getSoldier(i)->getX() == s->getX() && self->getSoldier(i)->getY() == s->getY() && self->getSoldier(i)->getRole() != s->getRole())
		{
			return true;
		}
		if (enemy->getSoldier(i)->getX() == s->getX() && enemy->getSoldier(i)->getY() == s->getY())
		{
			return true;
		}
	}
	int directions[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
	for (auto dir : directions)
	{
		int newRow = s->getY() + dir[0];
		int newCol = s->getX() + dir[1];
		if (maze[newRow][newCol] == SOLDIER) {
			Soldier* enemySoldier = findEnemy(newRow, newCol, enemy);
			if (enemySoldier != nullptr) {
				if (enemySoldier->getInPath())
				{
					takeCover(s, self, enemy);
					return false;
				}
			}
			return true;
		}

	}

	return false;
}

void retreatSoldier(Soldier* s)
{
	Room* room = s->getRoom();
	int minDistance = 1000;
	//find closest room based on distance from centerX, centerY
	if (s->getRoom() == nullptr)
	{
		return;
	}
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		if (rooms[i] == s->getRoom())
			continue;
		int distance = sqrt(pow(s->getRoom()->getCenterX() - rooms[i]->getCenterX(), 2) + pow(s->getRoom()->getCenterY() - rooms[i]->getCenterY(), 2));
		if (distance < minDistance)
		{
			minDistance = distance;
			room = rooms[i];
		}
	}
	s->setTargetRoom(room);
	RunAStar(s, room->getCenterY(), room->getCenterX());
	s->setRetreat(true);
}

void attack(Soldier* s, Team* self, Team* enemy) {
	if (!s->getInCover())
	{
		takeCover(s, self, enemy);
		return;
	}
	// attack
	// Pick enemy soldier to attack
	Soldier* enemySoldier = pickTarget(s, enemy);
	// Attack enemy soldier
	if (enemySoldier != nullptr)
	{
		// shoot enemy
		s->shoot(enemySoldier);
		if (!enemySoldier->getIsAlive())
		{
			enemy->soldierKilled();
		}

	}
	if (s->getNumBullets() <= MIN_BULLETS || s->getHp() <= ATTACK_THERSHOLD)
	{
		//throw grenade
		if (s->getNumGrenades() > 0)
		{
			Cell* grenadeLocation = enemiesInRoom(s, enemy);
			if (grenadeLocation != nullptr)
			{
				Grenade* g = new Grenade(grenadeLocation->getRow(), grenadeLocation->getCol());
				grenades[grenadeClip] = g;
				grenadeClip++;
				printf("Grenade thrown\n");
			}
		}
	}
}

void defend(Soldier* s, Team* self, Team* enemy)
{
	// pick supply need
	int supplyType = s->pickSupplyNeed();
	// get supply
	//check if need medkit?
	if (self->getSoldier(MEDIC)->getIsAlive())
	{
		if (self->getSoldier(MEDIC)->getRoom() == s->getRoom())
		{
			transferSupply(s, supplyType, self);
		}
		else
		{
			Soldier* medic = self->getSoldier(MEDIC);
			int medicX = medic->getX();
			int medicY = medic->getY();
			// move to medic
			if (self->getSoldier(MEDIC)->getRoom() != nullptr)
				RunAStar(s, medic->getRoom()->getCenterY(), medic->getRoom()->getCenterX());
			else
				RunAStar(s, medicY, medicX);
		}
	}
	else
	{
		s->setState(RESUPPLY);
	}
	if (s->getMedKits() > 0 && s->getHp() <= ATTACK_THERSHOLD)
	{
		s->useMedKit();

	}
	// defend
}

void War()
{
	for (int j = 0; j < TEAM_SIZE; j++)
	{
		Soldier* s = teams[1]->getSoldier(j);
		if (s->getIsAlive())
		{
			maze[s->getY()][s->getX()] = SOLDIER;
		}
	}
	//check if either team has lost

	for (int i = 0; i < 2; i++)
	{
		Team* self = teams[i];
		Team* enemy = teams[(i + 1) % 2];
		//reset securityMap


		for (int j = 0; j < TEAM_SIZE; j++)
		{


			Soldier* s = teams[i]->getSoldier(j);

			//check if tile is occupied by another soldier

			//check if soldier is dead
			if (!s->getIsAlive())
			{
				maze[s->getY()][s->getX()] = DEAD;
				continue;
			}



			pickState(s, self, enemy);
			int state = s->getState();
			int supplyType;
			resetSecurityMap();
			GenerateSecurityMap(enemy);
			if (s->getRetreat())
			{

				if (s->getTargetRoom() == nullptr)
					//throw grenade
				{
					if (s->getNumGrenades() > 0)
					{
						Cell* grenadeLocation = enemiesInRoom(s, enemy);
						if (grenadeLocation != nullptr)
						{
							Grenade* g = new Grenade(grenadeLocation->getRow(), grenadeLocation->getCol());
							grenades[grenadeClip] = g;
							grenadeClip++;
							printf("Grenade thrown as soldier retreating\n");
						}
					}
					retreatSoldier(s);
					continue;
				}
				if (s->getTargetRoom() == s->getRoom())
				{
					s->setRetreat(false);
				}
				else
				{
					RunAStar(s, s->getTargetRoom()->getCenterY(), s->getTargetRoom()->getCenterX());
				}
				continue;
			}

			if (soldierCollide(s, self, enemy))
			{
				takeCover(s, self, enemy);
				continue;
			}


			switch (state)
			{
			case ATTACK:
			{
				attack(s, self, enemy);
				break;
			}

			case DEFEND:
				defend(s, self, enemy);
				break;
			case SEARCH:
				// search
				SearchEnemy(s, enemy);
				break;
			case RESUPPLY:
				// resupply
				//check if medic ia alive 
				if (self->getSoldier(MEDIC)->getIsAlive() && s->getRole() != MEDIC)
				{
					//move to medic
					RunAStar(s, self->getSoldier(MEDIC)->getY(), self->getSoldier(MEDIC)->getX());
				}
				else
					resupply(s, self);
				break;
			}
		}
	}
	// mark all living soldiers on maze
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < TEAM_SIZE; j++)
		{
			Soldier* s = teams[i]->getSoldier(j);
			if (s->getIsAlive())
			{
				if (i == 1) {
					maze[s->getY()][s->getX()] = ENEMY;
				}
				else {
					maze[s->getY()][s->getX()] = SOLDIER;
				}
			}
			else
			{
				maze[s->getY()][s->getX()] = DEAD;
			}
		}
	}
	for (int i = 0; i < 2; i++)
	{
		if (teams[i]->getNumSoldiers() == 0)
		{
			cout << "Team " << i + 1 << " has lost\n";
			runWar = false;
			return;
		}
	}

	for (int i = 0; i < grenadeClip; i++)
	{
		if (grenades[i] != nullptr)
		{
			grenades[i]->explode();
		}
	}
}

Room* findClosestSupplyRoom(Soldier* s, Team* t)
{
	// find closest room using manhattan distance
	Room* room = nullptr;
	int minDistance = 1000;
	int closestRoom = 0;
	for (int i = 0; i < SUPPLIES_SIZE; i++)
	{
		room = t->getSupplies(i)->getRoom();
		int distance = abs(s->getX() - room->getCenterX()) + abs(s->getY() - room->getCenterY());
		if (distance < minDistance &&
			(t->getSupplies(i)->getMedKits() > 0 ||
				t->getSupplies(i)->getBullets() > 0 ||
				t->getSupplies(i)->getGrenades() > 0))
		{
			minDistance = distance;
			closestRoom = i;
			room = t->getSupplies(i)->getRoom();
		}
	}

	return room;
}

void resupply(Soldier* s, Team* t) {
	Room* room = s->getTargetRoom();
	bool hasSupplies = false;

	if (room == nullptr)
	{
		room = findClosestSupplyRoom(s, t);
	}
	else
	{
		// check if we are in the room
		if (s->getRoom() == room)
		{
			Supplies* supplies = nullptr;
			int index_room = 0;
			for (int i = 0; i < SUPPLIES_SIZE; i++)
			{
				if (t->getSupplies(i)->getRoom() == room)
				{
					supplies = t->getSupplies(i);
					index_room = i;
					break;
				}
			}
			if (supplies == nullptr)
			{
				s->setTargetRoom(nullptr);
				return;
			}
			supplies->takeSupplies(s, s->pickSupplyNeed());
			s->setTargetRoom(nullptr);
		}
		else
		{
			RunAStar(s, room->getCenterY(), room->getCenterX());
		}
		return;
	}
	//    check if room has supplies
	Supplies* supplies = nullptr;
	int index_room = 0;
	for (int i = 0; i < SUPPLIES_SIZE; i++)
	{
		if (t->getSupplies(i)->getRoom() == room)
		{
			supplies = t->getSupplies(i);
			index_room = i;
			break;
		}
	}
	if (supplies == nullptr)
	{
		return;
	}
	int supplyType = s->pickSupplyNeed();
	switch (supplyType)
	{
	case MEDKIT:
		if (supplies->getMedKits() > 0)
		{
			hasSupplies = true;
		}
		break;
	case BULLET:
		if (supplies->getBullets() > 0)
		{
			hasSupplies = true;
		}
		break;
	case GRENADE:
		if (supplies->getGrenades() > 0)
		{
			hasSupplies = true;
		}
		break;
	}

	if (!hasSupplies)
	{
		room = t->getSupplies((index_room + 1) % 2)->getRoom();

	}
	s->setTargetRoom(room);
	RunAStar(s, room->getCenterY(), room->getCenterX());


}


void RunAStar(Soldier* s, int targetRow, int targetCol) {
	priority_queue<Cell*, vector<Cell*>, CompareCells> pqc;
	Cell* Start = new Cell(s->getY(), s->getX(), targetRow, targetCol, 0, nullptr);
	Start->ComputeH();
	Start->setF();
	pqc.push(Start);
	Room* currentRoom = s->getRoom();
	int directions[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

	while (!pqc.empty()) {
		Cell* pCurrent = pqc.top();
		pqc.pop();

		// set current cell to black
		int currentRow = pCurrent->getRow();
		int currentCol = pCurrent->getCol();

		security_map[currentRow][currentCol] = BLACK;

		// check neighbors
		for (auto dir : directions) {
			int newRow = currentRow + dir[0];
			int newCol = currentCol + dir[1];

			// Add boundary checks
			if (newRow >= 0 && newRow < MSZ && newCol >= 0 && newCol < MSZ) {
				//check if path is clear
				if (maze[newRow][newCol] == PATH && !s->getInPath())
				{
					// find Path
					Path* temp = findPath(newRow, newCol);
					// check if path is open
					if (temp != nullptr && temp->getOccupied())
					{
						continue;
					}
				}
				if ((security_map[newRow][newCol]) != BLACK &&
					(maze[newRow][newCol]) != WALL &&
					(security_map[newRow][newCol]) != GRAY)
				{
					Cell* neighbor = new Cell(newRow, newCol, targetRow, targetCol, pCurrent->getG() + 1, pCurrent);
					neighbor->updateG((int)security_map[newRow][newCol]);
					neighbor->ComputeH();
					neighbor->setF();
					pqc.push(neighbor);
					// mark as gray
					security_map[newRow][newCol] = GRAY;

					// check if we reached the target
					if (newRow == targetRow && newCol == targetCol) {


						moveSoldier(s, neighbor);
					}
				}
			}
		}
	}
	while (!pqc.empty()) {
		Cell* p = pqc.top();
		pqc.pop();
		delete p;
	}

}


