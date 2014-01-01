#ifndef UnitBase_h__
#define UnitBase_h__

#include "Utils.h"
#include "Picture.h"

class UnitBase
{
protected:
	UInt itsIdx;
	UShort itsSize;
	UShort itsX, itsY;
	Picture* itsParentPicture;

public:
	UnitBase( Picture* picture, UShort x, UShort y, UShort size ) :
		itsX( x ),
		itsY( y ),
		itsIdx( getZScanIdxIn4x4( x, y ) ),
		itsSize( size ),
		log( )
	{
		assert
			(
			picture != nullptr &&
			x < SeqParams( )->getPicWidth( ) &&
			x % SeqParams( )->getMinTUSize( ) == 0 &&
			y < SeqParams( )->getPicHeight( ) &&
			y % SeqParams( )->getMinTUSize( ) == 0 &&
			size > 0 && size % 4 == 0
			);
		itsParentPicture = picture;
	}

	~UnitBase( )
	{
	}

#pragma region Akcesory

	UShort getY( ) const
	{
		return itsY;
	}
	UShort getX( ) const
	{
		return itsX;
	}
	UShort getSize( ) const
	{
		return itsSize;
	}
	UInt getIdx( ) const
	{
		return itsIdx;
	}
	Void setIdx( UInt val )
	{
		itsIdx = val;
	}
	Picture* getPicture( )
	{
		return itsParentPicture;
	}

#pragma endregion

	virtual Void printDescription(  )
	{
	}

	Logger log;
};

class CTUComponentBase : public UnitBase
{
protected:
	CTU* itsParentCTU;

public:
	CTUComponentBase( Picture* picture, CTU* ctu, UShort x, UShort y, UShort size ):
		UnitBase(picture, x, y, size ),
		itsParentCTU(ctu)
	{
		assert( ctu != nullptr );
	}

	CTU* getParentCTU( )
	{
		return itsParentCTU;
	}

	~CTUComponentBase( )
	{
	}
};

#endif // UnitBase_h__