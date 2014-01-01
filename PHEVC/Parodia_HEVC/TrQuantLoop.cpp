/** \file     TrQuantLoop.cpp
\brief    Klasa obliczajaca blok po przejsciu przez transformate prosta i odwrotna (z kwantyzacja)
*/

#include "TrQuantLoop.h"

using namespace std;

/** Konstruktor domyslny, wylaczone logowanie
*
*/
TrQuantLoop::TrQuantLoop():
	log()
{
	nullMatrices();
}

/** Konstruktor przyjmuje sciezke do logu i rozkaz wlaczenia/nie wlaczania logu
*	\param path sciezka do pliku logu
\param isLogging pisze log, jesli true
*/
TrQuantLoop::TrQuantLoop(string path, bool isLogging):
	log(path,isLogging)
{
	nullMatrices();
}

TrQuantLoop::TrQuantLoop(string path, bool isLogging, Transform* tr, Quant* q):
	log(path,isLogging),
	t(tr),
	q(q)
{
	nullMatrices();
}

TrQuantLoop::~TrQuantLoop()
{
	deleteMatrices(prevBlockSize);
}

void TrQuantLoop::nullMatrices()
{
	delta=NULL;
	horTrans=NULL;
	vertTrans=NULL;
	quant=NULL;
	dequant=NULL;
	invVertTrans=NULL;
	invHorTrans=NULL;
}
void TrQuantLoop::deleteMatrices(int size)
{
	if(delta)//przykladowa, wszystkie leca rownolegle
	{
		for(int i=0;i<size;++i)
		{
			delete[]delta[i];
			delete[]horTrans[i];
			delete[]vertTrans[i];
			delete[]quant[i];
			delete[]dequant[i];
			delete[]invVertTrans[i];
			delete[]invHorTrans[i];
		}
		delete[]delta;
		delete[]horTrans;
		delete[]vertTrans;
		delete[]quant;
		delete[]dequant;
		delete[]invVertTrans;
		delete[]invHorTrans;
	}
}

void TrQuantLoop::allocateMatrices(int blockSize)
{
	delta=new Sample*[blockSize];
	horTrans=new Coeff*[blockSize];
	vertTrans = new Coeff*[ blockSize ];
	quant = new Coeff*[ blockSize ];
	dequant = new Coeff*[ blockSize ];
	invVertTrans = new Coeff*[ blockSize ];
	invHorTrans = new Sample*[ blockSize ];

	for(int i=0;i<blockSize;++i)
	{
		delta[ i ] = new Sample[ blockSize ];
		horTrans[ i ] = new Coeff[ blockSize ];
		vertTrans[ i ] = new Coeff[ blockSize ];
		quant[ i ] = new Coeff[ blockSize ];
		dequant[ i ] = new Coeff[ blockSize ];
		invVertTrans[ i ] = new Coeff[ blockSize ];
		invHorTrans[ i ] = new Sample[ blockSize ];
	}
}

void TrQuantLoop::TrQuantDequantItr( Sample** const block, Sample** residuals, Sample** result, const Int& QP, const Int& bitDepth, const Int& blockSize, const Bool& isIntra4x4LumaDST_hor, const Bool& isIntra4x4LumaDST_ver )
{
	deleteMatrices(prevBlockSize);
	prevBlockSize=blockSize;
	allocateMatrices(blockSize);

	t->forwardTrans(residuals,vertTrans,blockSize,bitDepth,isIntra4x4LumaDST_hor,isIntra4x4LumaDST_ver);
	for(Int i=0;i<blockSize;++i)
	{
		for(int j=0;j<blockSize;++j)
		{
			horTrans[i][j]=t->afterHor[i][j];
		}
	}
	q->quant(vertTrans,quant,QP,bitDepth,blockSize);

	q->deQuant(quant,dequant,QP,bitDepth,blockSize);

	t->inverseTrans(dequant,invHorTrans,blockSize,bitDepth,isIntra4x4LumaDST_hor,isIntra4x4LumaDST_ver);
	for(Int i=0;i<blockSize;++i)
	{
		for(int j=0;j<blockSize;++j)
		{
			invVertTrans[i][j]=t->afterHor[i][j];
		}
	}

	Short maxVal = 1<<bitDepth;
	for(Int i=0;i<blockSize;++i)
	{
		for(int j=0;j<blockSize;++j)
		{
			delta[i][j]=residuals[i][j]-invHorTrans[i][j];
			Short sample = block[i][j]+delta[i][j];
			result[i][j]=std::min<Short> (std::max<Short> (0, sample) , maxVal);
		}
	}
	if(log.isLogging())
	{
		log << "*** input block "<< blockSize << "x" << blockSize << " QP=" << QP << " ***\n";
		for(Int i=0;i<blockSize;++i) 
		{
			for(Int j=0;j<blockSize;++j)
			{
				log << block[i][j] << ((j==blockSize-1)?"":" ");
			}
			log << "\n";
		}
		log << "*** input residuals "<< blockSize << "x" << blockSize << " QP=" << QP << " ***\n";
		for(Int i=0;i<blockSize;++i) 
		{
			for(Int j=0;j<blockSize;++j)
			{
				log << residuals[i][j] << ((j==blockSize-1)?"":" ");
			}
			log << "\n";
		}
		log << "*** output residuals "<< blockSize << "x" << blockSize << " QP=" << QP << " ***\n";
		for(Int i=0;i<blockSize;++i) 
		{
			for(Int j=0;j<blockSize;++j)
			{
				log << invHorTrans[i][j] << ((j==blockSize-1)?"":" ");
			}
			log << "\n";
		}
		log << "*** output block "<< blockSize << "x" << blockSize << " QP=" << QP << " ***\n";
		for(Int i=0;i<blockSize;++i) 
		{
			for(Int j=0;j<blockSize;++j)
			{
				log << result[i][j] << ((j==blockSize-1)?"":" ");
			}
			log << "\n";
		}
	}
}