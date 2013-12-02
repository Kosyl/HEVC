#ifndef INTRA_PRED_H
#define INTRA_PRED_H

#include <cassert>
#include <algorithm>

#include "SeqParams.h"
#include "IntraPu.h"
#include "IntraMode.h"

class IntraPred
{
  private:
    enum ModeType {PLANAR = 0, DC, LINEAR, ANGULAR};
    static IntraPred *instance;
    IntraMode **modes;
    const IntraPu *pu;
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
    int **calcPred(const IntraPu *);
	int **calcPredForceRefs(const IntraPu *, const int *leftRefs, const int *topRefs, const int corner);
};

#endif