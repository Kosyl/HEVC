/** \file     Quant.h
    \brief    Klasa kwantyzatora (header)
*/
#ifndef _FRAME_
#define _FRAME_

#include "TypeDef.h"
#include "Logger.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <string.h>

class Frame
{
public:
	Frame();
	Frame(unsigned int width, unsigned int height, unsigned short bitDepth);
	void initFrame(unsigned int width, unsigned int height, unsigned short bitDepth);
	~Frame();

	void loadFrameFromYuv(std::ifstream& yuvStream);
	void setSize(const unsigned int width, const unsigned int height);
	UInt getWidth();
	UInt getHeight();
	Int** samples(const ImgComp comp);
	Int** reconMatrix(const ImgComp comp);
	void deleteMatrices();

private:
	unsigned int bitDepth_;
	unsigned int width_;
	unsigned int height_;
	Int** YSamples_;
	Int** CbSamples_;
	Int** CrSamples_;

	Int** YRecon_;
	Int** CbRecon_;
	Int** CrRecon_;
};

#endif