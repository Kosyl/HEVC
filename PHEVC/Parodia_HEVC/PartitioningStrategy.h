#ifndef _PARTITIONINGSTRATEGY_H_
#define _PARTITIONINGSTRATEGY_H_

#include "Utils.h"
#include "CTU.h"
#include "QuadTree.h"
#include "CU.h"
#include "TU.h"
#include "PU.h"

class PartitioningStrategy
{
public:
	PartitioningStrategy( )
	{
	}
	virtual ~PartitioningStrategy( )
	{
	}

	virtual Void doPartitioning( std::shared_ptr<CTU> ctu ) = 0;
};

class DefaultBigPartitions : public PartitioningStrategy
{
	static DefaultBigPartitions* instance;
	DefaultBigPartitions( )
	{
	}
public:
	static DefaultBigPartitions* getStrategy( )
	{
		if( instance == nullptr )
			instance = new DefaultBigPartitions( );
		return instance;
	}

	virtual ~DefaultBigPartitions( )
	{
	}

	virtual Void doPartitioning( std::shared_ptr<CTU> ctu )
	{
		doPartitioning( ctu->getCUQuadTree( ) );
	}

	Void doPartitioning( std::shared_ptr<CUQuadTree> CUtree )
	{
		if( CUtree->getSize( ) <= SeqParams( )->getMaxCUSize( ) && CUtree->getSize( ) <= SeqParams( )->getPicWidth( ) && CUtree->getSize( ) <= SeqParams( )->getPicHeight( ) )
		{
			CUtree->setLeaf( std::make_shared<CUIntra>( CUtree->getParentCTU( ), CUtree->getX( ), CUtree->getY( ), CUtree->getSize( ) ) );
		}
		else
		{
			CUtree->createSubTrees( );

			for( UInt i = QTCOMPONENT_FIRST; i <= QTCOMPONENT_LAST; ++i )
			{
				if( CUtree->getSubTree( (QTComponent)i ) != nullptr )
					doPartitioning( CUtree->getSubTree( (QTComponent)i ) );
			}
		}
	}

};

class DefaultSmallPartitions : public PartitioningStrategy
{
private:

	static DefaultSmallPartitions* instance;
	DefaultSmallPartitions( )
	{
	}
public:
	static DefaultSmallPartitions* getStrategy( )
	{
		if( instance == nullptr )
			instance = new DefaultSmallPartitions( );
		return instance;
	}

	virtual ~DefaultSmallPartitions( )
	{
	}

	Void doPartitioning( std::shared_ptr<CTU> ctu )
	{
		doPartitioning( ctu->getCUQuadTree( ) );
	}
	Void doPartitioning( std::shared_ptr<CUQuadTree> CUtree )
	{
		if( CUtree->getSize( ) <= SeqParams( )->getMinCUSize( ) && CUtree->getSize( ) <= SeqParams( )->getPicWidth( ) && CUtree->getSize( ) <= SeqParams( )->getPicHeight( ) )
		{
			CUtree->setLeaf( std::make_shared<CUIntra>( CUtree->getParentCTU( ), CUtree->getX( ), CUtree->getY( ), CUtree->getSize( ) ) );
			CUtree->getLeaf( )->setIntraPredictionDerivationType( 0 );
			doPartitioning( CUtree->getLeaf( ) );
		}
		else
		{
			CUtree->createSubTrees( );

			for( UInt i = QTCOMPONENT_FIRST; i <= QTCOMPONENT_LAST; ++i )
			{
				if( CUtree->getSubTree( (QTComponent)i ) != nullptr )
					doPartitioning( CUtree->getSubTree( (QTComponent)i ) );
			}
		}
	}
	Void doPartitioning( std::shared_ptr<CUIntra> cu )
	{
		cu->setPartitionMode( PART_NxN );
		cu->createPUs( 10 );
		doPartitioning( cu->getTransformTree( ) );
		for( QTComponent position = QTCOMPONENT_FIRST; position <= QTCOMPONENT_LAST; ++position )
		{
			cu->getPU( position )->setTransformArea( cu->getTransformTree( )->getSubTree( position ) );
		}
	}
	Void doPartitioning( std::shared_ptr<TUQuadTree> TUTree )
	{
		if( ( TUTree->getSize( ) <= SeqParams( )->getMinTUSize( ) ) )
		{
			TUTree->setLeaf( std::make_shared<TU>( TUTree->getParentCU( ), TUTree->getX( ), TUTree->getY( ), TUTree->getSize( ) ) );
		}
		else
		{
			TUTree->createSubTrees( );

			for( QTComponent position = QTCOMPONENT_FIRST; position <= QTCOMPONENT_LAST; ++position )
			{
				if( TUTree->getSubTree( position ) != nullptr )
					doPartitioning( TUTree->getSubTree( position ) );
			}
		}
	}
};

#endif