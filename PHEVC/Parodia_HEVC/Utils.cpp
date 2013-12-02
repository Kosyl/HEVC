#include "Utils.h"
#include <cassert>

SeqParams* SeqParams::instance = nullptr;

SeqParams::SeqParams()
{
  picWidth = picHeight = bitDepthLuma = bitDepthChroma = maxCuSize = -1;
  smoothEn = false;
}
 
SeqParams::~SeqParams()
{
}

SeqParams* SeqParams::getInstance()
{
  if (instance == nullptr)
    instance = new SeqParams();
  return instance;
}

void SeqParams::setPicWidth(const int newPicWidth)
{
  picWidth = newPicWidth;
}

void SeqParams::setPicHeight(const int newPicHeight)
{
  picHeight = newPicHeight;
}

void SeqParams::setBitDepthLuma(const int newBitDepthLuma)
{
  bitDepthLuma = newBitDepthLuma;
}

void SeqParams::setBitDepthChroma(const int newBitDepthChroma)
{
  bitDepthChroma = newBitDepthChroma;
}

void SeqParams::setMaxCuSize(const int newMaxCuSize)
{
  maxCuSize = newMaxCuSize;
}

void SeqParams::setSmoothEn(const bool newSmoothEn)
{
  smoothEn = newSmoothEn;
}

int SeqParams::getPicWidth() const
{
  return picWidth;
}

int SeqParams::getPicHeight() const
{
  return picHeight;
}

int SeqParams::getBitDepthLuma() const
{
  return bitDepthLuma;
}

int SeqParams::getBitDepthChroma() const
{
  return bitDepthChroma;
}

int SeqParams::getMaxCuSize() const
{
  return maxCuSize;
}

bool SeqParams::getSmoothEn() const
{
  return smoothEn;
}

int SeqParams::clip(ImgComp comp, int ref)
{
  int maxVal = comp == LUMA ? 1 << bitDepthLuma : 1 << bitDepthChroma;
  if (ref < 0)
    return 0;
  else if (ref > maxVal)
    return maxVal;
  else
    return ref;
}

int SeqParams::getDefVal(ImgComp comp)
{
  if (comp == LUMA)
    return 1 << (bitDepthLuma - 1);
  else
    return 1 << (bitDepthChroma - 1);
}

Void loadNextMatrix(istream& input, Short**& destination, Int& size, Int& QP)
{
	string s;
	input >> s;
	while(s[0]=='+')
	{
		getline(input,s);
	}

	while(s[0]!='*')
	{
		input >> s;
	}

	do
	{
		input >> s;
	}
	while(!isdigit(s[0]));
	//s==MxM
	string sizestr;
	if(s[0]=='1' || s[0]=='3')
	{
		sizestr=s.substr(0,2);
	}
	else
	{
		sizestr=s.substr(0,1);
	}
	istringstream isstr(sizestr);
	isstr >> size;

	input >> s;//*** albo QP
	if(s=="QP")
	{
		input >> s;//=
		input >> QP;
		input >> s;//***
	}

	destination=new Short*[size];
	for(int i=0;i<size;++i)destination[i]=new Short[size];
	for(int i=0;i<size;++i)
	{
		for(int j=0;j<size;++j)
		{
			input >> destination[i][j];
		}
	}
}

int log2(const int value)
{
  assert(value > 0);

  int log2 = 0, val = value;
  for (; val != 1; val >>= 1, log2++);
  return log2;
}

