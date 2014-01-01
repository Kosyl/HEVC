#ifndef CU_H
#define CU_H

#include <cassert>
#include <vector>
#include "Utils.h"
#include "Picture.h"
#include "UnitBase.h"

class PUIntra;
class TUQuadTree;

class CU : public CTUComponentBase
{
protected:
	Sample*** itsPredictions;
	Short itsQPDeltaForCU;
	Bool itsTransQuantBypassEnabled; // skalowanie, transformata i filtr deblok.

	PartitionMode itsPartitionMode;
	std::shared_ptr<TUQuadTree> itsTransformTree;
	
public:
	CU( CTU* ctu, UShort x, UShort y, UShort size );
	virtual ~CU( );

#pragma region Akcesory

	std::shared_ptr<TUQuadTree> getTransformTree( ) const
	{
		return itsTransformTree;
	}
	Void setTransformTree( std::shared_ptr<TUQuadTree> val )
	{
		itsTransformTree = val;
	}
	PartitionMode getPartitionMode( ) const
	{
		return itsPartitionMode;
	}
	virtual Void setPartitionMode( PartitionMode val )
	{
		itsPartitionMode = val;
	}
	Sample** getPredictionMatrix( ImgComp comp )
	{
		return itsPredictions[ comp ];
	}

#pragma endregion

	virtual Void printDescription( );
	virtual Void reconstructionLoop( ) = 0;
};

class CUIntra : public CU
{
private:

	UShort itsIntraMPMs[ 3 ];
	std::shared_ptr<PUIntra> itsPUs[ 4 ];
	
	UShort itsChromaPredictionDerivationType;
	
public:
	CUIntra( CTU* ctu, UShort x, UShort y, UShort size );
	virtual ~CUIntra( );

#pragma region Akcesory

	UShort getIntraPredictionDerivationType( ) const
	{
		return itsChromaPredictionDerivationType;
	}
	void setIntraPredictionDerivationType( UShort val )
	{
		itsChromaPredictionDerivationType = val;
	}
	virtual Void setPartitionMode( PartitionMode val )
	{
		assert( val == PART_2Nx2N || val == PART_NxN );
		itsPartitionMode = val;
		for( UInt i = QTCOMPONENT_FIRST; i <= QTCOMPONENT_LAST; ++i )
		{
			if( itsPUs[ i ] != nullptr )
				itsPUs[ i ].reset( );
		}
	}
	Void addPU( std::shared_ptr<PUIntra> newPU, QTComponent position = QTCOMPONENT_FIRST )
	{
		itsPUs[ position ] = newPU;
	}

	std::shared_ptr<PUIntra> getPU( QTComponent position = QTCOMPONENT_FIRST )
	{
		return itsPUs[ position ];
	}

#pragma endregion

	Void createPUs( UInt lumaModeIdx );
	virtual Void printDescription( );
	virtual Void reconstructionLoop( );
};

#endif