
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <queue>
#include "Cell.h"
#include <iostream>
#include "Room.h"
#include "Soldier.h"
#include "CompareCells.h"
#include "Bullet.h"
#include "Grenade.h"
#include "generalFunctions.h"



using namespace std;

const int WIDTH = 700;
const int HEIGHT = 700;


bool bulletFired = false;
bool grenadeThrown = false;
Bullet* pb=nullptr;
Grenade* pg = nullptr;
bool isObjectPhase = false;



void init()
{
	glClearColor(0.5, 0.5, 0.5, 0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system

	srand(time(0));

	SetupDungeon();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	ShowDungeon();
	//show 
	if (pg != nullptr)
		pg->show();

	if (bulletClip > 0 || grenadeClip > 0)
	{
		for (int i = 0; i < 6; i++)
		{
			if (bullets[i] != nullptr)
			{
				bullets[i]->show();
			}
			if(grenades[i] != nullptr)
			{
				grenades[i]->show();
			}
		}
	}

	glutSwapBuffers(); // show all
}

void processObjects()
{
	bool objectStillMoving = false;

	// Move each active bullet one step
	for (int i = 0; i < 6; i++)
	{
		if (bullets[i] != nullptr && bullets[i]->getIsMoving())
		{
			bullets[i]->move(maze);
			//bullets[i]->show();

			// If bullet stopped moving, clean it up
			if (!bullets[i]->getIsMoving())
			{
				bulletClip--;
				delete bullets[i]; // Properly deallocate
				bullets[i] = nullptr;
			}
			else
			{
				objectStillMoving = true;
			}
		}
		if (grenades[i] != nullptr)
		{
			bool moving = false;
			grenades[i]->expand(maze);
			for (int j = 0; j < NUM_BULLETS; j++)
			{
				if (grenades[i]->getBulletsArray()[j]->getIsMoving())
				{
					moving = true;
					objectStillMoving = true;
				}

			}
			if (!moving)
			{
				grenadeClip--;
				delete grenades[i];
				grenades[i] = nullptr;
			}

		}
		// If no bullets are moving, switch to soldier phase
		if (!objectStillMoving)
		{
			isObjectPhase = false;
		}
	}
}

void idle() 
{

	if (grenadeThrown)
		pg->expand(maze);

	if(bulletClip > 0 || grenadeClip > 0)
		isObjectPhase = true;
	if (isObjectPhase)
	{
		processObjects();
	}
	else
	{
		if (runWar)
		{
			War();
//			Sleep(100);
		}
	}

	glutPostRedisplay();
}

void menu(int choice)
{
	switch (choice)
	{
	case 1: // fire bullet
		bulletFired = true;
		pb->setIsMoving(true);
		break;
	case 2: // throw grenade
		grenadeThrown = true;
		pg->explode();
		break;
	case 3: // security map
		//GenerateSecurityMap();
		break;
	case 4:
		runWar = true;
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		pb = new Bullet(MSZ*x/(double)WIDTH, MSZ* (HEIGHT - y) / (double)HEIGHT, 0, 0);

		pg = new Grenade(MSZ * (HEIGHT - y) / (double)HEIGHT, MSZ * x / (double)WIDTH);
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	// definitions for visual memory (Frame buffer) and double buffer
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(600, 20);
	glutCreateWindow("WAR GAME");

	// display is a refresh function
	glutDisplayFunc(display);
	// idle is a update function
	glutIdleFunc(idle);
	
	glutMouseFunc(mouse);

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Fire bullet", 1);
	glutAddMenuEntry("Throw Grenade", 2);
	glutAddMenuEntry("Generate Security Map", 3);
	glutAddMenuEntry("War", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	init();



	glutMainLoop();
    return 0;
}