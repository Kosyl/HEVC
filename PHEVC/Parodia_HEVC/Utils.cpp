#include "Utils.h"
#include <cassert>

Void loadNextMatrix( std::istream& input, Short**& destination, Int& size, Int& QP )
{
	std::string s;
	input >> s;
	while( s[ 0 ] == '+' )
	{
		std::getline( input, s );
	}

	while( s[ 0 ] != '*' )
	{
		input >> s;
	}

	do
	{
		input >> s;
	}
	while( !isdigit( s[ 0 ] ) );
	//s==MxM
	std::string sizestr;
	if( s[ 0 ] == '1' || s[ 0 ] == '3' )
	{
		sizestr = s.substr( 0, 2 );
	}
	else
	{
		sizestr = s.substr( 0, 1 );
	}
	std::istringstream isstr( sizestr );
	isstr >> size;

	input >> s;//*** albo QP
	if( s == "QP" )
	{
		input >> s;//=
		input >> QP;
		input >> s;//***
	}

	destination = new Short*[ size ];
	for( int i = 0; i < size; ++i )destination[ i ] = new Short[ size ];
	for( int i = 0; i < size; ++i )
	{
		for( int j = 0; j < size; ++j )
		{
			input >> destination[ i ][ j ];
		}
	}
}

Void loadNextMatrix( std::istream& input, Short**& destination, UInt& bins, UInt size )
{
	std::string s;
	input >> s;
	while( s[ 0 ] == '+' )
	{
		std::getline( input, s );
	}
/*
	do
	{
		input >> s;
	}
	while( !isdigit( s[ 0 ] ) && !( s[ 0 ] == '-' &&  isdigit( s[ 1 ] ) ) );
	*/
	destination = new Short*[ size ];
	for( UInt i = 0; i < size; ++i )destination[ i ] = new Short[ size ];
	for( UInt i = 0; i < size; ++i )
	{
		for( UInt j = 0; j<size; ++j )
		{
			input >> destination[ j ][ i ];
		}
	}
	std::getline( input, s );
	do
	{
		std::getline( input, s );
	}
	while( s[ 0 ] == '+' );

	bins = std::stoi(s);
}

int log2Int( const int value )
{
	assert( value > 0 );

	int log2 = 0, val = value;
	for( ; val != 1; val >>= 1, log2++ );
	return log2;
}

Int numBits( const int value )
{
	return log2Int( value ) + 1;
}

UInt numBits( const UInt value )
{
	return numBits( (Int)value );
}

QTComponent& operator++( QTComponent& orig )
{
	orig = static_cast<QTComponent>( orig + 1 );
	return orig;
}