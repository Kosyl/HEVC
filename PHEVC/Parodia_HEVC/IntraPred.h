#ifndef INTRA_PRED_H
#define INTRA_PRED_H

#include <cassert>
#include <algorithm>

#include "Utils.h"
#include "PU.h"
#include "IntraMode.h"

class IntraPred
{
private:
	static IntraPred *itsInstance;
	IntraMode **itsModes;
	PBIntra* itsCurrentPB;
	Sample itsCornerValue;
	Sample** itsReferenceValues;

	IntraPred( );

	UShort getFilteringThreshold( ) const;
	Bool isFilteringRequired( ) const;
	Sample filtRef( const Sample mainRef, const Sample leftRef, const Sample rightRef ) const;
	Void filterSideRefs( const IntraDirection referenceDirection );
	Void doReferenceFiltering( );
	Bool checkSmoothConditions( const IntraDirection direction ) const;
	Bool isSmoothingRequired( ) const;
	Sample getSmoothedReferenceAtPosition( const IntraDirection dir, const UShort offset ) const;
	Void smoothSideRefs( const IntraDirection dir );
	Void doReferenceSmoothing( );
	IntraMode* getPredictionStrategy( );
public:
	~IntraPred( );
	static IntraPred *getInstance( );
	Sample** calcPred( PBIntra* targetPB );
	Sample** calcPredForceRefs( PBIntra* tergetPB, Sample* leftRefs, Sample* topRefs, const Sample corner );
};

#endif