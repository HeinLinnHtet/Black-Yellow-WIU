#include "GOPrizeStock.h"
#include <iostream>

GOPrizeStock::GOPrizeStock()
{
	Stock = 0;
	Columns = 0;
	Rows = 0;
}

GOPrizeStock::GOPrizeStock(int NumToCreate, GOPrize Item, int col, int rows)
{
	int itemcount = 0;

	StockedItem = Item;
	Stock = NumToCreate;
	Columns = col;
	Rows = rows;
	
	for (int i = 0; i < (NumToCreate / col); i++)
	{
		for (int j = 0; j < (NumToCreate / rows); j++)
		{
			
			prizearray[itemcount] = Item;
			prizearray[itemcount].position.x = Item.position.x - (i * (Item.dimensions.x / 2));
			prizearray[itemcount].position.z = Item.position.z - (j * (Item.dimensions.z / 2));
			itemcount++;
		}
	}
}

GOPrizeStock::~GOPrizeStock()
{
}

int GOPrizeStock::GetStock()
{
	return Stock;
}

int GOPrizeStock::GetCols()
{
	return Columns;
}

int GOPrizeStock::GetRows()
{
	return Rows;
}

void GOPrizeStock::SetStock(int newstock, GOPrize Item)
{
	if (Stock > newstock)
	{
		int itemssold = Stock - newstock;

		if (newstock != 0)
		{
			for (int i = 0; i < itemssold; i++)
			{
				prizearray[Stock].bought = true;
				std::cout << Stock << " - " << newstock << std::endl;
				Stock--;
				std::cout << "Item has been purchased" << std::endl;
				break;
			}
		}
		else
		{
			prizearray[0].bought = true;
			std::cout << Stock << " - " << newstock << std::endl;
			Stock--;
			std::cout << "Out of stock!" << std::endl;
		}
	}
	else if (Stock < newstock)
	{
		for (int i = 0; i < (newstock - Stock); i++)
		{
			prizearray[Stock + 1] = Item;
			Stock++;
		}
	}
}
