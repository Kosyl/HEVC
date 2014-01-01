#ifndef _TILE_H_
#define _TILE_H_

#include "Utils.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include "SeqPicParameters.h"

class CTU;

class Tile
{
private:
	UShort itsX, itsY;
	UShort itsWidth, itsWidthInCTBs, itsHeight, itsHeightInCTBs;
	UShort itsIdx;

	std::vector<std::shared_ptr<CTU> > itsCTUs;

public:
	Tile( );
	Tile( UShort x, UShort y, UShort width, UShort height );
	~Tile( );

#pragma region Akcesory

	UShort getY( ) const
	{
		return itsY;
	}
	UShort getX( ) const
	{
		return itsX;
	}
	UShort getIdx( ) const
	{
		return itsIdx;
	}
	Void setIdx( UShort val )
	{
		itsIdx = val;
	}
	UShort getHeightInCTBs( ) const
	{
		return itsHeightInCTBs;
	}
	UShort getHeight( ) const
	{
		return itsHeight;
	}
	UShort getWidthInCTBs( ) const
	{
		return itsWidthInCTBs;
	}
	UShort getWidth( ) const
	{
		return itsWidth;
	}

#pragma endregion 

	Void appendCTU( std::shared_ptr<CTU> ctu );
};

#endif