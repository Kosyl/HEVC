#include "Pu.h"
#include "IntraPred.h"

Pu::Pu(Cu *newCu, const int X, const int Y)
{
	assert( X >= 0 && Y >= 0);
	setCu(newCu);
	puX=X;
	puY=Y;
	puIdx = calcPuIdx(X,Y);
	blocks = new Pb*[3];
	blocks[0] = new Pb(this, LUMA);
	blocks[1] = new Pb(this, CB);
	blocks[2] = new Pb(this, CR);
}

Pu::~Pu()
{
	if(blocks != nullptr)
	{
		for(int i=0;i<3;++i)
			delete blocks[i];
		delete[] blocks;
		blocks=nullptr;
	}
}

int Pu::calcPuIdx(const int puX, const int puY) const
{
	int puXDivBy4 = puX >> 2;
	int puYDivBy4 = puY >> 2;
	int calcPuIdx = ((puXDivBy4 & 8) << 3) | ((puXDivBy4 & 4) << 2) | ((puXDivBy4 & 2) << 1) | (puXDivBy4 & 1);
	calcPuIdx |= ((puYDivBy4 & 8) << 4) | ((puYDivBy4 & 4) << 3) | ((puYDivBy4 & 2) << 2) | ((puYDivBy4 & 1) << 1);
	return calcPuIdx;
}

void Pu::setCu(Cu *newCu)
{
	assert(newCu != nullptr);
	cu = newCu;
}

void Pu::setPuIdx(const int newPuIdx)
{
	assert(newPuIdx >= 0);
	puIdx = newPuIdx;
}

void Pu::setPuSize(const int newPuSize)
{
	assert((newPuSize >= 4) && (newPuSize <= 32));
	puSize = newPuSize;
}

void Pu::setModeIdx(const int newModeIdx)
{
	assert((newModeIdx >= 0) && (newModeIdx <= 34));
	modeIdx = newModeIdx;
}

void Pu::setPicRecon(Frame* frame)
{
	assert(frame != nullptr);
	int puX = getPuX(), puY = getPuY();
	blocks[LUMA]->setPicRecon(frame->reconMatrix(LUMA));
	blocks[CR]->setPicRecon(frame->reconMatrix(CR));
	blocks[CB]->setPicRecon(frame->reconMatrix(CB));
}

Cu* Pu::getCu() const
{
	return cu;
}

int Pu::getPuIdx() const
{
	assert(puIdx >= 0);
	return puIdx;
}

int Pu::getPuSize() const
{
	assert((puSize >= 4) && (puSize <= 32));
	return puSize;
}

int Pu::getModeIdx() const
{
	assert((modeIdx >= 0) && (modeIdx <= 34));
	return modeIdx;
}

int Pu::getPuX() const
{
	return puX;
}

int Pu::getPuY() const
{
	return puY;
}

int** Pu::getPred(ImgComp comp)
{
	return blocks[comp]->getPred();
}

////////////////////////////////////////////

Pb::Pb(const Pu* newPu, ImgComp newComp)
{
	setPu(newPu);
	comp = newComp;
}

Pb::~Pb()
{
}

void Pb::setPu(const Pu* newPu)
{
	assert(newPu != nullptr);
	pu = newPu;
}

void Pb::setPicRecon(int const* const* matrix)
{
	assert(matrix != nullptr);
	picRecon = matrix;
}

ImgComp Pb::getImgComp()  const
{
	return comp;
}

int Pb::getPuIdx() const
{
	return pu->getPuIdx();
}

int Pb::getPuSize() const
{
	return pu->getPuSize()/(comp==LUMA ? 1 : 2);
}

int Pb::getModeIdx() const
{
	return pu->getModeIdx();
}

int Pb::getPuX() const
{
	return pu->getPuX()/(comp==LUMA ? 1 : 2);
}

int Pb::getPuY() const
{
	return pu->getPuY()/(comp==LUMA ? 1 : 2);
}

int Pb::getCorner() const
{
	int puX = getPuX(), puY = getPuY();
	int cornerRef = SeqParams::getInstance()->getDefVal(comp);
	if (calcPuAvail(puX - 1, puY - 1))
		cornerRef = getReconRef(CORNER_DIR);
	else if (calcPuAvail(puX - 1, puY))
		cornerRef = getReconRef(LEFT_DIR);
	else if (calcPuAvail(puX, puY - 1))
		cornerRef = getReconRef(TOP_DIR);
	return cornerRef;
}

int* Pb::getSideRefs(const Direction dir) const
{
	assert(dir != CORNER_DIR);

	int puSize = pu->getPuSize();

	int *refs = new int [2 * puSize];
	int puX = getPuX(), puY = getPuY();

	int nghPuX = dir == LEFT_DIR ? puX - 1 : puX;
	int nghPuY = dir == TOP_DIR ? puY - 1 : puY;
	bool neighborAvail = calcPuAvail(nghPuX, nghPuY);

	int refSubs = getRefSubs(dir);
	for (int x = 0; x < puSize; x++)
		refs[x] = neighborAvail ? getReconRef(dir, x) : refSubs;

	nghPuX = dir == LEFT_DIR ? puX - 1 : puX + puSize;
	nghPuY = dir == TOP_DIR ? puY - 1 : puY + puSize;
	neighborAvail = calcPuAvail(nghPuX, nghPuY);

	refSubs = refs[puSize - 1];
	for (int x = puSize; x < 2 * puSize; x++)
		refs[x] = neighborAvail ? getReconRef(dir, x) : refSubs;

	return refs;
}

int** Pb::getPred() const
{
	return IntraPred::getInstance()->calcPred(this);
}

int **Pb::getPredForceRef(int *leftRefs, int *topRefs, int corner) const
{
	return IntraPred::getInstance()->calcPredForceRefs(this,leftRefs,topRefs,corner);
}

int Pb::getReferenceAtPosition(const Direction dir, const int offset) const
{
	int puX = getPuX(), puY = getPuY();
	if (dir == LEFT_DIR && puX > 0)
		return picRecon[puX - 1][puY + offset];
	else if (dir == CORNER_DIR && puX > 0 && puY > 0)
		return picRecon[puX - 1][puY - 1];
	else if (dir == TOP_DIR && puY > 0)
		return picRecon[puX + offset][puY - 1];
	else 
		return SeqParams::getInstance()->getDefVal(comp);
}

bool Pb::calcPuAvail(const int targetPuX, const int targetPuY) const
{
  SeqParams *seqParams = SeqParams::getInstance();

  int picWidth = seqParams->getPicWidth()/(comp == LUMA ? 1 : 2);
  int picHeight = seqParams->getPicHeight()/(comp == LUMA ? 1 : 2);
  /*int maxCuSize = seqParams->getMaxCuSize()/(comp == LUMA ? 1 : 2);*/
  int puX = pu->getPuX()/(comp == LUMA ? 1 : 2);
  int puY = pu->getPuY()/(comp == LUMA ? 1 : 2);

  bool puOutsidePic = (targetPuX < 0) || (targetPuY < 0) || (targetPuX >= picWidth) || (targetPuY >= picHeight);
  /*bool puFromNextCu = ((targetPuX - targetPuY >= maxCuSize) && (targetPuY >= puY)) || (targetPuY - puY >= maxCuSize);
	bool puAlreadyCalc = calcPuIdx(targetPuX % maxCuSize, targetPuY % maxCuSize) < puIdx;*/
  bool puFromPrevCu = (targetPuX < puX) || (targetPuY < puY);
  
	if(puOutsidePic)
		return false;
  if (puFromPrevCu )
    return true;
  else
    return false;
}

int Pb::getReconRef(const Direction dir, const int offset) const
{
  int puX = getPuX(), puY = getPuY();
  if (dir == LEFT_DIR)
    return picRecon[puX - 1][puY + offset];
  else if (dir == CORNER_DIR)
    return picRecon[puX - 1][puY - 1];
  else
    return picRecon[puX + offset][puY - 1];
}

int Pb::getRefSubs(const Direction dir) const
{
  int puX = getPuX(), puY = getPuY();

  Direction oppDir = dir == LEFT_DIR ? TOP_DIR : LEFT_DIR;
  int oppPuX = oppDir == LEFT_DIR ? puX - 1 : puX;
  int oppPuY = oppDir == TOP_DIR ? puY - 1 : puY;

  int refSubs = SeqParams::getInstance()->getDefVal(comp);
  if (calcPuAvail(puX - 1, puY - 1))
    refSubs = getReconRef(CORNER_DIR);
  else if (calcPuAvail(oppPuX, oppPuY))
    refSubs = getReconRef(oppDir);
  return refSubs;
}