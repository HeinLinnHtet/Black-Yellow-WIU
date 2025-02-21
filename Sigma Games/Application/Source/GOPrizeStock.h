#pragma once
#include <vector>
#include "Windows.h"
#include <iostream>
#include "GOPrize.h"

class GOPrizeStock
{
private:
	int Stock;
	int Columns;
	int Rows;
public:
	GOPrizeStock();
	GOPrizeStock(int NumToCreate, GOPrize Item, int col, int rows);
	~GOPrizeStock();

	int GetStock();
	int GetCols();
	int GetRows();
	void SetStock(int newstock, GOPrize Item);

	GOPrize StockedItem;
	GOPrize prizearray[99];
};

