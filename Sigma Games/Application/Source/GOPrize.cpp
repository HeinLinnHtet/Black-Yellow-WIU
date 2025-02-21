#include "GOPrize.h"

GOPrize::GOPrize()
{
	cost = 10;
	name = "Default Name";
	description = "This is the description of the product";
	bought = false;
	dimensions = { 1,1,1 };
	position = { 0, 0, 0 };
}

GOPrize::GOPrize(int newcost, std::string prizename, std::string desc, float w, float d, float h, float x, float z, float y)
{
	cost = newcost;
	name = prizename;
	description = desc;
	bought = false;
	position = { x, y, z };
	dimensions = { w, h, d };
}

GOPrize::~GOPrize()
{
}
