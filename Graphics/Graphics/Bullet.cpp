#include "Bullet.h"
#include <math.h>
#include "glut.h"

Bullet::Bullet(double xx, double yy, double target_x, double target_y)
{
	x = xx;
	y = yy;
	double dirXX = target_x - x;
	double dirYY = target_y - y;
	angle = atan2(dirYY, dirXX);
	dirX = cos(angle);
	dirY = sin(angle);
	speed = 0.5;
	isMoving = true;
}

Bullet::Bullet(double xx, double yy, double angle) {
	x = xx; 
	y = yy; 
	this->angle = angle; 
	dirX = cos(angle); 
	dirY = sin(angle); 
	speed = 0.9;
	isMoving = true; 
}

void Bullet::move(int maze[MSZ][MSZ])
{
	// move the bullet according to its direction and speed 
	if (isMoving)
	{
		x += speed * dirX;
		y += speed * dirY;
		if (maze[(int)y][(int)x] == WALL || maze[(int)y][(int)x] == SOLDIER)
		{
			isMoving = false;
			//delete this;
		}
	}
}

void Bullet::show()
{
	// draw bullet as a red square of size 1x1 at (x,y) location in the maze 
	glColor3d(1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2d(x - 0.5, y);
	glVertex2d(x , y+ 0.5);
	glVertex2d(x + 0.5, y);
	glVertex2d(x , y- 0.5);
	glEnd();
}

void Bullet::SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ])
{
	isMoving = true;
	while (isMoving)
	{
		sm[(int)y][(int)x] += SECURITY_FACTOR;
		move(maze);
	}
}

