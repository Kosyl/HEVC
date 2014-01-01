#ifndef BINARIZATION_H
#define BINARIZATION_H

#include "TypeDef.h"
#include "Logger.h"
#include <bitset>

class ScanningMode
{
public:
	ScanningMode( );
	virtual ~ScanningMode( );

	virtual Short* scan( Short** coeffs4x4, UInt& out_lastCoeffX, UInt& out_lastCoeffY ) = 0;
};

class HorizontalScanningMode : public ScanningMode
{
public:
	HorizontalScanningMode( );
	~HorizontalScanningMode( );

	Short* scan( Short** coeffs4x4, UInt& out_lastCoeffX, UInt& out_lastCoeffY );
};

class VerticalScanningMode : public ScanningMode
{
public:
	VerticalScanningMode( );
	~VerticalScanningMode( );

	Short* scan( Short** coeffs4x4, UInt& out_lastCoeffX, UInt& out_lastCoeffY );
};

class DiagonalScanningMode : public ScanningMode
{
public:
	DiagonalScanningMode( );
	~DiagonalScanningMode( );

	const static UInt indexes[2][16];

	Short* scan( Short** coeffs4x4, UInt& out_lastCoeffX, UInt& out_lastCoeffY );
};

class Binarization
{
private:
	static const Int COEFF_REMAIN_THRESHOLD;

public:
	Binarization( );
	Binarization( std::string logPath, Bool isLogging );
	~Binarization( );

	UInt countBinsIn4x4TU( Short** const coefficients, ScanningMode* scanningMode );
	UInt countNonZeroCoeffsSignsAndAbsolutes( Short* coeffs, UInt len, Short* out_lastCoeffPositionInScan, Short* out_absolutes, std::bitset<16>& out_signs );
	UInt calcC1andC2( Short* absolutes, Short* out_remains, UInt numNonZero );

	UInt binarizeCoefficientRemainingLevel_NumBits( UInt symbol, UInt &rParam );
	UInt binarizeTU_NumBits( UInt val, UInt maxLenBeforeSuffix, UInt riceParam );
	UInt binarizeEGk_NumBits( Int val, UInt k );
	UInt binarizeFL_NumBits( UInt val, UInt maxLenBeforeSuffix, UInt riceParam );

	Logger log;
};

#endif