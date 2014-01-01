#include "PU.h"
#include "IntraMode.h"

IntraMode::IntraMode( ) :
itsCurrentPB( nullptr ),
itsReferenceValues( new Sample*[ 2 ] ),
itsCornerReference( 0 )
{
}

IntraMode::~IntraMode( )
{
}

Sample **IntraMode::initPred( ) const
{
	return getEmptyMatrix<Sample>( itsCurrentPB->getSize( ), itsCurrentPB->getSize( ) );
}

Void IntraMode::setPb( PBIntra *newPb )
{
	itsCurrentPB = newPb;
}

Void IntraMode::setCorner( const Sample newCorner )
{
	itsCornerReference = newCorner;
}

Void IntraMode::setSideRefs( const IntraDirection dir, Sample *refs )
{
	assert( dir != INTRA_DIR_CORNER );

	itsReferenceValues[ dir ] = refs;
}

PlanarMode::PlanarMode( ) :
IntraMode( )
{
}

PlanarMode::~PlanarMode( )
{
}

Sample** PlanarMode::calcPred( )
{
	assert( itsCurrentPB != nullptr );
	assert( itsReferenceValues != nullptr && itsReferenceValues[ INTRA_DIR_LEFT ] != nullptr && itsReferenceValues[ INTRA_DIR_TOP ] != nullptr );

	UInt size = itsCurrentPB->getSize( );
	Int log2PuSize = log2Int( size );

	Sample **pred = initPred( );
	for( UInt x = 0; x < size; x++ )
	{
		for( UInt y = 0; y < size; y++ )
		{
			Int offset = ( itsReferenceValues[ INTRA_DIR_LEFT ][ y ] + itsReferenceValues[ INTRA_DIR_TOP ][ x ] + 1 ) << log2PuSize;
			Int sumLeft = ( x + 1 ) * ( itsReferenceValues[ INTRA_DIR_TOP ][ size ] - itsReferenceValues[ INTRA_DIR_LEFT ][ y ] );
			Int sumTop = ( y + 1 ) * ( itsReferenceValues[ INTRA_DIR_LEFT ][ size ] - itsReferenceValues[ INTRA_DIR_TOP ][ x ] );
			pred[ x ][ y ] = ( offset + sumLeft + sumTop ) >> ( log2PuSize + 1 );
		}
	}
	return pred;

	//porownac z:
	//Void TComPrediction::xPredIntraPlanar( Int* pSrc, Int srcStride, Pel* rpDst, Int dstStride, UInt width, UInt height )
	//{
	//	assert( width == height );

	//	Int k, l, bottomLeft, topRight;
	//	Int horPred;
	//	Int leftColumn[ MAX_CU_SIZE + 1 ], topRow[ MAX_CU_SIZE + 1 ], bottomRow[ MAX_CU_SIZE ], rightColumn[ MAX_CU_SIZE ];
	//	UInt blkSize = width;
	//	UInt offset2D = width;
	//	UInt shift1D = g_aucConvertToBit[ width ] + 2;
	//	UInt shift2D = shift1D + 1;

	//	// Get left and above reference column and row
	//	for( k = 0; k < blkSize + 1; k++ )
	//	{
	//		topRow[ k ] = pSrc[ k - srcStride ];
	//		leftColumn[ k ] = pSrc[ k*srcStride - 1 ];
	//	}

	//	// Prepare intermediate variables used in interpolation
	//	bottomLeft = leftColumn[ blkSize ];
	//	topRight = topRow[ blkSize ];
	//	for( k = 0; k < blkSize; k++ )
	//	{
	//		bottomRow[ k ] = bottomLeft - topRow[ k ];
	//		rightColumn[ k ] = topRight - leftColumn[ k ];
	//		topRow[ k ] <<= shift1D;
	//		leftColumn[ k ] <<= shift1D;
	//	}

	//	// Generate prediction signal
	//	for( k = 0; k < blkSize; k++ )
	//	{
	//		horPred = leftColumn[ k ] + offset2D;
	//		for( l = 0; l < blkSize; l++ )
	//		{
	//			horPred += rightColumn[ k ];
	//			topRow[ l ] += bottomRow[ l ];
	//			rpDst[ k*dstStride + l ] = ( ( horPred + topRow[ l ] ) >> shift2D );
	//		}
	//	}
	//}
}

DcMode::DcMode( )
{
}

DcMode::~DcMode( )
{
}

Void DcMode::calcDcVal( )
{
	UInt size = itsCurrentPB->getSize( );
	itsDCVal = size;
	for( UInt x = 0; x < size; ++x )
		itsDCVal += itsReferenceValues[ INTRA_DIR_LEFT ][ x ] + itsReferenceValues[ INTRA_DIR_TOP ][ x ];
	itsDCVal >>= log2Int( size ) + 1;
}

Sample DcMode::getFiltCorner( ) const
{
	if( itsFiltreEdges )
		return ( itsReferenceValues[ INTRA_DIR_LEFT ][ 0 ] + 2 * itsDCVal + itsReferenceValues[ INTRA_DIR_TOP ][ 0 ] + 2 ) >> 2;
	else
		return itsDCVal;
}

Sample DcMode::getFiltEdge( const IntraDirection dir, const UInt offset ) const
{
	assert( dir != INTRA_DIR_CORNER );
	assert( itsReferenceValues != nullptr && itsReferenceValues[ INTRA_DIR_LEFT ] != nullptr && itsReferenceValues[ INTRA_DIR_TOP ] != nullptr );

	const Sample* refs = itsReferenceValues[ dir ];

	if( itsFiltreEdges )
		return ( refs[ offset ] + 3 * itsDCVal + 2 ) >> 2;
	else
		return itsDCVal;
}

Sample** DcMode::calcPred( )
{
	assert( itsCurrentPB != nullptr );
	assert( itsReferenceValues != nullptr && itsReferenceValues[ INTRA_DIR_LEFT ] != nullptr && itsReferenceValues[ INTRA_DIR_TOP ] != nullptr );

	calcDcVal( );

	itsFiltreEdges = ( itsCurrentPB->getImgComp( ) == LUMA ) && ( itsCurrentPB->getSize( ) < 32 );

	Sample** pred = initPred( );
	pred[ 0 ][ 0 ] = getFiltCorner( );
	for( Int x = 1; x < itsCurrentPB->getSize( ); x++ )
	{
		pred[ 0 ][ x ] = getFiltEdge( INTRA_DIR_LEFT, x );
		pred[ x ][ 0 ] = getFiltEdge( INTRA_DIR_TOP, x );
	}

	for( Int x = 1; x < itsCurrentPB->getSize( ); x++ )
	{
		for( Int y = 1; y < itsCurrentPB->getSize( ); y++ )
			pred[ x ][ y ] = itsDCVal;
	}

	return pred;
}

LinearMode::LinearMode( )
{
}

LinearMode::~LinearMode( )
{
}

Sample LinearMode::getFiltEdge( const IntraDirection dir, const UInt offset )
{
	const Sample* mainRefs = itsReferenceValues[ dir ];
	const Sample* sideRefs = itsReferenceValues[ ( dir + 1 ) % 2 ];
	int ref = mainRefs[ 0 ] + ( ( sideRefs[ offset ] - itsCornerReference ) >> 1 );
	return SeqParams( )->clip( itsCurrentPB->getImgComp( ), ref );
}

Sample** LinearMode::calcPred( )
{
	assert( itsCurrentPB != nullptr );
	assert( itsReferenceValues != nullptr && itsReferenceValues[ INTRA_DIR_LEFT ] != nullptr && itsReferenceValues[ INTRA_DIR_TOP ] != nullptr );

	IntraDirection dir = itsCurrentPB->getModeIdx( ) == 10 ? INTRA_DIR_LEFT : INTRA_DIR_TOP;
	bool filtEdge = ( itsCurrentPB->getImgComp( ) == LUMA ) && ( itsCurrentPB->getSize( ) < 32 );

	Sample **pred = initPred( );

	pred[ 0 ][ 0 ] = itsReferenceValues[ dir ][ 0 ];
	if( filtEdge )
		pred[ 0 ][ 0 ] = getFiltEdge( dir, 0 );

	for( Int x = 1; x < itsCurrentPB->getSize( ); x++ )
		pred[ x ][ 0 ] = dir == INTRA_DIR_TOP ? itsReferenceValues[ INTRA_DIR_TOP ][ x ] : filtEdge ? getFiltEdge( INTRA_DIR_LEFT, x ) : itsReferenceValues[ INTRA_DIR_LEFT ][ 0 ];

	for( Int y = 1; y < itsCurrentPB->getSize( ); y++ )
		pred[ 0 ][ y ] = dir == INTRA_DIR_LEFT ? itsReferenceValues[ INTRA_DIR_LEFT ][ y ] : filtEdge ? getFiltEdge( INTRA_DIR_TOP, y ) : itsReferenceValues[ INTRA_DIR_TOP ][ 0 ];

	for( Int x = 1; x < itsCurrentPB->getSize( ); x++ )
	for( Int y = 1; y < itsCurrentPB->getSize( ); y++ )
		pred[ x ][ y ] = dir == INTRA_DIR_LEFT ? itsReferenceValues[ INTRA_DIR_LEFT ][ y ] : itsReferenceValues[ INTRA_DIR_TOP ][ x ];

	return pred;
}

const Int AngMode::angles[] = { 32, 26, 21, 17, 13, 9, 5, 2, 0, -2, -5, -9, -13, -17, -21, -26, -32, -26, -21, -17, -13, -9, -5,
-2, 0, 2, 5, 9, 13, 17, 21, 26, 32 };

const Int AngMode::invAngles[] = { 4096, 1638, 910, 630, 482, 390, 315, 256, 315, 390, 482, 630, 910, 1638, 4096 };

AngMode::AngMode( )
{
	refsArray = nullptr;
}

AngMode::~AngMode( )
{
	if( refsArray != nullptr )
		delete[] refsArray;
}

int AngMode::getAngle( ) const
{
	assert( ( itsCurrentPB->getModeIdx( ) != 0 ) && ( itsCurrentPB->getModeIdx( ) != 1 ) && ( itsCurrentPB->getModeIdx( ) != 10 ) && ( itsCurrentPB->getModeIdx( ) != 26 ) );
	return AngMode::angles[ itsCurrentPB->getModeIdx( ) - 2 ];
}

int AngMode::getInvAngle( ) const
{
	assert( ( itsCurrentPB->getModeIdx( ) > 10 ) || ( itsCurrentPB->getModeIdx( ) < 26 ) );
	return AngMode::invAngles[ itsCurrentPB->getModeIdx( ) - 11 ];
}

void AngMode::getRefsArray( )
{
	assert( itsCurrentPB != nullptr );
	assert( itsReferenceValues != nullptr && itsReferenceValues[ INTRA_DIR_LEFT ] != nullptr && itsReferenceValues[ INTRA_DIR_TOP ] != nullptr );

	Int angle = getAngle( );

	Bool modeHor = itsCurrentPB->getModeIdx( ) < 18;

	UInt size = itsCurrentPB->getSize( );

	if( refsArray != nullptr )
		delete[] refsArray;
	refsArray = new Sample[ 2 * size + 1 ];

	UInt start = angle > 0 ? 0 : size;
	refsArray[ start++ ] = itsCornerReference;
	for( UInt x = start; x < 2 * size + 1; x++ )
		refsArray[ x ] = modeHor ? itsReferenceValues[ INTRA_DIR_LEFT ][ x - start ] : itsReferenceValues[ INTRA_DIR_TOP ][ x - start ];

	if( angle < 0 )
	{
		Int invAngle = getInvAngle( );
		Int invAngleSum = 128;
		Int limit = ( angle * size ) >> 5;
		for( Int x = -1; x > limit; x-- )
		{
			invAngleSum += invAngle;
			Int refIdx = ( invAngleSum >> 8 ) - 1;
			refsArray[ size + x ] = modeHor ? itsReferenceValues[ INTRA_DIR_TOP ][ refIdx ] : itsReferenceValues[ INTRA_DIR_LEFT ][ refIdx ];
		}
	}
}

Sample **AngMode::calcPred( )
{
	assert( itsCurrentPB != nullptr );
	assert( itsReferenceValues != nullptr && itsReferenceValues[ INTRA_DIR_LEFT ] != nullptr && itsReferenceValues[ INTRA_DIR_TOP ] != nullptr );

	getRefsArray( );

	Int angle = getAngle( );
	Sample* refs = angle >= 0 ? refsArray : refsArray + itsCurrentPB->getSize( );

	Bool modeHor = itsCurrentPB->getModeIdx( ) < 18;

	Sample **pred = initPred( );
	Int angleSum = 0, iFact, iIdx, refIdx;
	for( Int x = 0; x < itsCurrentPB->getSize( ); x++ )
	{
		angleSum += angle;
		iFact = angleSum & 31;
		iIdx = angleSum >> 5;
		for( Int y = 0; y < itsCurrentPB->getSize( ); y++ )
		{
			refIdx = iIdx + y + 1;
			Int comp0 = iFact != 32 ? ( 32 - iFact ) * refs[ refIdx ] : 0;
			Int comp1 = iFact != 0 ? iFact * refs[ refIdx + 1 ] : 0;
			pred[ x ][ y ] = ( comp0 + comp1 + 16 ) >> 5;
		}
	}

	if( !modeHor )
	for( Int y = 0; y < itsCurrentPB->getSize( ); y++ )
	for( Int x = y; x < itsCurrentPB->getSize( ); x++ )
	{
		Int swap = pred[ x ][ y ];
		pred[ x ][ y ] = pred[ y ][ x ];
		pred[ y ][ x ] = swap;
	}

	delete[] refsArray;
	refsArray = nullptr;

	return pred;
}
