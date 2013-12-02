#include "Pu.h"
#include "IntraMode.h"

IntraMode::IntraMode()
{
  pb = nullptr;
  leftRefs = topRefs = nullptr;
}

IntraMode::~IntraMode()
{
}

int **IntraMode::initPred() const
{
	return getEmptyMatrix<int>(pb->getPuSize(),pb->getPuSize());
}

void IntraMode::setPb(const Pb *newPb)
{
  pb = newPb;
}

void IntraMode::setCorner(const int newCorner)
{
  corner = newCorner;
}

void IntraMode::setSideRefs(const Direction dir, const int *refs)
{
  assert(dir != CORNER_DIR);

  if (dir == LEFT_DIR)
    leftRefs = refs;
  else
    topRefs = refs;
}

PlanarMode::PlanarMode()
{
}

PlanarMode::~PlanarMode()
{
}

int **PlanarMode::calcPred()
{
  assert(pb != nullptr);
  assert((leftRefs != nullptr) || (topRefs != nullptr));

  int log2PuSize = log2(pb->getPuSize());

  int **pred = initPred();
  for (int x = 0; x < pb->getPuSize(); x++)
  {
    for (int y = 0; y < pb->getPuSize(); y++)
    {
      int offset = (leftRefs[y] + topRefs[x] + 1) << log2PuSize;
      int sumLeft = (x + 1) * (topRefs[pb->getPuSize()] - leftRefs[y]);
      int sumTop = (y + 1) * (leftRefs[pb->getPuSize()] - topRefs[x]);
      pred[x][y] = (offset + sumLeft + sumTop) >> (log2PuSize + 1);
    }
  }
  return pred;
}

DcMode::DcMode()
{
}

DcMode::~DcMode()
{
}

void DcMode::calcDcVal()
{
  dcVal = pb->getPuSize();
  for (int x = 0; x < pb->getPuSize(); x++)
    dcVal += leftRefs[x] + topRefs[x];
  dcVal >>= log2(pb->getPuSize()) + 1;
}

int DcMode::getFiltCorner() const
{
  bool filtCorner = (pb->getImgComp() == LUMA) && (pb->getPuSize() < 32);
  if (filtCorner)
    return (leftRefs[0] + 2 * dcVal + topRefs[0] + 2) >> 2;
  else
    return dcVal;
}

int DcMode::getFiltEdge(const Direction dir, const int offset) const
{
  assert(dir != CORNER_DIR);
  assert((offset >= 0) && (offset < pb->getPuSize()));

  const int *refs = dir == LEFT_DIR ? leftRefs : topRefs;
  bool filtRef = (pb->getImgComp() == LUMA) && (pb->getPuSize() < 32);

  if (filtRef)
    return (refs[offset] + 3 * dcVal + 2) >> 2;
  else
    return dcVal;
}

int **DcMode::calcPred()
{
  assert(pb != nullptr);
  assert((leftRefs != nullptr) || (topRefs != nullptr));

  calcDcVal();

  int **pred = initPred();
  pred[0][0] = getFiltCorner();
  for (int x = 1; x < pb->getPuSize(); x++)
  {
    pred[0][x] = getFiltEdge(LEFT_DIR, x);
    pred[x][0] = getFiltEdge(TOP_DIR, x);
  }

  for (int x = 1; x < pb->getPuSize(); x++)
    for (int y = 1; y < pb->getPuSize(); y++)
      pred[x][y] = dcVal;

  return pred;
}

LinearMode::LinearMode()
{
}

LinearMode::~LinearMode()
{
}

int LinearMode::getFiltEdge(const Direction dir, const int offset)
{
  const int *mainRefs = dir == LEFT_DIR ? leftRefs : topRefs;
  const int *sideRefs = dir == LEFT_DIR ? topRefs : leftRefs;
  int ref = mainRefs[0] + ((sideRefs[offset] - corner) >> 1);
  return SeqParams::getInstance()->clip(pb->getImgComp(), ref);
}

int **LinearMode::calcPred()
{
  assert(pb != nullptr);
  assert((leftRefs != nullptr) || (topRefs != nullptr));

  Direction dir = pb->getModeIdx() == 10 ? LEFT_DIR : TOP_DIR;
  bool filtEdge = (pb->getImgComp() == LUMA) && (pb->getPuSize() < 32);

  int **pred = initPred();

  pred[0][0] = dir == LEFT_DIR ? leftRefs[0] : topRefs[0];
  if (filtEdge)
    pred[0][0] = getFiltEdge(dir, 0);

  for (int x = 1; x < pb->getPuSize(); x++)
    pred[x][0] = dir == TOP_DIR ? topRefs[x] : filtEdge ? getFiltEdge(LEFT_DIR, x) : leftRefs[0];

  for (int y = 1; y < pb->getPuSize(); y++)
    pred[0][y] = dir == LEFT_DIR ? leftRefs[y] : filtEdge ? getFiltEdge(TOP_DIR, y) : topRefs[0];

  for (int x = 1; x < pb->getPuSize(); x++)
    for (int y = 1; y < pb->getPuSize(); y++)
      pred[x][y] = dir == LEFT_DIR ? leftRefs[y] : topRefs[x];

  return pred;
}

const int AngMode::angles[] = {32, 26, 21, 17, 13, 9, 5, 2, 0, -2, -5, -9, -13, -17, -21, -26, -32, -26, -21, -17, -13, -9, -5,
                              -2, 0, 2, 5, 9, 13, 17, 21, 26, 32};

const int AngMode::invAngles[] = {4096, 1638, 910, 630, 482, 390, 315, 256, 315, 390, 482, 630, 910, 1638, 4096};

AngMode::AngMode()
{
  refsArray = nullptr;
}

AngMode::~AngMode()
{
  if (refsArray != nullptr)
    delete [] refsArray;
}

int AngMode::getAngle() const
{
  assert((pb->getModeIdx() != 0) && (pb->getModeIdx() != 1) && (pb->getModeIdx() != 10) && (pb->getModeIdx() != 26));
  return AngMode::angles[pb->getModeIdx() - 2];
}

int AngMode::getInvAngle() const
{
  assert((pb->getModeIdx() > 10) || (pb->getModeIdx() < 26));
  return AngMode::invAngles[pb->getModeIdx() - 11];
}

void AngMode::getRefsArray()
{
  assert(pb != nullptr);
  assert((leftRefs != nullptr) || (topRefs != nullptr));

  int angle = getAngle();

  bool modeHor = pb->getModeIdx() < 18;

  if (refsArray != nullptr)
    delete [] refsArray;
  refsArray = new int [2 * pb->getPuSize() + 1];

  int start = angle > 0 ? 0 : pb->getPuSize();
  refsArray[start++] = corner;
  for (int x = start; x < 2 * pb->getPuSize() + 1; x++)
    refsArray[x] = modeHor ? leftRefs[x - start] : topRefs[x - start];

  if (angle < 0)
  {
    int invAngle = getInvAngle();
    int invAngleSum = 128;
    int limit = (angle * pb->getPuSize()) >> 5;
    for (int x = -1; x > limit; x--)
    {
      invAngleSum += invAngle;
      int refIdx = (invAngleSum >> 8) - 1;
      refsArray[pb->getPuSize() + x] = modeHor ? topRefs[refIdx] : leftRefs[refIdx];
    }
  }
}

int **AngMode::calcPred()
{
  assert(pb != nullptr);
  assert((leftRefs != nullptr) || (topRefs != nullptr));

  getRefsArray();

  int angle = getAngle();
  int *refs = angle >= 0 ? refsArray : refsArray + pb->getPuSize();

  bool modeHor = pb->getModeIdx() < 18;

  int **pred = initPred();
  int angleSum = 0, iFact, iIdx, refIdx;
  for (int x = 0; x < pb->getPuSize(); x++)
  {
    angleSum += angle;
    iFact = angleSum & 31;
    iIdx = angleSum >> 5;
    for (int y = 0; y < pb->getPuSize(); y++)
    {
      refIdx = iIdx + y + 1;
      int comp0 = iFact != 32 ? (32 - iFact) * refs[refIdx] : 0;
      int comp1 = iFact != 0 ? iFact * refs[refIdx + 1] : 0;
      pred[x][y] = (comp0 + comp1 + 16) >> 5;
    }
  }

  if (!modeHor)
    for (int y = 0; y < pb->getPuSize(); y++)
      for (int x = y; x < pb->getPuSize(); x++)
      {
        int swap = pred[x][y];
        pred[x][y] = pred[y][x];
        pred[y][x] = swap;
      }

  delete [] refsArray;
  refsArray = nullptr;

  return pred;
}
