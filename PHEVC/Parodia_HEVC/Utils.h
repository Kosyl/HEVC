#ifndef _UTILS_
#define _UTILS_

#include <algorithm>
#include "TypeDef.h"
#include "Logger.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> 
#include <sstream>
#include <assert.h>
#include <memory>

/** \file     Utils.h
\brief    Proste funkcje uzywane w roznych miejscach (header)
*/

#pragma region Enums

enum ImgComp
{
	LUMA = 0, CB = 1, CR = 2
};
enum IntraDirection
{
	INTRA_DIR_LEFT = 0, INTRA_DIR_TOP = 1, INTRA_DIR_CORNER = 2
};
enum IntraModeType
{
	INTRAMODE_PLANAR = 0, INTRAMODE_DC = 1, INTRAMODE_LINEAR = 2, INTRAMODE_ANGULAR = 3
};
enum ScanOrder // 2 dla predMode 6-14, 1 dla 22-30, 0 wpp
{
	SCAN_DIAGONAL = 0, SCAN_HORIZONTAL = 1, SCAN_VERTICAL = 2
};
enum SliceType
{
	SLICE_I = 0, SLCIE_P = 1, SLICE_B = 2
};
enum SAOType
{
	SAO_NONE = 0, SAO_BAND = 1, SAO_EDGE = 2
};
enum QTComponent
{
	UPPERLEFT = 0, UPPERRIGHT = 1, LOWERLEFT = 2, LOWERRIGHT = 3, QTCOMPONENT_FIRST = UPPERLEFT, QTCOMPONENT_LAST = LOWERRIGHT
};
QTComponent& operator++( QTComponent& orig );
enum PartitionMode
{
	PART_NxN, PART_2Nx2N, PART_2NxN, PART_Nx2N, PART_2NxnU, PART_2NxnD, PART_nLx2N, PART_nRx2N, PART_NOTSET
};

#pragma endregion

template <typename T>
Void printMatrix( T** matrix, UInt size, std::ostream& out = std::cout )
{
	printMatrix<T>( matrix, size, size, out );
}

template <typename T>
Void printMatrix( T** matrix, UInt sizeX, UInt sizeY, std::ostream& out = std::cout, UInt offsetX = 0, UInt offsetY = 0, std::string tabs = "" )
{
	for( UInt i = offsetY; i < offsetY+sizeY; ++i )
	{
		out << tabs;
		for( UInt j = offsetX; j < offsetX+sizeX; ++j )
		{
			out << std::setw( 4 ) << matrix[ j ][ i ] << " ";
		}
		out << std::endl;
	}
}

template <typename T>
Void printTable( T* matrix, UInt size, std::ostream& out = std::cout, UInt width = 4 )
{
	assert( size > 0 );
	for( UInt j = 0; j < size; ++j )
	{
		if( width > 0 )
			out << std::setw( width );
		out << matrix[ j ] << " ";
	}
	out << std::endl;
}

template <typename T>
Void printMatrix( T** matrix, UInt size, Logger& out )
{
	printMatrix<T>( matrix, size, size, out );
}

template <typename T>
Void printMatrix( T** matrix, UInt sizeX, UInt sizeY, Logger& out, UInt offsetX = 0, UInt offsetY = 0, std::string tabs = "" )
{
	for( UInt i = offsetY; i < offsetY + sizeY; ++i )
	{
		out << tabs;
		for( UInt j = offsetX; j < offsetX + sizeX; ++j )
		{
			out << std::setw( 4 ) << matrix[ j ][ i ] << " ";
		}
		out << std::endl;
	}
}

template <typename T>
Void printTable( T* matrix, UInt size, Logger& out, UInt width = 4 )
{
	assert( size > 0 );
	for( UInt j = 0; j < size; ++j )
	{
		if( width > 0 )
			out << std::setw( width );
		out << matrix[ j ] << " ";
	}
	out << std::endl;
}

template <typename T>
T** getEmptyMatrix( UInt size1, UInt size2 )
{
	assert( size1 > 0 && size2 > 0 );
	T** tmp = new T*[ size1 ];
	for( UInt i = 0; i < size1; ++i )
	{
		tmp[ i ] = new T[ size2 ];
		memset( tmp[ i ], 0, size2*sizeof( T ) );
	}
	return tmp;
}

template <typename T>
void deleteMatrix( T**& matrix, UInt size )
{
	if( matrix == nullptr )
		return;

	for( UInt i = 0; i < size; ++i )
	{
		if( matrix[ i ] != nullptr )
			delete[] matrix[ i ];

		matrix[ i ] = nullptr;
	}
	delete[] matrix;
	matrix = nullptr;
}

Void loadNextMatrix( std::istream& input, Short**& destination, Int& size, Int& QP );
Void loadNextMatrix( std::istream& input, Short**& destination, UInt& bins, UInt size = 4 );

template <typename T> inline T clipToRange( T minVal, T maxVal, T a )
{
	return std::min<T>( std::max<T>( minVal, a ), maxVal );
}

Int log2Int( const Int );
Int numBits( const Int value );
UInt numBits( const UInt value );

UInt getZScanIdxIn4x4( const UInt puX, const UInt puY );

#endif