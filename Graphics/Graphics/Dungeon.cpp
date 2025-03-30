//
// Created by Omer Ishai on 29/03/2025.
//

#include "Dungeon.h"

int maze[MSZ][MSZ] = { 0 }; // WALLs
double security_map[MSZ][MSZ] = { 0 }; //
Room* rooms[NUM_ROOMS];
Bullet* bullets[6];
int bulletClip = 0;
Grenade* grenades[6];
int grenadeClip = 0;
bool runWar = false;
Team* teams[2];
bool greandeThrown = false;
int baseMap[MSZ][MSZ] = { 0 };
Path* paths[MAX_PATHS];
int numPaths = 0;

void RestorePath(Cell* pc)
{
    //savePath(pc);
    int startX = pc->getCol();
    int startY = pc->getRow();
    int r = 0;
    int c = 0;

    while (pc != nullptr)
    {
        r = pc->getRow();
        c = pc->getCol();
        if (maze[r][c] == WALL)
            maze[r][c] = PATH;
        pc = pc->getParent();
    }

}


Path* findPath(int row, int col)
{
    for (int i = 0; i < numPaths; i++)
    {
        if (paths[i]->contains(row, col))
        {
            return paths[i];
        }
    }
    return nullptr;
}

// row, col are the indices of neighbor cell
void AddNeighbor(int r, int c, Cell* pCurrent,
    priority_queue<Cell*, vector<Cell*>, CompareCells>& pq,
    vector <Cell>& grays, vector <Cell>& black) // blacks shouldn't be changed
{
    double newg, cost;
    vector<Cell>::iterator itGray;
    vector<Cell>::iterator itBlack;


    if (maze[r][c] == WALL) cost = WALL_COST;
    else cost = SPACE_COST;
    newg = pCurrent->getG() + cost;
    Cell* pNeighbor = new Cell(r, c, pCurrent->getTargetRow(), pCurrent->getTargetCol(),
        newg, pCurrent);
    // check what to do with the neighbor Cell
    // 1. if the neighbor is black: do nothing
    // 2. if the neighbor is white: add it to PQ and to grays
    // 3. if it is gray:
    // 3.1 if F of neighbor is not below the neighbor copy that is stored in PQ then do nothing
    // 3.2 otherwise then we must update the PQ and grays
    itGray = find(grays.begin(), grays.end(), *pNeighbor);
    itBlack = find(black.begin(), black.end(), *pNeighbor);

    if (itBlack == black.end()) // then it is not black
    {
        if (itGray == grays.end()) // then it is not gray => it is white
        {
            // paint it gray
            pq.push(pNeighbor);
            grays.push_back(*pNeighbor);
        }
        else // it is gray
        {
            if (pNeighbor->getF() < itGray->getF()) // then we found a better path and we have to exchange it
            {
                grays.erase(itGray);
                grays.push_back(*pNeighbor);

                // and do the same with PQ
                vector<Cell*> tmp;
                while (!pq.empty() && !((*pq.top()) == (*pNeighbor)))
                {
                    tmp.push_back(pq.top()); // save the top element in tmp
                    pq.pop(); // remove top element from pq
                }
                if (pq.empty()) // ERROR!
                {
                    cout << "PQ is empty\n";
                    exit(2);
                }
                else // we have found the neighbor cell in PQ
                {
                    pq.pop(); // remove old neighbor from pq
                    pq.push(pNeighbor);
                    // now restore pq
                    while (!tmp.empty())
                    {
                        pq.push(tmp.back());
                        tmp.pop_back();
                    }
                }
            }
        }
    }


}


// run A* from room at index1 to room at index2
void BuildPath(int index1, int index2)
{
    int r, c, tr, tc;

    r = rooms[index1]->getCenterY();
    c = rooms[index1]->getCenterX();
    tr = rooms[index2]->getCenterY();
    tc = rooms[index2]->getCenterX();
    Cell* pCurrent;
    Cell* start = new Cell(r, c, tr, tc, 0, nullptr);
    priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
    vector <Cell> grays;
    vector <Cell> black;
    vector<Cell>::iterator itGray;

    pq.push(start);
    grays.push_back(*start);
    // pq shouldn't be empty because we are going to reach the target beforehand
    while (!pq.empty())
    {
        pCurrent = pq.top();
        if (pCurrent->getH() < 0.001) // this is a target cell
        {
            RestorePath(pCurrent);

            return;
        }
        else // target hasn't been reached
        {
            // 1. remove pCurrent from pq
            pq.pop();
            // 2. find and remove pCurrent from grays
            itGray = find(grays.begin(), grays.end(), *pCurrent);
            if (itGray == grays.end()) // pCurrent wasn't found
            {
                cout << "Error: pCurrent wasn't found in grays\n";
                exit(1);
            }
            grays.erase(itGray);
            // 3. paint pCurrent black
            black.push_back(*pCurrent);
            // 4. take care of neighbors
            r = pCurrent->getRow();
            c = pCurrent->getCol();
            // up
            if (r + 1 < MSZ)
                AddNeighbor(r + 1, c, pCurrent, pq, grays, black);
            // down
            if (r - 1 >= 0)
                AddNeighbor(r - 1, c, pCurrent, pq, grays, black);
            // left
            if (c - 1 >= 0)
                AddNeighbor(r, c - 1, pCurrent, pq, grays, black);
            // right
            if (c + 1 < MSZ)
                AddNeighbor(r, c + 1, pCurrent, pq, grays, black);
        }

    }
    cout << "Error pq is empty, target hasn't been found\n";
}

bool isValid(int row, int col) {
    return row >= 0 && row < MSZ && col >= 0 && col < MSZ;
}

bool visited[MSZ][MSZ];

const int dRow[] = { 1, -1, 0, 0 };
const int dCol[] = { 0, 0, 1, -1 };

void followPath(int row, int col) {
    std::vector<std::pair<int, int>> points;
    points.emplace_back(row, col);
    visited[row][col] = true;

    while (true) {
        int nextRow = -1, nextCol = -1, neighbors = 0;

        for (int i = 0; i < 4; ++i) {
            int r = row + dRow[i], c = col + dCol[i];
            if (r >= 0 && r < MSZ && c >= 0 && c < MSZ &&
                maze[r][c] == PATH && !visited[r][c]) {
                nextRow = r;
                nextCol = c;
                neighbors++;
            }
        }

        if (neighbors != 1) break;  // stop at junctions or dead ends

        row = nextRow;
        col = nextCol;
        visited[row][col] = true;
        points.emplace_back(row, col);
    }

    if (points.size() < 2 || numPaths >= MAX_PATHS) return;

    if (points.size() == 3) {
        paths[numPaths++] = new Path(points[0].first, points[0].second,
            points[1].first, points[1].second,
            points[2].first, points[2].second);
    }
    else {
        paths[numPaths++] = new Path(points.front().first, points.front().second,
            points.back().first, points.back().second);
    }
}

void FindAllPaths() {
    numPaths = 0;
    std::fill(&visited[0][0], &visited[0][0] + MSZ * MSZ, false);

    for (int row = 0; row < MSZ; ++row) {
        for (int col = 0; col < MSZ; ++col) {
            if (maze[row][col] == PATH && !visited[row][col]) {
                followPath(row, col);
            }
        }
    }

    for (int i = 0; i < numPaths; ++i) {
        Path* p = paths[i];
        printf("Path Found: (%d,%d) -> (%d,%d)", p->getY(), p->getX(), p->getTargetY(), p->getTargetX());
        if (p->getHasThird()) {
            printf(" and (%d,%d)", p->getThirdY(), p->getThirdX());
        }
        printf("\n");
    }
    printf("Total paths found: %d\n", numPaths);
}



void BuildPathBetweenTheRooms()
{
    int i, j;

    for (i = 0; i < NUM_ROOMS; i++)
        for (j = i + 1; j < NUM_ROOMS; j++)
        {
            BuildPath(i, j); // A*
            cout << "The path from " << i << " to " << j << " has been paved\n";
        }

}

void placeSoldier(Team* teams[2])
{
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < TEAM_SIZE; j++)
        {
            maze[teams[i]->getSoldier(j)->getY()][teams[i]->getSoldier(j)->getX()] = SOLDIER;
        }
    }
    // place supplies
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < SUPPLIES_SIZE; j++)
        {
            maze[teams[i]->getSupplies(j)->getRoom()->getCenterY()][teams[i]->getSupplies(j)->getRoom()->getCenterX()] = SUPPLY;
        }
    }
}

void placeSoldiers(Room* rooms[NUM_ROOMS], Team* teams[2])
{
    int maxDistance = 0;
    int distance;
    int roomIndex = 0;
    Room* room = teams[0]->getSoldier(0)->getRoom();
    for (int i = 0; i < NUM_ROOMS; i++)
    {
        if (rooms[i] == room)
            continue;
        distance = (int)(sqrt(pow(room->getCenterX() - rooms[i]->getCenterX(), 2) + pow(room->getCenterY() - rooms[i]->getCenterY(), 2)));
        if (distance > maxDistance)
        {
            maxDistance = distance;
            roomIndex = i;
        }
    }
    room = rooms[roomIndex];
    Soldier* soldiers[TEAM_SIZE];
    for (int i = 0; i < TEAM_SIZE; i++)
    {
        soldiers[i] = teams[1]->getSoldier(i);
    }
    for (int i = 0; i < TEAM_SIZE; i++)
    {
        int x = rand() % room->getWidth() + room->getCenterX() - room->getWidth() / 2;
        int y = rand() % room->getHeight() + room->getCenterY() - room->getHeight() / 2;

        soldiers[i]->setX(x);
        soldiers[i]->setY(y);
        soldiers[i]->setRoom(room);
    }

}

void createTeams()
{
    for (int i = 0; i < 2; i++)
    {
        teams[i] = new Team(rooms);

    }
}

void SetupDungeon()
{
    int i, j;
    int cx, cy, w, h;
    bool hasOverlap;
    for (i = 0; i < NUM_ROOMS; i++)
    {
        do
        {
            hasOverlap = false;
            w = 6 + rand() % (MSZ / 5);
            h = 6 + rand() % (MSZ / 5);
            cx = 2 + w / 2 + rand() % (MSZ - w - 4);
            cy = 2 + h / 2 + rand() % (MSZ - h - 4);
            for (j = 0; j < i && !hasOverlap; j++)
                hasOverlap = rooms[j]->Overlap(cx, cy, w, h);
        } while (hasOverlap); // check the validity of the room

        rooms[i] = new Room(cx, cy, w, h, maze);
    }
    createTeams();



    // create the random obstacles in the maze
    for (i = 0; i < 500; i++) {
        maze[rand() % MSZ][rand() % MSZ] = WALL;
    }

    BuildPathBetweenTheRooms();

    FindAllPaths();


    placeSoldiers(rooms, teams);
    placeSoldier(teams);

    for (int i = 0; i < MSZ; i++)
        for (int j = 0; j < MSZ; j++)
        {
            baseMap[i][j] = maze[i][j];
            if (maze[i][j] == SOLDIER)
            {
                baseMap[i][j] = SPACE;
            }
        }

    //    printAllMaze();
}

void ShowDungeon()
{
    int i, j;
    double s;

    for (i = 0; i < MSZ; i++)
        for (j = 0; j < MSZ; j++)
        {
            //s = security_map[i][j];
            //1. set color of cell
            switch (maze[i][j])
            {
            case SPACE:
                glColor3d(1, 1, 1); // white
                break;
            case PATH:
                glColor3d(1, 1, 1); // white
                break;
            case WALL:
                glColor3d(0.3, 0.3, 0.4); // dark gray
                break;
            case DEAD:
                glColor3d(0.5, 0, 0); // darek red
                break;
            case SUPPLY:
                glColor3d(0, 0, 1); // blue
                break;
            case SOLDIER:
                glColor3d(0, 1, 0); // green
                break;
            case ENEMY:
                glColor3d(1, 0.5, 0);// orange
                break;

            }
            // show cell
            glBegin(GL_POLYGON);
            glVertex2d(j, i);
            glVertex2d(j, i + 1);
            glVertex2d(j + 1, i + 1);
            glVertex2d(j + 1, i);
            glEnd();
        }


}

void GenerateSecurityMap(Team* enemy)
{
    int numSimulations = 1000;
    int i;

    for (i = 0; i < numSimulations; i++)
    {
        int x = rand() % MSZ;
        int y = rand() % MSZ;
        Grenade* g = new Grenade(rand() % MSZ, rand() % MSZ);

        g->SimulateExplosion(baseMap, security_map);
        delete g;
    }
}

Cell* enemiesInRoom(Soldier* s, Team* enemy)
{
    Room* room = s->getRoom();
    int averageX = 0;
    int averageY = 0;
    int count = 0;
    for (int i = 0; i < TEAM_SIZE; i++)
    {
        if (enemy->getSoldier(i)->getIsAlive() && enemy->getSoldier(i)->getRoom() == room)
        {
            averageX += enemy->getSoldier(i)->getX();
            averageY += enemy->getSoldier(i)->getY();
            count++;
        }
    }

    if (count == 0)
        return nullptr;
    averageX /= count;
    averageY /= count;

    // add some randomness to the location
    averageX += rand() % 4 - 1;
    averageY += rand() % 4 - 1;

    return new Cell(averageY, averageX, nullptr);
}

void throwGrenade(Soldier* s, Team* enemy)
{
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
        for (int i = 0; i < TEAM_SIZE; i++)
        {
            if (s->getRoom() == enemy->getSoldier(i)->getRoom())
            {
                enemy->getSoldier(i)->decreaseHp(3);
            }
        }
    }
}