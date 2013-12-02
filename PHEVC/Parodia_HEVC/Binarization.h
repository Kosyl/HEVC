#ifndef BINARIZATION_H
#define BINARIZATION_H

#include "TypeDef.h"
#include "Logger.h"

class Binarization
{
private:
	static const Int COEFF_REMAIN_THRESHOLD;

public:
	Binarization();
	Binarization(std::string logPath, Bool isLogging);
	~Binarization();

	UInt binarizeCoefficientRemainingLevel_NumBits ( UInt symbol, UInt &rParam );

	Logger log;
};

#endif