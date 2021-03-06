#include "PU.h"
#include "IntraPred.h"
#include "QuadTree.h"
#include "TU.h"

PUIntra::PUIntra( CUIntra *newCU, UInt X, UInt Y, UInt size ) :
CTUComponentBase( newCU != nullptr ? newCU->getPicture( ) : nullptr, newCU != nullptr ? newCU->getParentCTU( ) : nullptr, X, Y, size ),
itsParentCU( newCU )
{
}

PUIntra::~PUIntra( )
{
}

Void PUIntra::printDescription( )
{
	LOG( "PART" ) << "PUIntra[" << getX( ) << "][" << getY( ) << "], size: " << getSize( ) << ", lumaPredMode: " << itsLumaModeIdx << std::endl;

	LOG( "PART" ) << "zawarte TU:" << std::endl;
	LOG_TAB( "PART" );
	itsTransformArea->printDescription( );
	LOG_UNTAB( "PART" );
}

UInt PUIntra::getModeForChroma( UInt modeForLuma, UInt chromaPredictionDerivationMode )
{
	switch( chromaPredictionDerivationMode )
	{
	case 0:
		return modeForLuma == 0 ? 34 : 0;
	case 1:
		return modeForLuma == 26 ? 34 : 26;
	case 2:
		return modeForLuma == 10 ? 34 : 10;
	case 3:
		return modeForLuma == 1 ? 34 : 1;
	case 4:
		return modeForLuma;
	default:
		assert( false );
		return 0;
	}
}

Void PUIntra::reconstructionLoop( )
{
	LOG( "RECO" ) << "PUIntra[" << getX( ) << "][" << getY( ) << "], size: " << getSize( ) << ", lumaPredMode: " << itsLumaModeIdx << std::endl;
	LOG_TAB("RECO" );
	itsTransformArea->reconstructionLoop( this );
	LOG_UNTAB("RECO" );
}
////////////////////////////////////////////

PBIntra::PBIntra( TB* parentTB, ImgComp comp, UInt x, UInt y, UInt size ) :
UnitBase( parentTB != nullptr ? parentTB->getPicture( ) : nullptr, x, y, size ),
itsComp( comp ),
itsParentTB( parentTB ),
itsCornerReference( 0 ),
itsReferencesReady( false ),
itsPredictionTarget( nullptr )
{
	assert( parentTB != nullptr );
	itsPicRecon = itsParentPicture->reconMatrix( itsComp );
	assert( itsPicRecon != nullptr );
	itsSideReferences = getEmptyMatrix<Sample>( 2, 2 * itsSize );
	//itsPredictionTarget = itsParentTB->getCu( )->getPredictionMatrix( itsComp );
}


PBIntra::~PBIntra( )
{
}

ImgComp PBIntra::getImgComp( )  const
{
	return itsComp;
}

UInt PBIntra::getPUIdx( ) const
{
	return itsParentTB->getIdx( );
}

UInt PBIntra::getModeIdx( ) const
{
	return itsModeIdx;
}

Void PBIntra::setModeIdx( UInt mode )
{
	itsModeIdx = mode;
}

Sample PBIntra::getCornerReference( )
{
	if( !itsReferencesReady )
		calcReferences( );

	return itsCornerReference;
}

Sample* PBIntra::getSideRefs( const IntraDirection dir )
{
	assert( dir != INTRA_DIR_CORNER );

	if( !itsReferencesReady )
		calcReferences( );

	return itsSideReferences[ dir ];
}

Sample** PBIntra::getPred( )
{
	return IntraPred::getInstance( )->calcPred( this );
}

Sample **PBIntra::getPredForceRef( Sample* leftRefs, Sample* topRefs, Sample corner )
{
	return IntraPred::getInstance( )->calcPredForceRefs( this, leftRefs, topRefs, corner );
}

Bool PBIntra::calcPuAvail( const Int targetPuX, const Int targetPuY ) const
{
	UInt picWidth = SeqParams( )->getPicWidth( ) / ( itsComp == LUMA ? 1 : 2 );
	UInt picHeight = SeqParams( )->getPicHeight( ) / ( itsComp == LUMA ? 1 : 2 );

	bool puOutsidePic = ( targetPuX < 0 ) || ( targetPuY < 0 ) || ( targetPuX >= picWidth ) || ( targetPuY >= picHeight );
	bool puAlreadyCalc = getZScanIdxIn4x4( targetPuX, targetPuY ) < itsIdx;

	if( puOutsidePic || !puAlreadyCalc )
		return false;

	return true;
}

UShort PBIntra::getReferenceValue( const IntraDirection dir, const UInt offset ) const
{
	if( dir == INTRA_DIR_LEFT )
		return itsPicRecon[ itsX - 1 ][ itsY + offset ];
	else if( dir == INTRA_DIR_CORNER )
		return itsPicRecon[ itsX - 1 ][ itsY - 1 ];
	else
		return itsPicRecon[ itsX + offset ][ itsY - 1 ];
}

Void PBIntra::calcReferences( )
{

	Bool** sideAvailable = getEmptyMatrix<Bool>( 2, 2 * itsSize );
	Bool cornerAvailable = false;
	Bool atLeasOneAvailable = false;

	Int X = itsX;
	Int Y = itsY;
	//corner
	cornerAvailable = calcPuAvail( X - 1, Y - 1 );
	if( cornerAvailable )
	{
		itsCornerReference = itsPicRecon[ X - 1 ][ Y - 1 ];
		atLeasOneAvailable = true;
	}

	UInt offsetLimit = 2 * itsSize;

	for( UInt offset = 0; offset < offsetLimit; ++offset )
	{
		//left
		sideAvailable[ INTRA_DIR_LEFT ][ offset ] = calcPuAvail( X - 1, Y + offset );
		if( sideAvailable[ INTRA_DIR_LEFT ][ offset ] )
		{
			itsSideReferences[ INTRA_DIR_LEFT ][ offset ] = getReferenceValue( INTRA_DIR_LEFT, offset );
			atLeasOneAvailable = true;
		}

		//up
		sideAvailable[ INTRA_DIR_TOP ][ offset ] = calcPuAvail( X + offset, Y - 1 );
		if( sideAvailable[ INTRA_DIR_TOP ][ offset ] )
		{
			itsSideReferences[ INTRA_DIR_TOP ][ offset ] = getReferenceValue( INTRA_DIR_TOP, offset );
			atLeasOneAvailable = true;
		}
	}

	if( !atLeasOneAvailable )
	{
		Sample def = SeqParams( )->getDefaultSampleValue( itsComp );
		itsCornerReference = def;
		for( UInt offset = 0; offset < offsetLimit; ++offset )
		{
			itsSideReferences[ INTRA_DIR_LEFT ][ offset ] = def;
			itsSideReferences[ INTRA_DIR_TOP ][ offset ] = def;
		}
	}
	else
	{
		fillMissingReferences( sideAvailable, cornerAvailable );
	}

	delete[] sideAvailable[ INTRA_DIR_LEFT ];
	delete[] sideAvailable[ INTRA_DIR_TOP ];
	delete[] sideAvailable;

	LOG( "RECO" ) << "Probki referencyjne:" << std::endl << "Corner: " << itsCornerReference << std::endl;
	printMatrix( itsSideReferences, 2, 2*itsSize, LOG( "RECO" ) );
	itsReferencesReady = true;
}

Void PBIntra::fillMissingReferences( Bool** sideAvailable, Bool cornerAvailable )
{
	if( sideAvailable[ INTRA_DIR_LEFT ][ 2 * itsSize - 1 ] == false )
	{
		for( Int idx = 2 * itsSize - 2; idx >= 0; --idx )
		{
			//scan left
			if( sideAvailable[ INTRA_DIR_LEFT ][ idx ] )
			{
				itsSideReferences[ INTRA_DIR_LEFT ][ 2 * itsSize - 1 ] = itsSideReferences[ INTRA_DIR_LEFT ][ idx ];
				sideAvailable[ INTRA_DIR_LEFT ][ 2 * itsSize - 1 ] = true;
				break;
			}
		}

		if( sideAvailable[ INTRA_DIR_LEFT ][ 2 * itsSize - 1 ] == false && cornerAvailable )
		{
			itsSideReferences[ INTRA_DIR_LEFT ][ 2 * itsSize - 1 ] = itsCornerReference;
			sideAvailable[ INTRA_DIR_LEFT ][ 2 * itsSize - 1 ] = true;
		}

		if( sideAvailable[ INTRA_DIR_LEFT ][ 2 * itsSize - 1 ] == false )
		{
			UInt idxLimit = 2 * itsSize;
			for( UInt idx = 0; idx < idxLimit; ++idx )
			{
				//scan top
				if( sideAvailable[ INTRA_DIR_TOP ][ idx ] )
				{
					itsSideReferences[ INTRA_DIR_LEFT ][ 2 * itsSize - 1 ] = itsSideReferences[ INTRA_DIR_TOP ][ idx ];
					sideAvailable[ INTRA_DIR_TOP ][ 2 * itsSize - 1 ] = true;
					break;
				}
			}
		}
	}
	for( Int idx = 2 * itsSize - 2; idx >= 0; --idx )
	{
		if( !sideAvailable[ INTRA_DIR_LEFT ][ idx ] )
		{
			itsSideReferences[ INTRA_DIR_LEFT ][ idx ] = itsSideReferences[ INTRA_DIR_LEFT ][ idx + 1 ];
		}
	}
	if( !cornerAvailable )
		itsCornerReference = itsSideReferences[ INTRA_DIR_LEFT ][ 0 ];
	if( !sideAvailable[ INTRA_DIR_TOP ][ 0 ] )
		itsSideReferences[ INTRA_DIR_TOP ][ 0 ] = itsCornerReference;
	UInt idxLimit = 2 * itsSize;
	for( UInt idx = 1; idx < idxLimit; ++idx )
	{
		if( !sideAvailable[ INTRA_DIR_TOP ][ idx ] )
		{
			itsSideReferences[ INTRA_DIR_TOP ][ idx ] = itsSideReferences[ INTRA_DIR_TOP ][ idx - 1 ];
		}
	}
}

Void PBIntra::calcAndWritePredictionToCU( PUIntra* mainPU )
{
	itsPredictionTarget = mainPU->getCu( )->getPredictionMatrix( itsComp );
	itsModeIdx = mainPU->getModeIdx( itsComp );

	Sample** pred = this->getPred( );
	UInt shift = (itsComp == LUMA ? 0 : 1);
	UInt offsetX = ( mainPU->getX( ) - mainPU->getCu( )->getX( ) ) >> shift;
	UInt offsetY = ( mainPU->getY( ) - mainPU->getCu( )->getY( ) ) >> shift;
	for( UInt x = offsetX; x < offsetX + itsSize; ++x )
	{
		for( UInt y = offsetY; y < offsetY + itsSize; ++y )
		{
			itsPredictionTarget[ x ][ y ] = pred[ x - offsetX ][ y - offsetY ];
		}
	}
	deleteMatrix( pred, itsSize );
}

Void PBIntra::printDescription( )
{
	LOG( "PART" ) << "PBIntra[" << getX( ) << "][" << getY( ) << "], size: " << getSize( ) << ", comp: " << itsComp << std::endl;
}