#ifndef _CUQUADTREE_H_
#define _CUQUADTREE_H_

#include "Utils.h"
#include "Picture.h"
#include "UnitBase.h"

class CU;
class TU;
class CUIntra;
class PUIntra;

template< class T, class self >
class QuadTree : public CTUComponentBase
{
protected:
	std::shared_ptr<self > itsSubTrees[ 4 ];
	std::shared_ptr<T> itsUnit;

public:
	QuadTree( CTU* parentCTU, UInt x, UInt y, UInt size );
	virtual ~QuadTree( );

#pragma region Akcesory

	std::shared_ptr<self> getSubTree( QTComponent placement )
	{
		return itsSubTrees[ placement ];
	}
	std::shared_ptr<T> getLeaf( )
	{
		return itsUnit;
	}
	Void setLeaf( std::shared_ptr<T> rhs )
	{
		itsUnit = rhs;
	}

#pragma endregion

	virtual std::shared_ptr<self > createSubTree( UInt x, UInt y, UInt size ) = 0;

	Void createSubTrees( )
	{
		itsSubTrees[ UPPERLEFT ] = createSubTree( itsX, itsY, itsSize / 2 );

		Bool createRight = itsX + itsSize / 2 < SeqParams( )->getPicWidth( );
		Bool createLower = itsY + itsSize / 2 < SeqParams( )->getPicHeight( );

		if( createRight )
			itsSubTrees[ UPPERRIGHT ] = createSubTree( itsX + ( itsSize / 2 ), itsY, itsSize / 2 );

		if( createLower )
			itsSubTrees[ LOWERLEFT ] = createSubTree( itsX, itsY + ( itsSize / 2 ), itsSize / 2 );

		if( createLower && createRight )
			itsSubTrees[ LOWERRIGHT ] = createSubTree( itsX + ( itsSize / 2 ), itsY + ( itsSize / 2 ), itsSize / 2 );

		itsUnit = nullptr;
	}

	Bool isLeaf( )
	{
		return itsUnit != nullptr;
	}

	Void printSubTreesDescription( )
	{
		if( itsSubTrees[ UPPERLEFT ] != nullptr )
		{
			LOG( "PART" ) << "UPPERLEFT" << std::endl;
			itsSubTrees[ UPPERLEFT ]->printDescription( );
		}
		if( itsSubTrees[ UPPERRIGHT ] != nullptr )
		{
			LOG( "PART" ) << "UPPERRIGHT" << std::endl;
			itsSubTrees[ UPPERRIGHT ]->printDescription( );
		}
		if( itsSubTrees[ LOWERLEFT ] != nullptr )
		{
			LOG( "PART" ) << "LOWERLEFT" << std::endl;
			itsSubTrees[ LOWERLEFT ]->printDescription( );
		}
		if( itsSubTrees[ LOWERRIGHT ] != nullptr )
		{
			LOG( "PART" ) << "LOWERRIGHT" << std::endl;
			itsSubTrees[ LOWERRIGHT ]->printDescription( );
		}
	}
};

class CUQuadTree : public QuadTree<CUIntra, CUQuadTree>
{
private:

public:
	CUQuadTree( CTU* parentCTU, UInt x, UInt y, UInt size );
	virtual ~CUQuadTree( );

	std::shared_ptr<CUIntra> getCU( )
	{
		return itsUnit;
	}

	virtual std::shared_ptr<CUQuadTree > createSubTree( UInt x, UInt y, UInt size )
	{
		return std::make_shared<CUQuadTree>( itsParentCTU, x, y, size );
	}

	virtual Void printDescription( );
	Void reconstructionLoop( );
};

class TUQuadTree : public QuadTree<TU, TUQuadTree>
{
private:
	CU* itsParentCu;

public:
	TUQuadTree( CU* parentCU, UInt x, UInt y, UInt size );
	virtual ~TUQuadTree( );

	std::shared_ptr<TU> getTU( )
	{
		return itsUnit;
	}
	CU* getParentCU( )
	{
		return itsParentCu;
	}

	virtual std::shared_ptr<TUQuadTree > createSubTree( UInt x, UInt y, UInt size )
	{
		return std::make_shared<TUQuadTree>( itsParentCu, x, y, size );
	}

	virtual Void printDescription( );
	Void reconstructionLoop( PUIntra* mainPU );
};
#endif