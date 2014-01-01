#ifndef PB_H
#define PB_H

#include <cassert>
#include "Utils.h"
#include "CU.h"
#include "UnitBase.h"

class PUIntra;
class TB;

class PBIntra : public UnitBase
{
private:
	TB *itsParentTB;
	ImgComp itsComp;
	Sample const* const *itsPicRecon;
	Sample** itsPredictionTarget;
	Sample** itsSideReferences;
	Sample itsCornerReference;
	Bool itsReferencesReady;
	UInt itsModeIdx;

	Bool calcPuAvail( const Int x, const Int y) const;
	UShort getReferenceValue( const IntraDirection dir, const UInt offset = 0 ) const;
	Void fillMissingReferences( Bool** sideAvailable, Bool cornerAvailable );

public:
	PBIntra( TB* parentTB, ImgComp comp, UInt x, UInt y, UInt size );
	~PBIntra( );

	Void calcAndWritePredictionToCU( PUIntra* mainPU );
	Void calcReferences( );
	ImgComp getImgComp( ) const;
	UInt getModeIdx( ) const;
	UInt getPUIdx( ) const;
	Void setModeIdx( UInt mode );
	Sample getCornerReference( );
	Sample* getSideRefs( const IntraDirection );
	Sample **getPred( );
	Sample **getPredForceRef( Sample* leftRefs, Sample* topRefs, Sample corner );

	virtual Void printDescription( );
};

class PUIntra : public CTUComponentBase
{
private:
	CUIntra* itsParentCU;
	UInt itsLumaModeIdx, itsChromaModeIdx;

	std::shared_ptr<TUQuadTree> itsTransformArea;

public:
	PUIntra( CUIntra* newCU, UInt X, UInt Y, UInt size );
	~PUIntra( );
	Void setLumaModeIdx( const UInt modeIdx )
	{
		assert( modeIdx >= 0 && modeIdx <= 34 );
		itsLumaModeIdx = modeIdx;
		itsChromaModeIdx = PUIntra::getModeForChroma( itsLumaModeIdx, itsParentCU->getIntraPredictionDerivationType( ) );
	}

	virtual CUIntra* getCu( ) const
	{
		return itsParentCU;
	}
	UInt getModeIdx( ImgComp comp ) const
	{
		if( comp == LUMA )
			return itsLumaModeIdx;
		else
			return itsChromaModeIdx;
	}

	std::shared_ptr<TUQuadTree> getTransformArea( ) const
	{
		return itsTransformArea;
	}
	Void setTransformArea( std::shared_ptr<TUQuadTree> val )
	{
		itsTransformArea = val;
	}

	Void reconstructionLoop( );

	static UInt getModeForChroma( UInt modeForLuma, UInt chromaPredictionDerivationMode );
	virtual Void printDescription( );
};

#endif