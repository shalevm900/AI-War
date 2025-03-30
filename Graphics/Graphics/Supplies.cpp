#include "Supplies.h"

Supplies::Supplies(Room* room)
{
    this->room = room;
    medKits = 30;
    Bullets = 500;
    Grenades = 60;
}
void Supplies::takeSupplies(Soldier* s, int supply)
{
    switch (supply)
    {
	case MEDKIT:
		takeMedKit(s);
		break;
	case BULLET:
		takeBullet(s);
		break;
	case GRENADE:
		takeGrenade(s);
		break;
    }
}
void Supplies::takeMedKit(Soldier* s)
{
    int takeMedkits = MAX_MEDKITS - s->getMedKits();
	if (medKits > takeMedkits)
	{
		s->setMedKits(s->getMedKits() + takeMedkits);
		medKits -= takeMedkits;
	}
	else
	{
		s->setMedKits(s->getMedKits() + medKits);
		medKits = 0;
	}
}

void Supplies::takeBullet(Soldier* s)
{
    int takeBullets = MAX_BULLETS - s->getNumBullets();
    if (Bullets > takeBullets)
    {
        s->setNumBullets(s->getNumBullets()+takeBullets);
        Bullets -= takeBullets;
    }
    else
    {
        s->setNumBullets(s->getNumBullets() + Bullets);
        Bullets = 0;
    }
}

void Supplies::takeGrenade(Soldier* s)
{
    int takeGrenades = MAX_GRENADES - s->getNumGrenades();
    if (Grenades > takeGrenades)
    {
        s->setNumGrenades(s->getNumGrenades() + takeGrenades);
        Grenades -= takeGrenades;
    }
    else
    {
        s->setNumGrenades(s->getNumGrenades() + Grenades);
        Grenades = 0;
    }
}
