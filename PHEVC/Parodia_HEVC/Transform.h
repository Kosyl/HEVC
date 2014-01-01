/** \file     Transform.h
    \brief    Klasa transformaty (header)
*/

#ifndef _TRANSFORM_
#define _TRANSFORM_

#include "TypeDef.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

class Transform
{
public:
	static Transform* getInstance();
	~Transform();

	void forwardTrans(Sample** const block, Coeff** coeff, const Int& trSize, const Int& bitDepth, const Bool& isIntra4x4LumaDST_hor, const Bool& isIntra4x4LumaDST_ver);
	void inverseTrans( Coeff** const coeff, Sample** block, const Int& trSize, const Int& bitDepth, const Bool& isIntra4x4LumaDST_hor, const Bool& isIntra4x4LumaDST_ver );
	void forwardTrans( Sample** const block, Coeff** coeff, const Int& totalSize, const Int& trSize, const Int& bitDepth, const Int& offset_hor, const Int& offset_ver, const Bool& isIntra4x4LumaDST_hor, const Bool& isIntra4x4LumaDST_ver );
	void inverseTrans( Coeff** const coeff, Sample** block, const Int& totalSize, const Int& trSize, const Int& bitDepth, const Int& offset_hor, const Int& offset_ver, const Bool& isIntra4x4LumaDST_hor, const Bool& isIntra4x4LumaDST_ver );

	Int** afterHor; ///< tablica tymczasowa przechowujaca wynik transformaty poziomej do wgladu

	Logger log;
	void initLog(string path, bool isLogging);

private:
	static Transform* instance;
	Transform();
	static const Short TCOEFF_4[4][4];///< tablica wspolczynnikow DCT 4x4
	static const Short TCOEFF_8[8][8];///< tablica wspolczynnikow DCT 8x8
	static const Short TCOEFF_16[16][16];///< tablica wspolczynnikow DCT 16x16
	static const Short TCOEFF_32[32][32];///< tablica wspolczynnikow DCT 32x32
	static const Short TCOEFF_DST_4[4][4];///< tablica wspolczynnikow DST 4x4

	static const Short SHIFT_INV_1ST;///< przesuniecie po poziomej transformacie odwrotnej
	static const Short SHIFT_INV_2ND;///< przesuniecie po pionowej transformacie odwrotnej
};

#endif