#include "IntraPred.h"

IntraPred* IntraPred::instance = nullptr;

IntraPred::IntraPred()
{
  modes = new IntraMode *[4];
  modes[PLANAR] = new PlanarMode();
  modes[DC] = new DcMode();
  modes[LINEAR] = new LinearMode();
  modes[ANGULAR] = new AngMode();
}

IntraPred::~IntraPred()
{
  for (int i = 0; i < 4; i++)
    delete modes[i];
  delete [] modes;
}

IntraPred* IntraPred::getInstance()
{
  if (instance == nullptr)
    instance = new IntraPred();
  return instance;
}

int IntraPred::getFiltThresh() const
{
  int thresh = 10;
  switch (pb->getPuSize())
  {
    case 8:
      thresh = 7;
      break;
    case 16:
      thresh = 1;
      break;
    case 32:
      thresh = 0;
      break;
  };
  return thresh;
}

bool IntraPred::isFiltReq() const
{
  bool skipFiltration = (pb->getImgComp() != LUMA) || (pb->getModeIdx() == 1);
  int dist = std::min(abs(pb->getModeIdx() - 10), abs(pb->getModeIdx() - 26));
  bool distTooSmall = dist <= getFiltThresh();
  if (skipFiltration | distTooSmall)
    return false;
  return true;
}

int IntraPred::filtRef(const int mainRef, const int leftRef, const int rightRef) const
{
  return (leftRef + 2 * mainRef + rightRef + 2) >> 2;
}

void IntraPred::filterSideRefs(const Direction dir)
{
  int *refs = dir == LEFT_DIR ? leftRefs : topRefs;

  int prevRef = corner, currRef;
  for (int x = 0; x < 2 * pb->getPuSize() - 1; x++, prevRef = currRef)
  {
    currRef = refs[x];
    refs[x] = filtRef(refs[x], prevRef, refs[x + 1]);
  }
}

void IntraPred::filter()
{
  assert((leftRefs != nullptr) && (topRefs != nullptr));

  int firstLeft = leftRefs[0];
  int firstTop = topRefs[0];

  filterSideRefs(LEFT_DIR);
  filterSideRefs(TOP_DIR);

  corner = filtRef(corner, firstLeft, firstTop);
}

bool IntraPred::checkSmoothCond(const Direction dir) const
{
  assert(dir != CORNER_DIR);
  const int *currRefs = dir == LEFT_DIR ? leftRefs : topRefs;
  int cond = abs(corner + currRefs[2 * pb->getPuSize() - 1] - 2 * currRefs[pb->getPuSize() - 1]);
  int limit = 1 << (SeqParams::getInstance()->getBitDepthLuma() - 5);
  return cond < limit;
} 

bool IntraPred::isSmoothReq() const
{
  bool skipFiltration = (pb->getImgComp() != LUMA) || (pb->getModeIdx() == 1);
  bool skipSmoothing = (pb->getPuSize() != 32) || !SeqParams::getInstance()->getSmoothEn();
  bool smoothCond = checkSmoothCond(LEFT_DIR) && checkSmoothCond(TOP_DIR);
  return !(skipFiltration || skipSmoothing) && smoothCond;
}

int IntraPred::smothRef(const Direction dir, const int offset) const
{
  int lastRef = dir == LEFT_DIR ? leftRefs[2 * pb->getPuSize() - 1] : topRefs[2 * pb->getPuSize() - 1];
  return ((63 - offset) * corner + (offset + 1) * lastRef + 32) >> 6;
}

void IntraPred::smoothSideRefs(const Direction dir)
{
  int *refs = dir == LEFT_DIR ? leftRefs : topRefs;
  for (int x = 0; x < 2 * pb->getPuSize(); x++)
    refs[x] = smothRef(dir, x);
}

void IntraPred::smooth()
{
  smoothSideRefs(LEFT_DIR);
  smoothSideRefs(TOP_DIR);
}

IntraMode *IntraPred::getStrategy()
{
  switch (pb->getModeIdx())
  {
    case 0:
      return modes[PLANAR];
    case 1:
      return modes[DC];
    case 10:
    case 26:
      return modes[LINEAR];
    default:
      return modes[ANGULAR];
  }
}

int **IntraPred::calcPred(const Pb *newPb)
{
  assert(newPb != nullptr);

  pb = newPb;

  corner = pb->getCorner();
  leftRefs = pb->getSideRefs(LEFT_DIR);
  topRefs = pb->getSideRefs(TOP_DIR);

  if (isFiltReq())
  {
    if (isSmoothReq())
      smooth();
    else 
      filter();
  }

  IntraMode *strategy = getStrategy();
  strategy->setPb(pb);
  strategy->setCorner(corner);
  strategy->setSideRefs(LEFT_DIR, leftRefs);
  strategy->setSideRefs(TOP_DIR, topRefs);

  int **pred = strategy->calcPred();

  delete [] leftRefs;
  delete [] topRefs;

  return pred;
}

int **IntraPred::calcPredForceRefs(const Pb *newPb, int *leftRefs, int *topRefs, const int corner)
{
  assert(newPb != nullptr);

  pb = newPb;

  this->corner = corner;
  this->leftRefs = leftRefs;
  this->topRefs = topRefs;

  if (isFiltReq())
  {
    if (isSmoothReq())
      smooth();
    else 
      filter();
  }

  IntraMode *strategy = getStrategy();
  strategy->setPb(pb);
  strategy->setCorner(corner);
  strategy->setSideRefs(LEFT_DIR, leftRefs);
  strategy->setSideRefs(TOP_DIR, topRefs);

  int **pred = strategy->calcPred();

  return pred;
}