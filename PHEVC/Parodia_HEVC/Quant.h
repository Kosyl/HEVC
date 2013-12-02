/** \file     Quant.h
    \brief    Klasa kwantyzatora (header)
*/
#ifndef _QUANT_
#define _QUANT_

#include "TypeDef.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

class Quant
{
public:
	static Quant *getInstance();
	~Quant();

	void quant(Short** const coeff, Short** result, const Int& QP, const Int& bitDepth, const Int& totalSize, const Int& trSize);
	void deQuant(Short** const input, Short** block, const Int& QP, const Int& bitDepth, const Int& totalSize, const Int& trSize);
	void quant(Short** const coeff, Short** result, const Int& QP, const Int& bitDepth, const Int& trSize);
	void deQuant(Short** const input, Short** block, const Int& QP, const Int& bitDepth, const Int& trSize);

	void initLog(string path, bool isLogging);
	Logger log;
private:
	Quant();
	static Quant *instance;
	static const Short forfardQuantCoeffs[6];///< tablica do wyliczania Q w kwantyzacji
	static const Short inverseQuantCoeffs[6];///< tablica do wyliczania IQ w dekwantyzacji
};

#endif