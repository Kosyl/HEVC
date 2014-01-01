/** \file     TrQuantLoop.h
    \brief    Klasa obliczajaca blok po przejsciu przez transformate prosta i odwrotna (z kwantyzacja) (header)
*/
#ifndef _TRQUANTLOOP_
#define _TRQUANTLOOP_

#include "TypeDef.h"
#include "Transform.h"
#include "Quant.h"
#include "Logger.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

class TrQuantLoop
{
public:
	TrQuantLoop();
	TrQuantLoop(string logPath, bool isLogging);
	TrQuantLoop(string path, bool isLogging, Transform* tr, Quant* q);
	~TrQuantLoop();

	void TrQuantDequantItr( Sample** const block, Sample** residuals, Sample** result, const Int& QP, const Int& bitDepth, const Int& blockSize, const Bool& isIntra4x4LumaDST_hor, const Bool& isIntra4x4LumaDST_ver );

	Sample** delta;
	Coeff** horTrans;
	Coeff** vertTrans;
	Coeff** quant;
	Coeff** dequant;
	Coeff** invVertTrans;
	Sample** invHorTrans;

	Logger log;

private:
	void deleteMatrices(int size);
	void allocateMatrices(int size);
	void nullMatrices();
	int prevBlockSize;
	Transform* t;
	Quant* q;
};

#endif