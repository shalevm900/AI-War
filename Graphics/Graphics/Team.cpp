#include "Team.h"
#include <stdlib.h>

Team::Team(Room** roomList)
{
    int r = rand() % NUM_ROOMS;
    for (int i = 0; i < SUPPLIES_SIZE; i++)
    {
        // get random room
        while (roomList[r]->getUsedForSupply())
        {
            r = rand() % NUM_ROOMS;
        }
        // set room as used for supply
        roomList[r]->setUsedForSupply(true);
        // set supplies in the center of the room and add to supplies array
        supplies[i] = new Supplies(roomList[r]);  
    }

    // Choose respawn room
    int respawn_room = rand() % SUPPLIES_SIZE;
    Room* respawn = supplies[respawn_room]->getRoom();

    // Assign soldiers
    for (int i = 0; i < TEAM_SIZE; i++)
    {
        soldiers[i] = new Soldier(i);

        int x = rand() % respawn->getWidth() + respawn->getCenterX() - respawn->getWidth() / 2;
        int y = rand() % respawn->getHeight() + respawn->getCenterY() - respawn->getHeight() / 2;

        soldiers[i]->setX(x);
        soldiers[i]->setY(y);
        soldiers[i]->setRoom(respawn);  
    }
}
