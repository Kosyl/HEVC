#ifndef INTRA_PRED_H
#define INTRA_PRED_H

#include <cassert>
#include <algorithm>

#include "Utils.h"
#include "Pu.h"
#include "IntraMode.h"

class IntraPred
{
  private:
    static IntraPred *instance;
    IntraMode **modes;
    const Pb *pb;
    int corner;
    int *leftRefs, *topRefs;
    IntraPred();
    int getFiltThresh() const;
    bool isFiltReq() const;
    int filtRef(const int, const int, const int) const;
    void filterSideRefs(const Direction);
    void filter();
    bool checkSmoothCond(const Direction) const;
    bool isSmoothReq() const;
    int smothRef(const Direction, const int) const;
    void smoothSideRefs(const Direction);
    void smooth();
    IntraMode *getStrategy();
  public:
    ~IntraPred();
    static IntraPred *getInstance();
    int **calcPred(const Pb *);
	int **calcPredForceRefs(const Pb *, int *leftRefs, int *topRefs, const int corner);
};

#endif