/** \file     Transform2D.h
    \brief    Funkcje transformacji prostej i odwrotnej na dwuwymiarowych tablicach (header)
*/

#ifndef _TRANSFORM2D_
#define _TRANSFORM2D_

void xTr2D(short** block, short** coeff, int uiTrSize, int bitDepth, bool uiMode);
void xITr2D(short **coeff, short **block, int uiTrSize, int bitDepth, bool uiMode);

#endif