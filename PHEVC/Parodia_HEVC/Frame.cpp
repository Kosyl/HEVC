#include "Frame.h"
#include "Logger.h"
#include "Utils.h"

Frame::Frame()
{
	initFrame(0,0,8);
}

Frame::Frame(unsigned int width, unsigned int height, unsigned short bitDepth)
{
	initFrame(width,height,bitDepth);
}

void Frame::initFrame(unsigned int width, unsigned int height, unsigned short bitDepth)
{
	bitDepth_=bitDepth;
	YSamples_=nullptr;
	CrSamples_=nullptr;
	CbSamples_=nullptr;
	YRecon_=nullptr;
	CrRecon_=nullptr;
	CbRecon_=nullptr;
	setSize(width,height);
}

Frame::~Frame()
{
	deleteMatrices();
}

void Frame::deleteMatrices()
{
	if(YSamples_)
	{
		for(UInt i=0;i<width_;++i)
		{
			delete[] YSamples_[i];
			delete[] YRecon_[i];
		}
		delete[] YSamples_; YSamples_=nullptr;
		delete[] YRecon_; YRecon_ = nullptr;

		UInt tmp=width_/2;
		for(UInt i=0;i<tmp;++i)
		{
			delete[] CrSamples_[i];
			delete[] CbSamples_[i];
			delete[] CrRecon_[i];
			delete[] CbRecon_[i];
		}
		delete[] CrSamples_; CrSamples_ = nullptr;
		delete[] CbSamples_; CbSamples_ = nullptr;
		delete[] CrRecon_; CrRecon_ = nullptr;
		delete[] CbRecon_; CbRecon_ = nullptr;
	}
}

void Frame::setSize(const UInt width, const UInt height)
{
	if(width==width_ && height==height_)return;

	deleteMatrices();

	if(width==0 || height==0) return;

	width_=width;
	height_=height;
	YSamples_=getEmptyMatrix<Int>(width,height);
	YRecon_=getEmptyMatrix<Int>(width,height);
	UInt ch = height/2;
	UInt cw = width/2;

	CbSamples_=getEmptyMatrix<Int>(cw,ch);
	CrSamples_=getEmptyMatrix<Int>(cw,ch);
	CbRecon_=getEmptyMatrix<Int>(cw,ch);
	CrRecon_=getEmptyMatrix<Int>(cw,ch);
}

void Frame::loadFrameFromYuv(std::ifstream& yuvFile)
{
	char tmp;

	for(UInt i=0;i<width_;++i)
	{
		for(UInt j=0;j<height_;++j)
		{
			yuvFile.read(&tmp,1);
			YSamples_[i][j]=tmp+128;
		}
	}

	UInt ch = height_/2;
	UInt cw = width_/2;

	for(UInt i=0;i<cw;++i)
	{
		for(UInt j=0;j<ch;++j)
		{
			yuvFile.read(&tmp,1);
			CbSamples_[i][j]=tmp+128;
		}
	}
	for(UInt i=0;i<cw;++i)
	{
		for(UInt j=0;j<ch;++j)
		{
			yuvFile.read(&tmp,1);
			CrSamples_[i][j]=tmp+128;
		}
	}
}

UInt Frame::getWidth()
{
	return width_;
}

UInt Frame::getHeight()
{
	return height_;
}

Int** Frame::samples(const ImgComp comp)
{
	if(comp == LUMA)
		return YSamples_;
	else if(comp== CR)
		return CrSamples_;
	else
		return CbSamples_;
}

Int** Frame::reconMatrix(const ImgComp comp)
{
	if(comp == LUMA)
		return YRecon_;
	else if(comp== CR)
		return CrRecon_;
	else
		return CbRecon_;
}
