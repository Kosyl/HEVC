#ifndef SeqPicParameters_h__
#define SeqPicParameters_h__

#include "Utils.h"


class ZScanArray
{
	static Int** itsValues;
	static UInt itsSizeX, itsSizeY;

public:
	static Void reset( );
	static Int get( UInt x, UInt y );
	static Void print( std::ostream& out );
};

class SequenceParameters
{
private:
	static SequenceParameters *itsInstance;
	SequenceParameters( );

	UShort itsPicWidth, itsPicHeight, itsPicWidthInCTUs, itsPicHeightInCTUs;
	UShort itsLog2CTUSize, itsLog2MaxCUSize, itsLog2MinCUSize, itsLog2MaxTUSize, itsLog2MinTUSize;
	
	UShort itsBitDepthLuma, itsBitDepthChroma;
	UShort itsMaxCUSize, itsMinCUSize; //def. 64 i 8
	UShort itsMaxTUSize, itsMinTUSize; // def. 32 i 4
	Bool itsSAOEnabled;
	Bool itsIntraSmoothingEnabled; //vide 8.4.4.2.3

public:
	static SequenceParameters* getInstance( );
	~SequenceParameters( );

	UShort getPicWidth( ) const
	{
		return itsPicWidth;
	}
	UShort getPicWidthInCTUs( ) const
	{
		return itsPicWidthInCTUs;
	}
	Void setPicWidth( UShort val )
	{
		itsPicWidth = val;
		itsPicWidthInCTUs = ( val - 1 ) / itsMaxCUSize + 1;
	}
	UShort getPicHeight( ) const
	{
		return itsPicHeight;
	}
	UShort getPicHeightInCTUs( ) const
	{
		return itsPicHeightInCTUs;
	}
	Void setPicHeight( UShort val )
	{
		itsPicHeight = val;
		itsPicHeightInCTUs = ( val - 1 ) / itsMaxCUSize + 1;
	}

	UShort getBitDepthChroma( ) const
	{
		return itsBitDepthChroma;
	}
	Void setBitDepthChroma( UShort val )
	{
		itsBitDepthChroma = val;
	}
	UShort getBitDepthLuma( ) const
	{
		return itsBitDepthLuma;
	}
	Void setBitDepthLuma( UShort val )
	{
		itsBitDepthLuma = val;
	}
	UShort getBitDepth( ImgComp comp ) const
	{
		return comp == ImgComp::LUMA ? itsBitDepthLuma : itsBitDepthChroma;
	}
	Void setBitDepthLuma( ImgComp comp, UShort val )
	{
		if( comp == ImgComp::LUMA )
			itsBitDepthLuma = val;
		else
			itsBitDepthChroma = val;
	}

	Bool getSAOEnabled( ) const
	{
		return itsSAOEnabled;
	}
	Void setSAOEnabled( Bool val )
	{
		itsSAOEnabled = val;
	}

	UShort getMinCUSize( ) const
	{
		return itsMinCUSize;
	}
	Void setMinCUSize( UShort val )
	{
		itsMinCUSize = val;
		itsLog2MinCUSize = log2Int( val );
	}
	UShort getMaxCUSize( ) const
	{
		return itsMaxCUSize;
	}
	Void setMaxCUSize( UShort val )
	{
		itsMaxCUSize = val;
		itsLog2MaxCUSize = log2Int( val );
		itsLog2CTUSize = log2Int( val );
	}
	UShort getCTUSize( ) const
	{
		return getMaxCUSize( );
	}

	Bool getIntraSmoothingEnabled( ) const
	{
		return itsIntraSmoothingEnabled;
	}
	Void setIntraSmoothingEnabled( Bool val )
	{
		itsIntraSmoothingEnabled = val;
	}

	UShort getMinTUSize( ) const
	{
		return itsMinTUSize;
	}
	Void setMinTUSize( UShort val )
	{
		itsMinTUSize = val;
		itsLog2MinTUSize = log2Int( val );
	}

	UShort getMaxTUSize( ) const
	{
		return itsMaxTUSize;
	}
	Void setMaxTUSize( UShort val )
	{
		itsMaxTUSize = val;
		itsLog2MaxTUSize = log2Int( val );
	}

	Sample clip( ImgComp comp, Int value )
	{
		UShort maxVal = comp == LUMA ? 1 << itsBitDepthLuma : 1 << itsBitDepthChroma;
		return clipToRange<Sample>( 0, maxVal, value );
	}

	Sample getDefaultSampleValue( ImgComp comp )
	{
		if( comp == LUMA )
			return 1 << ( itsBitDepthLuma - 1 );
		else
			return 1 << ( itsBitDepthChroma - 1 );
	}
	UShort getLog2MinTUSize( ) const
	{
		return itsLog2MinTUSize;
	}
	UShort getLog2MaxTUSize( ) const
	{
		return itsLog2MaxTUSize;
	}
	UShort getLog2MinCUSize( ) const
	{
		return itsLog2MinCUSize;
	}
	UShort getLog2MaxCUSize( ) const
	{
		return itsLog2MaxCUSize;
	}
	UShort getLog2CTUSize( ) const
	{
		return itsLog2CTUSize;
	}
	Void initWithDefaults( );
};

class PictureParameters
{
private:
	static PictureParameters *itsInstance;
	PictureParameters( );

	Bool itsSDHEnabled;
	UShort itsQP;
	Short itsQPOffsetForCb, itsQpOffsetForCr;
	Bool itsTransformSkipEnabled;
	Bool itsTilesEnabled;
	Bool itsDeblockingFilterEnabled;
	Bool itsLoopFilterOnSlicesEnabled, itsLoopFilterOnTilesEnabled;

public:
	static PictureParameters* getInstance( );
	~PictureParameters( );

#pragma region Akcesory

	Bool getLoopFilterOnTilesEnabled( ) const
	{
		return itsLoopFilterOnTilesEnabled;
	}
	Void setLoopFilterOnTilesEnabled( Bool val )
	{
		itsLoopFilterOnTilesEnabled = val;
	}
	Bool getLoopFilterOnSlicesEnabled( ) const
	{
		return itsLoopFilterOnSlicesEnabled;
	}
	Void setLoopFilterOnSlicesEnabled( Bool val )
	{
		itsLoopFilterOnSlicesEnabled = val;
	}
	Bool getDeblockingFilterEnabled( ) const
	{
		return itsDeblockingFilterEnabled;
	}
	Void setDeblockingFilterEnabled( Bool val )
	{
		itsDeblockingFilterEnabled = val;
	}
	Bool getSDHEnabled( ) const
	{
		return itsSDHEnabled;
	}
	Void setSDHEnabled( Bool val )
	{
		itsSDHEnabled = val;
	}
	UShort getQP( ImgComp comp = ImgComp::LUMA ) const
	{
		if( comp == ImgComp::LUMA )
			return itsQP;
		else if( comp == ImgComp::CB )
			return itsQP + itsQPOffsetForCb;
		else
			return itsQP + itsQpOffsetForCr;
	}
	Void setQP( UShort val )
	{
		itsQP = val;
	}
	Short getQpOffsetForCr( ) const
	{
		return itsQpOffsetForCr;
	}
	Void setQpOffsetForCr( Short val )
	{
		itsQpOffsetForCr = val;
	}
	Short getQPOffsetForCb( ) const
	{
		return itsQPOffsetForCb;
	}
	Void setQPOffsetForCb( Short val )
	{
		itsQPOffsetForCb = val;
	}
	Bool getTransformSkipEnabled( ) const
	{
		return itsTransformSkipEnabled;
	}
	Void setTransformSkipEnabled( Bool val )
	{
		itsTransformSkipEnabled = val;
	}
	Bool getTilesEnabled( ) const
	{
		return itsTilesEnabled;
	}
	Void setTilesEnabled( Bool val )
	{
		itsTilesEnabled = val;
	}
	
#pragma endregion Akcesory

	Void initWithDefaults( );
};

PictureParameters* PicParams( );

SequenceParameters* SeqParams( );

UInt getRasterIdxIn4x4( UInt x, UInt y );

UInt getZScanIdxIn4x4( const UInt puX, const UInt puY );
#endif // SeqPicParameters_h__