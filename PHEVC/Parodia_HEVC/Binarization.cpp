#include "Binarization.h"
#include "Utils.h"
#include <bitset>
#include <fstream>
#include <stdio.h>
#include <boost\dynamic_bitset.hpp>

#pragma region Scanning Modes

ScanningMode::ScanningMode( )
{
}

ScanningMode::~ScanningMode( )
{
}

HorizontalScanningMode::HorizontalScanningMode( )
{
}

HorizontalScanningMode::~HorizontalScanningMode( )
{
}

Short* HorizontalScanningMode::scan( Short** coeffs4x4, UInt& out_lastCoeffX, UInt& out_lastCoeffY )
{
	Short* res = new Short[ 16 ];
	Short num = 0;
	bool pastFirstNonZero = false;

	for( Short i = 3; i >= 0; --i )
	{
		for( Short j = 3; j >= 0; --j )
		{
			res[ num ] = coeffs4x4[ j ][ i ];
			if( !pastFirstNonZero && res[ num ] != 0 )
			{
				pastFirstNonZero = true;
				out_lastCoeffX = j;
				out_lastCoeffY = i;
			}
			++num;
		}
	}
	return res;
}

VerticalScanningMode::VerticalScanningMode( )
{
}

VerticalScanningMode::~VerticalScanningMode( )
{
}

Short* VerticalScanningMode::scan( Short** coeffs4x4, UInt& out_lastCoeffX, UInt& out_lastCoeffY )
{
	Short* res = new Short[ 16 ];
	Short num = 0;
	bool pastFirstNonZero = false;

	for( Short i = 3; i >= 0; --i )
	{
		for( Short j = 3; j >= 0; --j )
		{
			res[ num ] = coeffs4x4[ i ][ j ];
			if( !pastFirstNonZero && res[ num ] != 0 )
			{
				pastFirstNonZero = true;
				out_lastCoeffX = i;
				out_lastCoeffY = j;
			}
			++num;
		}
	}
	return res;
}

DiagonalScanningMode::DiagonalScanningMode( )
{
}

DiagonalScanningMode::~DiagonalScanningMode( )
{
}

const UInt DiagonalScanningMode::indexes[ 2 ][ 16 ] =
{
	{ 3, 3, 2, 3, 2, 1, 3, 2, 1, 0, 2, 1, 0, 1, 0, 0 },
	{ 3, 2, 3, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 0, 1, 0 }
};

Short* DiagonalScanningMode::scan( Short** coeffs4x4, UInt& out_lastCoeffX, UInt& out_lastCoeffY )
{
	Short* res = new Short[ 16 ];
	UInt x = 0, y = 0;
	bool pastFirstNonZero = false;

	for( int i = 0; i < 16; ++i )
	{
		x = indexes[ 0 ][ i ];
		y = indexes[ 1 ][ i ];

		res[ i ] = coeffs4x4[ x ][ y ];
		if( !pastFirstNonZero && res[ i ] != 0 )
		{
			pastFirstNonZero = true;
			out_lastCoeffX = x;
			out_lastCoeffY = y;
		}
	}

	return res;
}

#pragma endregion

const Int Binarization::COEFF_REMAIN_THRESHOLD = 3; //przejœcie z GR na TU+EG(k)

Binarization::Binarization( ) :
log( )
{
}

Binarization::Binarization( std::string logPath, Bool isLogging ) :
log( logPath, isLogging )
{
}

Binarization::~Binarization( )
{
}

UInt Binarization::binarizeCoefficientRemainingLevel_NumBits( UInt symbol, UInt &rParam )
{
	Int value = (Int)symbol;
	Int threshold = 3 << rParam;

	if( value < threshold )
	{
		return ( value >> rParam ) + 1 + rParam;
	}
	value -= threshold;

	return ( ( ( numBits( value + ( 1 << rParam ) ) ) - 1 ) << 1 ) + 4 - rParam;

	/*Int codeNumber = (Int)symbol;
	UInt length;

	UInt prefixBins, prefixLength, suffixBins, suffixLength;

	log << "(" << symbol << "," << rParam << ")" << std::endl;
	if( codeNumber < ( COEFF_REMAIN_THRESHOLD << rParam ) )
	{
	log << "kodowanie GR, remain: " << codeNumber << " < " << ( COEFF_REMAIN_THRESHOLD << rParam ) << std::endl;
	++log;

	length = codeNumber >> rParam;

	prefixBins = ( 1 << ( length + 1 ) ) - 2;
	prefixLength = length + 1;
	suffixBins = codeNumber % ( 1 << rParam );
	suffixLength = rParam;

	std::bitset<( sizeof Int ) * 8> first( prefixBins );
	std::bitset<( sizeof Int ) * 8> second( suffixBins );
	for( int i = prefixLength - 1; i >= 0; --i )
	log << first[ i ];
	log << " ";
	for( int i = suffixLength - 1; i >= 0; --i )
	log << second[ i ];
	log << std::endl;
	--log;
	}
	else
	{
	log << "kodowanie TU+EG(k), remain: " << codeNumber << " >= " << ( COEFF_REMAIN_THRESHOLD << rParam ) << std::endl;
	++log;
	length = rParam;
	codeNumber = codeNumber - ( COEFF_REMAIN_THRESHOLD << rParam );

	log << "poprawka remain: " << codeNumber << std::endl;

	++log;
	while( codeNumber >= ( 1 << length ) )
	{
	log << "remain = " << codeNumber << " >= 1<<length = " << ( 1 << length ) << std::endl;
	codeNumber -= ( 1 << ( length++ ) );
	log << "remain = " << codeNumber << ", length = " << length << std::endl;
	}
	--log;

	prefixBins = ( 1 << ( COEFF_REMAIN_THRESHOLD + length + 1 - rParam ) ) - 2;
	prefixLength = COEFF_REMAIN_THRESHOLD + length + 1 - rParam;
	suffixBins = codeNumber;
	suffixLength = length;

	std::bitset<( sizeof Int ) * 8> first( prefixBins );
	std::bitset<( sizeof Int ) * 8> second( suffixBins );
	for( int i = prefixLength - 1; i >= 0; --i )
	log << first[ i ];
	log << " ";
	for( int i = suffixLength - 1; i >= 0; --i )
	log << second[ i ];
	log << std::endl;
	--log;
	}
	log << "(" << symbol << "," << rParam << ") -> " << ( prefixLength + suffixLength ) << " bitow" << std::endl;
	return prefixLength + suffixLength;*/
}

UInt Binarization::countBinsIn4x4TU( Short** const coefficients, ScanningMode* scanningMode )
{
	// TODO flaga transformSkip
	UInt totalBins = 0;
	UInt log2TUSize = 2;

	UInt lastCoeffX = 0, lastCoeffY = 0, lastCoeffInScan = 0;
	Short* absolutes = new Short[ 16 ];
	Short* remains = new Short[ 16 ];
	Short* nonZeroIndexesInScan = new Short[ 16 ];

	std::bitset<16> signs;

	Short* values = scanningMode->scan( coefficients, lastCoeffX, lastCoeffY );

	if( lastCoeffX == 0 && lastCoeffY == 0 && values[ 0 ] == 0 )
		return 1;

	totalBins += lastCoeffX == 3 ? 3 : lastCoeffX + 1; // zliczanie lastCoeffX
	totalBins += lastCoeffY == 3 ? 3 : lastCoeffY + 1; // zliczanie lastCoeffY

#pragma region Log

	std::cout << "*************" << std::endl << "wspolczynniki:" << std::endl;
	printMatrix( coefficients, 4 );
	std::cout << std::endl << "po skanowaniu:" << std::endl;
	printTable( values, 16 );
	std::cout << std::endl;
	std::cout << "lastX:" << lastCoeffX << std::endl;
	std::cout << "lastY:" << lastCoeffY << std::endl;
	std::cout << "totalBins: " << totalBins << std::endl;

	log << "macierz: ";
	printTable( values, 16, log.getStream( ), 0 );

#pragma endregion

	UInt numNonZero = countNonZeroCoeffsSignsAndAbsolutes( values, 16, nonZeroIndexesInScan, absolutes, signs );
	lastCoeffInScan = nonZeroIndexesInScan[ 0 ];

	totalBins += 16 - lastCoeffInScan - 1; // zliczanie mapy znaczonoœci, bez LastCoeff
	totalBins += numNonZero; // zliczanie bitów znaków

	for( UInt i = 0; i < numNonZero; ++i ) // wiemy ze sa >=1, wiec kodowac bedziemy x-1
	{
		remains[ i ] = absolutes[ i ] - 1;
	}

#pragma region Log

	std::cout << "lastCoeffInScan:" << lastCoeffInScan << std::endl;
	std::cout << "mapa znaczonosci:" << 16 - lastCoeffInScan - 1 << std::endl;
	std::cout << "znaki:" << numNonZero << std::endl;
	std::cout << "totalBins: " << totalBins << std::endl;
	std::cout << std::endl << "znaki:" << std::endl;
	std::cout << signs;
	std::cout << std::endl << "remains:" << std::endl;
	printTable( remains, numNonZero );
	std::cout << std::endl << "absolutes:" << std::endl;
	printTable( absolutes, numNonZero );

#pragma endregion

	UInt C1andC2bins = calcC1andC2( absolutes, remains, numNonZero );

	totalBins += C1andC2bins; // biny flag C1 i C2

	UInt riceParam = 0;

#pragma region Log

	std::cout << "C1andC2bins:" << C1andC2bins << std::endl;
	std::cout << "totalBins: " << totalBins << std::endl;
	std::cout << std::endl << "remains:" << std::endl;
	printTable( remains, numNonZero );
	log << "przed ostatnia faza = " << totalBins << std::endl;

#pragma endregion

	for( UInt i = 0; i < numNonZero; ++i )
	{
		if( remains[ i ] >= 0 )
		{
			UInt size = binarizeCoefficientRemainingLevel_NumBits( remains[ i ], riceParam );
			totalBins += size;

#pragma region Log

			std::cout << "bity symbolu[ " << i << " ] (" << remains[ i ] << "," << riceParam << "): " << size << std::endl;
			log << "funkcja_" << nonZeroIndexesInScan[ i ] + 1 << "_obrot (val=" << remains[i] << ", rice=" << riceParam << ") = " << totalBins << std::endl;

#pragma endregion

			if( absolutes[ i ] > 3 * ( 1 << riceParam ) )
			{
				riceParam = std::min<UInt>( riceParam + 1, 4 );
			}
		}
	}

	delete[] absolutes;
	delete[] values;
	delete[] remains;

#pragma region Log

	std::cout << "Koniec! totalBins: " << totalBins << std::endl;
	log << "wynik koncowy = " << totalBins << std::endl << "****************" << std::endl;

#pragma endregion

	return totalBins;
}

UInt Binarization::countNonZeroCoeffsSignsAndAbsolutes( Short* coeffs, UInt len, Short* out_significantIndexesInScan, Short* out_absolutes, std::bitset<16>& out_signs )
{
	UInt count = 0;

	for( UInt i = 0; i < len; ++i )
	{
		if( coeffs[ i ] != 0 )
		{
			out_absolutes[ count ] = abs( coeffs[ i ] );
			out_signs[ count ] = coeffs[ i ]>0 ? 1 : 0;
			out_significantIndexesInScan[ count ] = i;
			++count;
		}
	}

	return count;
}

UInt Binarization::binarizeTU_NumBits( UInt val, UInt maxLenBeforeSuffix, UInt riceParam )
{
	UInt len = 0;
	UInt prefix = val >> riceParam;
	UInt threshold = maxLenBeforeSuffix >> riceParam;

	len = prefix + 1;

	if( maxLenBeforeSuffix > val )
	{
		UInt suffix = val - ( prefix << riceParam );
		len += numBits( suffix );
	}
	return len;
}

UInt Binarization::binarizeEGk_NumBits( Int val, UInt k )
{
	UInt len = 0;
	Int absloute = abs( val );
	Bool stopLoop = false;
	do
	{
		if( absloute >= ( 1 << k ) )
		{
			++len;
			absloute = absloute - ( 1 << k );
			k++;
		}
		else
		{
			++len;
			while( k-- )
				++len;
			stopLoop = true;
		}
	}
	while( !stopLoop );

	return len;
}

UInt Binarization::binarizeFL_NumBits( UInt val, UInt maxLenBeforeSuffix, UInt riceParam )
{
	return 0;
}

UInt Binarization::calcC1andC2( Short* absolutes, Short* out_remains, UInt numNonZero )
{
	UInt result = 0;
	UInt C1count = 0;
	UInt C2count = 0;

	for( UInt i = 0; i < numNonZero; ++i )
	{
		if( absolutes[ i ] > 0 )
		{
			++C1count;
			out_remains[ i ] = out_remains[ i ] - 1;
			if( C2count == 0 && absolutes[ i ] > 1 )
			{
				C2count = 1;
				out_remains[ i ] = out_remains[ i ] - 1;
			}
		}
		if( C1count == 8 ) break;
	}

	return C1count + C2count;
}