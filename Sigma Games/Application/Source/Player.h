#pragma once
#include "GOPrizeStock.h"

class GOPlayer
{
public:
	GOPlayer();
	~GOPlayer();
	int ticketcount = 1000;
	GOPrizeStock Inventory;
};

