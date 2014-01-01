#include "TU.h"
#include "CU.h"
#include "Transform.h"
#include "Quant.h"
#include "PU.h"

TU::TU( CU* parentCU, UInt x, UInt y, UInt size ) :
CTUComponentBase( parentCU->getPicture( ), parentCU->getParentCTU( ), x, y, size ),
itsParentCU( parentCU ),
itsTBs( )
{
	itsTBs.push_back( std::make_shared<TB>( this, x, y, size, LUMA ) );

	if( itsParentCU->getPartitionMode( ) == PART_NxN && itsParentCU->getSize( ) == SeqParams( )->getMinCUSize( ) )
	{
		if( x == itsParentCU->getX( ) && y == itsParentCU->getY( ) )
		{
			itsTBs.push_back( std::make_shared<TB>( this, x / 2, y / 2, size, CB ) );
			itsTBs.push_back( std::make_shared<TB>( this, x / 2, y / 2, size, CR ) );
		}
	}
	else
	{
		itsTBs.push_back( std::make_shared<TB>( this, x / 2, y / 2, size / 2, CB ) );
		itsTBs.push_back( std::make_shared<TB>( this, x / 2, y / 2, size / 2, CR ) );
	}
}

Void TU::printDescription( )
{
	LOG( "PART" ) << "TU[" << getX( ) << "][" << getY( ) << "], size: " << getSize( ) << std::endl;
	LOG_TAB( );
	for( UInt i = 0; i < itsTBs.size( ); ++i )
		itsTBs[ i ]->printDescription( );
	LOG_UNTAB( );
}

//////////////////////////////////////

TB::TB( TU* parentTU, UShort x, UShort y, UShort size, ImgComp comp ) :
UnitBase( parentTU != nullptr ? parentTU->getPicture( ) : nullptr, x, y, size ),
itsParentTU( parentTU ),
itsComp( comp ),
itsScanStrategy( nullptr ),
itsCodedSubBlockFlags( ),
itsLastSignificantCoeffX( 0 ),
itsLastSignificantCoeffY( 0 )
{
	setHelperPB( std::make_shared<PBIntra>( this, comp, x, y, size ) );

	itsPictureSamples = parentTU->getPicture( )->samples( itsComp );
	itsReconstructionTarget = parentTU->getPicture( )->reconMatrix( itsComp );
	itsPrediction = parentTU->getParentCU( )->getPredictionMatrix( itsComp );
	itsResiduals = getEmptyMatrix<Sample>( itsSize, itsSize );
	itsCoefficients = getEmptyMatrix<Coeff>( itsSize, itsSize );
	itsHelperPB = std::make_shared<PBIntra>( this, comp, x, y, size );
}

TB::~TB( )
{
	deleteMatrix( itsResiduals, itsSize );
	deleteMatrix( itsCoefficients, itsSize );
}

Void TB::transformAndReconstruct( PUIntra* mainPU )
{
	LOG( "RECO" ) << "TB[" << getX( ) << "][" << getY( ) << "], size: " << getSize( ) << ", comp: " << itsComp << std::endl;
	LOG( "RECO" ) << "Helper PB:" << std::endl;
	itsHelperPB->printDescription( );

	LOG( "RECO" ) << "Predykcja przed:" << std::endl;
	UInt shift = ( itsComp == LUMA ? 0 : 1 );
	printMatrix( itsPrediction, itsParentTU->getParentCU( )->getSize( ) >> ( shift ), LOG( "RECO" ) );

	LOG_TAB( "RECO" );
	itsHelperPB->calcAndWritePredictionToCU( mainPU );

	LOG( "RECO" ) << "Predykcja po:" << std::endl;
	printMatrix( itsPrediction, itsParentTU->getParentCU( )->getSize( ) >> ( shift ), LOG( "RECO" ) );

	UInt reconOffsetX = ( itsParentTU->getX( ) - itsParentTU->getParentCU( )->getX( ) ) >> ( shift );
	UInt reconOffsetY = ( itsParentTU->getY( ) - itsParentTU->getParentCU( )->getY( ) ) >> ( shift );
	for( UInt x = 0; x < itsSize; ++x )
	{
		for( UInt y = 0; y < itsSize; ++y )
		{
			itsResiduals[ x ][ y ] = itsPictureSamples[ itsX + x ][ itsY + y ] - itsPrediction[ reconOffsetX + x ][ reconOffsetY + y ];
		}
	}

	LOG( "RECO" ) << "Residua:" << std::endl;
	printMatrix( itsResiduals, itsSize, LOG( "RECO" ) );

	Transform* tr = Transform::getInstance( );
	Quant* quant = Quant::getInstance( );

	Coeff** tmpCoeffs = getEmptyMatrix<Coeff>( itsSize, itsSize );
	Sample** reconResiduals = getEmptyMatrix<Sample>( itsSize, itsSize );

	tr->forwardTrans( itsResiduals, tmpCoeffs, itsSize, SeqParams( )->getBitDepth( itsComp ), false, false );
	quant->quant( tmpCoeffs, itsCoefficients, PicParams( )->getQP( itsComp ), SeqParams( )->getBitDepth( itsComp ), itsSize );
	quant->deQuant( itsCoefficients, tmpCoeffs, PicParams( )->getQP( itsComp ), SeqParams( )->getBitDepth( itsComp ), itsSize );
	tr->inverseTrans( tmpCoeffs, reconResiduals, itsSize, SeqParams( )->getBitDepth( itsComp ), false, false );

	LOG( "RECO" ) << "Residua po rekonstrukcji:" << std::endl;
	printMatrix( reconResiduals, itsSize, LOG( "RECO" ) );

	Sample sample = 0;
	for( UInt x = 0; x < itsSize; ++x )
	{
		for( UInt y = 0; y < itsSize; ++y )
		{
			Short sample = itsPictureSamples[ itsX + x ][ itsY + y ] + itsResiduals[ x ][ y ] - reconResiduals[ x ][ y ];
			itsReconstructionTarget[ itsX + x ][ itsY + y ] = SeqParams( )->clip( itsComp, sample );
		}
	}
	LOG( "RECO" ) << "Obraz zrekonstruowany:" << std::endl;
	printMatrix( itsReconstructionTarget, SeqParams( )->getPicWidth( ) >> shift, SeqParams( )->getPicHeight( ) >> shift, LOG( "RECO" ) );
	LOG_UNTAB( "RECO" );
}

Void TB::printDescription( )
{
	LOG( "PART" ) << "TB[" << getX( ) << "][" << getY( ) << "], size: " << getSize( ) << ", comp: " << itsComp << std::endl;
	LOG( "PART" ) << "Helper PB:" << std::endl;
	LOG_TAB( "PART" );
	itsHelperPB->printDescription( );
	LOG_UNTAB( "PART" );
}
