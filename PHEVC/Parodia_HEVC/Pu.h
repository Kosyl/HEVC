#ifndef PB_H
#define PB_H

#include <cassert>
#include "Utils.h"
#include "Cu.h"

class Pu;

class Pb
{
private:
	const Pu *pu;
	ImgComp comp;
	int const* const *picRecon;
	int getReferenceAtPosition(const Direction, const int = 0) const;
	bool calcPuAvail(const int, const int) const;
	int getReconRef(const Direction, const int = 0) const;
	int getRefSubs(const Direction) const;
public:
	Pb(const Pu*, ImgComp comp);
	~Pb();
	void setPu(const Pu*);
	void setPicRecon(int const* const*);

	ImgComp getImgComp()  const;
	int getPuIdx() const;
	int getPuSize() const;
	int getModeIdx() const;
	int getPuX() const;
	int getPuY() const;

	int getCorner() const;
	int *getSideRefs(const Direction) const;
	int **getPred() const;
	int **getPredForceRef(int *leftRefs, int *topRefs, int corner) const;
};

class Pu
{
private:
	Cu *cu;
	Pb **blocks;
	int puX, puY;
	int puIdx, puSize, modeIdx;
	int calcPuIdx(const int, const int) const;
public:
	Pu(Cu* cu, const int X, const int Y);
	~Pu();
	void setCu(Cu*);
	void setPuIdx(const int);
	void setPuSize(const int);
	void setModeIdx(const int);
	void setPicRecon(Frame* frame);

	Cu *getCu() const;
	int getPuIdx() const;
	int getPuSize() const;
	int getModeIdx() const;
	int getPuX() const;
	int getPuY() const;

	int** getPred(ImgComp comp);
};

#endif