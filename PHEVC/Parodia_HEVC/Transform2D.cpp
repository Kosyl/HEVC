/* The copyright in this software is being made available under the BSD
* License, included below. This software may be subject to other third party
* and contributor rights, including patent rights, and no such rights are
* granted under this license.  
*
* Copyright (c) 2010-2013, ITU/ISO/IEC
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  * Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

/** \file     Transform2D.cpp
    \brief    Funkcje transformacji prostej i odwrotnej na dwuwymiarowych tablicach
*/

#include "Transform2D.h"
#include "Constants.h"
#include "Utils.h"
#include <assert.h>
#include <iostream>

using namespace std;

/** Transformata prosta przez mnozenie macierzy.
*	\param block Wskaznik na tablice probek trSize*trSize (wartosci wejsciowe)
*	\param coeff Wskaznik na tablice wartosci wyjsciowych trSize*trSize
*	\param trSize Rozmiar bloku transformaty (4,8,16 lub 32)
*	\param bitDepth Glebia bitowa probek (max 16)
*	\param isIntra4x4LumaDST "True" jesli chcemy DST dla 4x4 blokow luminancji w trybie intra; "false" wpp.
*/
void xTr2D(short** block, short** coeff, int trSize, int bitDepth, bool isIntra4x4LumaDST)
{
	int i,j,k,iSum;
	int tmp[32][32];

	/*! \var short *iT
		\brief Wskaznik na odpowiednia macierz wspolczynnikow transformaty, wybierany na podstawie rozmiaru i trybu
	*/
	const short *iT;
	unsigned int log2TrSize ;

	if (trSize==4)
	{
		log2TrSize = 2;
		if (isIntra4x4LumaDST)//DST tylko dla 4x4
		{
			iT  =  g_as_DST_MAT_4[0];
		}
		else
		{
			iT  = g_aiT4[0];
		}
	}
	else if (trSize==8)
	{
		log2TrSize = 3;
		iT = g_aiT8[0];
	}
	else if (trSize==16)
	{
		log2TrSize = 4;
		iT = g_aiT16[0];
	}
	else if (trSize==32)
	{
		log2TrSize = 5;
		iT = g_aiT32[0];
	}
	else
	{
		assert(0);
	}

	//przesuniecia i poprawki wynikow po obu fazach
	int shift_1st = log2TrSize - 1 + bitDepth-8;
	int add_1st = 1<<(shift_1st-1);
	int shift_2nd = log2TrSize + 6;
	int add_2nd = 1<<(shift_2nd-1);

	/* Transformata pozioma */
	for (i=0; i<trSize; i++)
	{
		for (j=0; j<trSize; j++)
		{
			iSum = 0;
			for (k=0; k<trSize; k++)
			{
				iSum += iT[i*trSize+k]*block[j][k];
			}
			tmp[i][j] = (iSum + add_1st)>>shift_1st;
		}
	}

	/* Transformata pionowa */
	for (i=0; i<trSize; i++)
	{                 
		for (j=0; j<trSize; j++)
		{
			iSum = 0;
			for (k=0; k<trSize; k++)
			{
				iSum += iT[i*trSize+k]*tmp[j][k];        
			}
			coeff[i][j] = (iSum + add_2nd)>>shift_2nd; 
		}
	}
}

/** Transformata odwrotna przez mnozenie macierzy.
*	\param coeff Wskaznik na tablice wartosci wejsciowych (trSize*trSize)
*	\param block Wskaznik na tablice probek trSize*trSize (wartosci wyjsciowe)
*	\param trSize Rozmiar bloku transformaty (4,8,16 lub 32)
*	\param bitDepth Glebia bitowa probek (max 16)
*	\param isIntra4x4LumaDST "True" jesli coeff to DST dla 4x4 blokow luminancji w trybie intra; "false" wpp.
*/
void xITr2D(short **coeff, short **block, int trSize, int bitDepth, bool isIntra4x4LumaDST)
{
	int i,j,k;
	int iSum;
	int tmp[32][32];

	/*! \var short *iT
		\brief Wskaznik na odpowiednia macierz wspolczynnikow transformaty, wybierany na podstawie rozmiaru i trybu
	*/
	const short *iT;

	if (trSize==4)
	{
		if (isIntra4x4LumaDST) // DCT czy DST
		{
			iT  =  g_as_DST_MAT_4[0];
		}
		else
		{
			iT  = g_aiT4[0];
		}
	}
	else if (trSize==8)
	{
		iT = g_aiT8[0];
	}
	else if (trSize==16)
	{
		iT = g_aiT16[0];
	}
	else if (trSize==32)
	{
		iT = g_aiT32[0];
	}
	else
	{
		assert(0);
	}

	//przesuniecia i poprawki po obu fazach transformacji
	int shift_1st = SHIFT_INV_1ST;
	int add_1st = 1<<(shift_1st-1);
	int shift_2nd = SHIFT_INV_2ND - (bitDepth-8); //bylo g_bitDepth, nie wiem skad
	int add_2nd = 1<<(shift_2nd-1);

	/* Transformacja pozioma */
	for (i=0; i<trSize; i++)
	{    
		for (j=0; j<trSize; j++)
		{
			iSum = 0;
			for (k=0; k<trSize; k++)
			{        
				iSum += iT[k*trSize+i]*coeff[k][j]; 
			}
			tmp[i][j] = Clip3(-32768, 32767, (iSum + add_1st)>>shift_1st);
		}
	}   

	/* Transformacja pionowa */
	for (i=0; i<trSize; i++)
	{   
		for (j=0; j<trSize; j++)
		{
			iSum = 0;
			for (k=0; k<trSize; k++)
			{        
				iSum += iT[k*trSize+j]*tmp[i][k];
			}
			block[i][j] = Clip3(-32768, 32767, (iSum + add_2nd)>>shift_2nd);
		}
	}
}
