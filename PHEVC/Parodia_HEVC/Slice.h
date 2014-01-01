#ifndef Slice_h__
#define Slice_h__

#include "Utils.h"
#include <vector>
#include "Picture.h"

class Slice
{
private:
	SliceType itsSliceType;
	Bool itsSAOLumaEnabled, itsSAOChromaEnabled;
	Bool itsDeblockingFilterEnabled;

	Bool itsLoopFilterEnabled;

	Short itsQPDelta;
	Short itsQPOffsetForCr, itsQPOffsetForCb;
	Bool itsECSyncEnabled;

	std::vector<std::weak_ptr<CTU> > itsCTUs;
	Picture* itsParentPicture;

public:
	Slice( Picture* picture, SliceType type = SliceType::SLICE_I );
	~Slice( );

#pragma region Akcesory

	Picture* getParentPicture( ) const
	{
		return itsParentPicture;
	}
	SliceType getType( ) const
	{
		return itsSliceType;
	}
	Void setSliceType( SliceType type )
	{
		itsSliceType = type;
	}
	Bool getSAOChromaEnabled( ) const
	{
		return itsSAOChromaEnabled;
	}
	Void setSAOChromaEnabled( Bool val )
	{
		itsSAOChromaEnabled = val;
	}
	Bool getSAOLumaEnabled( ) const
	{
		return itsSAOLumaEnabled;
	}
	Void setSAOLumaEnabled( Bool val )
	{
		itsSAOLumaEnabled = val;
	}
	Bool getsLoopFilterEnabled( ) const
	{
		return itsLoopFilterEnabled;
	}
	Void setLoopFilterEnabled( Bool val )
	{
		itsLoopFilterEnabled = val;
	}
	Bool getDeblockingFilterEnabled( ) const
	{
		return itsDeblockingFilterEnabled;
	}
	Void setDeblockingFilterEnabled( Bool val )
	{
		itsDeblockingFilterEnabled = val;
	}
	Short getQPDelta( ImgComp comp )
	{
		switch( comp )
		{
		case CB:
			return itsQPDelta + itsQPOffsetForCb;
		case CR:
			return itsQPDelta + itsQPOffsetForCr;
		case LUMA:
		default:
			return itsQPDelta;
		}
	}
#pragma endregion

	Void appendCTU( std::shared_ptr<CTU> ctu );
};
#endif // Slice_h__
