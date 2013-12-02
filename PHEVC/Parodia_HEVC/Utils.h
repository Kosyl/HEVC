#ifndef _UTILS_
#define _UTILS_

#include <algorithm>
#include "TypeDef.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> 
#include <sstream>

using namespace std;

/** \file     Utils.h
\brief    Proste funkcje uzywane w roznych miejscach (header)
*/

enum ImgComp {LUMA=0, CB=1, CR=2};

class SeqParams
{
private:
	static SeqParams *instance;
	int picWidth, picHeight, bitDepthLuma, bitDepthChroma, maxCuSize;
	bool smoothEn;
	SeqParams();
public:
	static SeqParams* getInstance();
	~SeqParams();
	void setPicWidth(const int w);
	void setPicHeight(const int h);
	void setBitDepthLuma(const int bd);
	void setBitDepthChroma(const int bd);
	void setMaxCuSize(const int size);
	void setSmoothEn(const bool smooth);
	int getPicWidth() const;
	int getPicHeight() const;
	int getBitDepthLuma() const;
	int getBitDepthChroma() const;
	int getMaxCuSize() const;
	bool getSmoothEn() const;
	int clip(ImgComp comp, int value);
	int getDefVal(ImgComp comp);
};

template <typename T>
Void printMatrix(T** matrix, Int size, ostream& out=cout)
{
	for(int i=0;i<size;++i)
	{
		for(int j=0;j<size;++j)
		{
			out << std::setw(4) << matrix[j][i] << " ";
		}
		out << endl;
	}
}

template <typename T>
T** getEmptyMatrix(unsigned int size1, unsigned int size2)
{
	T** tmp = new T*[size1];
	for(int i=0;i<size1;++i)
	{
		tmp[i]=new T[size2];
		memset(tmp[i],0,size2*sizeof(T));
	}
	return tmp;
}

Void loadNextMatrix(istream& input, Short**& destination, Int& size, Int& QP);

enum Direction {LEFT_DIR = 0, TOP_DIR, CORNER_DIR};
enum ModeType {PLANAR = 0, DC, LINEAR, ANGULAR};

int log2(const int);

#endif