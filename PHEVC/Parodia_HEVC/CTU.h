#ifndef CTU_h__
#define CTU_h__

#include "Utils.h"
#include "SAOInfo.h"
#include "Picture.h"
#include "UnitBase.h"
#include "QuadTree.h"

class CTU : public UnitBase
{
private:
	std::shared_ptr<SAOInfo> itsSAOInfo;
	std::weak_ptr<Tile> itsParentTile;
	std::weak_ptr<Slice> itsParentSlice;
	std::shared_ptr<CUQuadTree> itsCUQuadTree;

public:
	CTU( Picture* picture, std::shared_ptr<Slice> slice, UShort x, UShort y );
	CTU( Picture* picture, std::shared_ptr<Slice> slice, std::shared_ptr<Tile> tile, UShort x, UShort y );
	virtual ~CTU( );

#pragma region Akcesory

	std::shared_ptr<SAOInfo> getSAOInfo( )
	{
		return itsSAOInfo;
	}
	std::shared_ptr<CUQuadTree> getCUQuadTree( )
	{
		return itsCUQuadTree;
	}

#pragma endregion

	Bool isPartitioned( )
	{
		return itsCUQuadTree != nullptr;
	}

	virtual Void printDescription( );

	Void reconstructionLoop( );
};
#endif // CTU_h__