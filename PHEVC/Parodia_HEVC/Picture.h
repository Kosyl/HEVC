#ifndef _PICTURE_H_
#define _PICTURE_H_

#include "TypeDef.h"
#include "Logger.h"
#include "Utils.h"
#include "SeqPicParameters.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string.h>

class Tile;
class CTU;
class Slice;
class PartitioningStrategy;

class Picture
{
protected:

	Sample** itsSamplesY;
	Sample** itsSamplesCb;
	Sample** itsSamplesCr;

	Sample** itsReconY;
	Sample** itsReconCb;
	Sample** itsReconCr;

	std::vector<std::shared_ptr<Tile> > itsTiles;
	std::vector<std::shared_ptr<Slice> > itsSlices;
	std::vector<std::shared_ptr<CTU> > itsCTUs;

	std::shared_ptr<Tile> itsCurrentTile;
	std::shared_ptr<Slice> itsCurrentSlice;
	std::shared_ptr<CTU> itsCurrentCTU;

	Void deleteMatrices( );
	Void setSize( UInt width, UInt height );

public:
	Picture( );
	~Picture( );

	Void initFrame( );
	Void loadFrameFromYuv( std::ifstream& yuvStream );

#pragma region Akcesory

	Sample** samples( const ImgComp comp ) const;
	Sample** reconMatrix( const ImgComp comp ) const;

	std::vector<std::shared_ptr<Tile> > & getTiles( )
	{
		return itsTiles;
	}
	std::vector<std::shared_ptr<Slice> > & getSlices( )
	{
		return itsSlices;
	}
	std::shared_ptr<CTU> getCTU( UInt ctuX, UInt ctuY )
	{
		return itsCTUs[ ctuX + ctuY * SeqParams( )->getPicWidthInCTUs( ) ];
	}
	std::shared_ptr<CTU> getCTUBySamplePosition( UInt sampleX, UInt sampleY )
	{
		UInt ctuX = sampleX / SeqParams( )->getCTUSize( ), ctuY = sampleY / SeqParams( )->getCTUSize( );

		return getCTU( ctuX, ctuY );
	}
	std::shared_ptr<Tile> getCurrentTile( )
	{
		return itsCurrentTile;
	}
	std::shared_ptr<Slice> getCurrentSlice( )
	{
		return itsCurrentSlice;
	}
	std::shared_ptr<CTU> getCurrentCTU( )
	{
		return itsCurrentCTU;
	}

#pragma endregion

	Void reset( );

	Void initPartitions( PartitioningStrategy* strategy );
	Void initCTUs( );

	Void printDescription( );
	Void reconstructionLoop( );
	Logger log;
};

#endif