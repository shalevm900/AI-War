#pragma once
#include <math.h>
class Cell
{
private:
	int row, col;
	int target_row, target_col;
	Cell* parent;
	double f, g, h;
//	double 
public:
	Cell();
	Cell(int r, int c, Cell* p);
	Cell(int r, int c,int tr,int tc, double newg,Cell* p);
	int getRow() { return row; }
	int getCol() { return col; }
	Cell* getParent() { return parent; }
	void ComputeH();
    void setF() { f = g+h; }
    void updateG() { g++; }
	void updateG(int weight) { g += weight; }
	double getF() { return f; }
	double getH() { return h; }
	double getG() { return g; }
	int getTargetRow() { return target_row; }
	int getTargetCol() { return target_col; }

	bool operator == (const Cell& other) { return row == other.row && col == other.col; }

};

