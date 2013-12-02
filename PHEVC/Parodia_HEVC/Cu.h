#ifndef CU_H
#define CU_H

#include <cassert>
#include <vector>
#include "Utils.h"
#include "Frame.h"

class Pu;
class Tu;

class Cu
{
private:
	Frame* frame;
	int cuX, cuY;
	int cuSize;
	int QP;
	std::vector<Pu*> puList;
	std::vector<Tu*> tuList;
public:
	Cu(Frame* frame, const int X = 0, const int Y = 0, const int size = 4);
	~Cu();

	void setCuX(const int X);
	void setCuY(const int Y);
	void setCuSize(const int size);
	void setQP(const int QP);

	void addPu(Pu &);

	int getCuX() const;
	int getCuY() const;
	int getCuSize() const;
	int getQP() const;
};

#endif