#include "Pu.h"
#include "Cu.h"
#include "Utils.h"


Cu::Cu(Frame *newFrame, const int newCuX, const int newCuY, const int newCuSize):
	puList(), tuList()
{
	setCuX(newCuX);
	setCuY(newCuY);
	setCuSize(newCuSize);
	frame = newFrame;
}

Cu::~Cu()
{
	for (int i = 0; i < puList.size(); ++i)
		delete puList[i];

	puList.clear();

	for (int i = 0; i < tuList.size(); ++i)
		delete tuList[i];

	tuList.clear();
}

void Cu::setCuX(const int newCuX)
{
	assert(newCuX >= 0);
	cuX = newCuX;
}

void Cu::setQP(const int newQP)
{
	assert(newQP >= 0);
	QP = newQP;
}

void Cu::setCuY(const int newCuY)
{
	assert(newCuY >= 0);
	cuY = newCuY;
}

void Cu::setCuSize(const int newCuSize)
{
	assert( ( newCuSize == 8 || newCuSize == 16 || newCuSize == 32 || newCuSize == 64 ) && (SeqParams::getInstance())->getMaxCuSize() >= newCuSize );
	cuY = newCuSize;
}

void Cu::addPu(Pu& newPu)
{
	//TODO sprawdzanie poprawnosci nowego warunku
	//std::vector<Pu*>::iterator iter = puList.begin();
	//while(iter != puList.end())
	//{

	//	assert((*iter)->getPuX() != newPu.getPuX() || (*iter)->getPuY() != newPu.getPuY());//nie jest w tym samym miejscu
	//	assert(newPu.getPuSize() < newPu.getPuX()-cuX
	//}
	newPu.setPicRecon(frame);
	puList.push_back(&newPu);
}

int Cu::getCuX() const
{
	return cuX;
}

int Cu::getCuY() const
{
	return cuY;
}

int Cu::getCuSize() const
{
	return cuSize;
}

int Cu::getQP() const
{
	return QP;
}