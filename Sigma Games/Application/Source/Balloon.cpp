#include "Balloon.h"

Balloon::Balloon()
{
	pos = { 0,0,0 };
	mass = 10;
	bounciness = 0;
}

Balloon::Balloon(float x, float y, float z, int pts)
{
	pos = { x,y,z };
	mass = 10;
	bounciness = 0;
	points = pts;
}

Balloon::~Balloon()
{
}
