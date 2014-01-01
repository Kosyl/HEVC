#ifndef TU_h__
#define TU_h__

#include "Utils.h"
#include "Binarization.h"
#include "UnitBase.h"
#include <boost/dynamic_bitset.hpp>

class PUIntra;

class SubBlock
{
	boost::dynamic_bitset<> itsSignificantCoeffFlags;
	boost::dynamic_bitset<> itsC1Group;
	Bool itsC2;
	boost::dynamic_bitset<> itsCoeffSigns;
};

class TU;
class CU;
class PBIntra;

class TB : public UnitBase
{
	ImgComp itsComp;
	TU* itsParentTU;
	Sample** itsPictureSamples;
	Sample** itsPrediction;
	Sample** itsResiduals;
	Sample** itsReconstructionTarget;
	Coeff** itsCoefficients;
	std::shared_ptr<PBIntra> itsHelperPB;
	
	ScanningMode* itsScanStrategy;
	UInt itsLastSignificantCoeffX, itsLastSignificantCoeffY;
	boost::dynamic_bitset<> itsCodedSubBlockFlags;

public:
	TB( TU* parentTU, UShort x, UShort y, UShort size, ImgComp comp = LUMA );
	~TB( );

	Void transformAndReconstruct( PUIntra* mainPU );
	virtual Void printDescription( );

	std::shared_ptr<PBIntra> getHelperPB( ) const
	{
		return itsHelperPB;
	}
	Void setHelperPB( std::shared_ptr<PBIntra> val )
	{
		itsHelperPB = val;
	}
};

class TU : public CTUComponentBase
{
private:
	Int itsQPDeltaFromCU;
	UInt itsQP;
	UInt transformDepth;
	Bool itsCBFs[ 3 ];
	std::vector<std::shared_ptr<TB> > itsTBs;
	CU* itsParentCU;

	std::shared_ptr<PUIntra> itsHelperPU;

public:
	TU( CU* parentCU, UInt x, UInt y, UInt size );
	~TU( )
	{
	}
	
	virtual Void printDescription( );

	Void reconstructionLoop( PUIntra* mainPU )
	{
		for( UInt i = 0; i < itsTBs.size( ); ++i )
			itsTBs[ i ]->transformAndReconstruct( mainPU );
	}

	CU* getParentCU( )
	{
		return itsParentCU;
	}
};
#endif // TU_h__