/** \file     Main.cpp
\brief    Test funkcji transformacji i kwantyzacji
*/

#ifndef _MAIN_
#define _MAIN_

#include "Quant.h"
#include "Transform.h"
#include "TypeDef.h"
#include "TrQuantLoop.h"
#include "Utils.h"
#include "Picture.h"
#include "Binarization.h"
#include "PartitioningStrategy.h"
#include "PU.h"
#include "CU.h"

#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <string>
#include <iomanip> 

using namespace std;

/**
*	Funkcja liczy transformate przez mnozenie macierzy dla tablic 2D, (ew. DST)
*	\param tab wskaznik na dwuwymiarowa tablice probek o wymiarach bok x bok
*	\param bok wymiar bloku transformaty (bok x bok);
*	\param tr obiekt liczacy transformate prosta i odwrotna
*	\param q obiekt liczacy kwantyzacje i dekwwantyzacje
*	\param qp parametr kwantyzacji
*/
void test( Short** tab, Int bok, Transform& tr, Quant& q, Int qp )
{
	Coeff** resNxN = new Coeff*[ bok ]; // tablica na wynik transformacji prostej
	for( Int i = 0; i < bok; ++i )
	{
		resNxN[ i ] = new Coeff[ bok ]; memset( resNxN[ i ], 0, bok*sizeof( Short ) );
	}

	Coeff** quantresNxN = new Coeff*[ bok ]; // tablica na wynik kwantyzacji
	for( Int i = 0; i < bok; ++i )
	{
		quantresNxN[ i ] = new Coeff[ bok ]; memset( quantresNxN[ i ], 0, bok*sizeof( Short ) );
	}

	Coeff** dequantresNxN = new Coeff*[ bok ]; // tablica na wynik dekwantyzacji
	for( Int i = 0; i < bok; ++i )
	{
		dequantresNxN[ i ] = new Coeff[ bok ]; memset( dequantresNxN[ i ], 0, bok*sizeof( Short ) );
	}

	Sample** invresNxN = new Sample*[ bok ]; // tablica na wynik transformacji odwrotnej
	for( Int i = 0; i < bok; ++i )invresNxN[ i ] = new Sample[ bok ];

	//policzenie wynikow, jesli 'tr' lub 'q' maja wlaczone logowanie, takze ono nastepuje
	tr.forwardTrans( tab, resNxN, bok, 8, true, true );
	q.quant( resNxN, quantresNxN, qp, 8, bok, bok );
	q.deQuant( quantresNxN, dequantresNxN, qp, 8, bok, bok );
	tr.inverseTrans( dequantresNxN, invresNxN, bok, 8, true, true );

	for( int i = 0; i < bok; ++i )
	{
		delete[] resNxN[ i ];
		delete[] invresNxN[ i ];
	}
	delete[] resNxN;
	delete[] invresNxN;
}
//
/**
*	Funkcja liczy transformate przez mnozenie macierzy dla tablic 2D (ew. DST), dla bloku MxM liczy transformaty NxN, gdzie M=k*N
*	\param tab wskaznik na dwuwymiarowa tablice probek o wymiarach totalSize x totalSize
*	\param totalSize calkowity wymiar bloku probek
*	\param trSize wymiar transformaty (totalSize = k*trSize)
*	\param tr obiekt liczacy transformate prosta i odwrotna
*	\param q obiekt liczacy kwantyzacje i dekwwantyzacje
*	\param qp parametr kwantyzacji
*/
void splittest( Short** tab, Int totalSize, Int trSize, Transform& tr, Quant& q, Int qp )
{
	Coeff** resNxN = new Coeff*[ totalSize ]; // tablica na wynik transformacji prostej
	for( Int i = 0; i < totalSize; ++i )
	{
		resNxN[ i ] = new Coeff[ totalSize ]; memset( resNxN[ i ], 0, totalSize*sizeof( Short ) );
	}

	Coeff** quantresNxN = new Coeff*[ totalSize ]; // tablica na wynik kwantyzacji
	for( Int i = 0; i < totalSize; ++i )
	{
		quantresNxN[ i ] = new Coeff[ totalSize ]; memset( quantresNxN[ i ], 0, totalSize*sizeof( Short ) );
	}

	Coeff** dequantresNxN = new Coeff*[ totalSize ]; // tablica na wynik dekwantyzacji
	for( Int i = 0; i < totalSize; ++i )
	{
		dequantresNxN[ i ] = new Coeff[ totalSize ]; memset( dequantresNxN[ i ], 0, totalSize*sizeof( Short ) );
	}

	Sample** invresNxN = new Sample*[ totalSize ]; // tablica na wynik transformacji odwrotnej
	for( Int i = 0; i < totalSize; ++i )invresNxN[ i ] = new Sample[ totalSize ];

	for( int i = 0; i <= totalSize - trSize; i += trSize )
	{
		for( int j = 0; j <= totalSize - trSize; j += trSize )
		{
			tr.forwardTrans( tab, resNxN, totalSize, trSize, 8, i, j, false, false );
		}
	}
	q.quant( resNxN, quantresNxN, qp, 8, totalSize, trSize );
	q.deQuant( quantresNxN, dequantresNxN, qp, 8, totalSize, trSize );
	for( int i = 0; i <= totalSize - trSize; i += trSize )
	{
		for( int j = 0; j <= totalSize - trSize; j += trSize )
		{
			tr.inverseTrans( dequantresNxN, invresNxN, totalSize, trSize, 8, i, j, false, false );
		}
	}

	for( int i = 0; i < totalSize; ++i )
	{

		delete[] resNxN[ i ];
		delete[] invresNxN[ i ];
	}
	delete[] resNxN;
	delete[] invresNxN;
}
//
void classicTest( )
{
	srand( (unsigned int)time( NULL ) );

	Int bok = 4; // rozmiar bloku
	Int realsize; // rozmiar transformaty (niekoniecznie == rozmiarowi bloku!)

	string s;
	ifstream input( "D:\\txt\\macierze4x4.txt" ); // strumien z macierzami wejsciowymi
	int count = 0;

	//konstruktorom przekazujemy sciezki plikow logow i wlaczamy logowanie
	Transform* t = Transform::getInstance( );
	t->initLog( "D:\\txt\\tlog_qp10.txt", true );
	Quant* q = Quant::getInstance( );
	q->initLog( "D:\\txt\\qlog_qp10.txt", true );

	//wylaczamy detale typu podawanie shiftow, offsetow itp
	q->log.includeDetails = false;
	t->log.includeDetails = false;

	Short** tab4; // tablica dla transformat 4x4 liczonych w blokach 8x8
	tab4 = new Short*[ 8 ];
	for( int i = 0; i < 8; ++i )tab4[ i ] = new Short[ 8 ];
	int ofs1 = 0, ofs2 = 0, ofsum = 0;

	Short** tab;
	input >> s;
	cout << s;
	while( s[ 0 ] == '*' )
	{
		count++;
		cout << count << endl;
		input >> realsize; // pierwszy wiersz po "***" to rozmiar macierzy
		bok = realsize;
		Int qp = 10;//rand()%31;

		if( 0 )
			//if(realsize==4) // kolekcjonujemy paczki po 4 macierze 4x4 i robimy z nich 8x8
		{
			for( int i = ofs1; i < bok + ofs1; ++i )
			{
				for( int j = ofs2; j < bok + ofs2; ++j )
				{
					input >> tab4[ i ][ j ];
				}
			}
			if( ofs1 == 4 && ofs2 == 4 ) // gdy tablica 8x8 jest pelna
			{
				splittest( tab4, 8, 4, *t, *q, qp );
			}
			ofsum++;
			ofs1 = 4 * ( ofsum % 2 );
			ofs2 = 4 * ( ofsum / 2 );
			if( ofsum == 4 ) //kolekcjonujemy od nowa zestaw czterech macierzy 4x4
			{
				ofs1 = 0; ofs2 = 0; ofsum = 0;
			}
		}
		else
		{
			tab = new Short*[ bok ];
			for( Int i = 0; i < bok; ++i )tab[ i ] = new Short[ bok ];

			for( int i = 0; i < bok; ++i )
			{
				for( int j = 0; j < bok; ++j )
				{
					input >> tab[ i ][ j ];
				}
			}
			test( tab, bok, *t, *q, qp );
		}
		input >> s;
	}
	for( int i = 0; i < bok; ++i )delete[] tab[ i ];
	delete[] tab;
	input.close( );
}
//
//void forwardAndBackwardTest()
//{
//	Int bok = 4; //rozmiar bloku
//	Int QP;
//
//	string s;
//
//	//strumienie wejsciowe
//	ifstream predInput("D:\\txt\\hm_lumapred_log.txt"); //strumien z macierzami wejsciowymi
//	ifstream quantInput("D:\\txt\\hm_quant_log.txt"); //do odczytania QP
//	ifstream origRecon("D:\\txt\\hm_lumareco_log.txt");
//
//	//=============================
//	string QPsuffix = ".txt";
//	//=============================
//
//	//glowny log
//	ofstream reconstructionLog;
//	reconstructionLog.open("D:\\txt\\reco_reconstructionLog"+QPsuffix,fstream::out | fstream::ate | fstream::app);
//
//	//log transformaty
//	ofstream transformationLog;
//	transformationLog.open("D:\\txt\\reco_transformationLog"+QPsuffix,fstream::out | fstream::ate | fstream::app);
//
//	//log kwantyzacji
//	ofstream quantizationLog;
//	quantizationLog.open("D:\\txt\\reco_quantizationLog"+QPsuffix,fstream::out | fstream::ate | fstream::app);
//
//	//log porownania
//	ofstream comparisonLog;
//	comparisonLog.open("D:\\txt\\reco_comparisonLog"+QPsuffix,fstream::out | fstream::ate | fstream::app);
//
//	//konstruktorom przekazujemy sciezki plikow logow i wlaczamy logowanie
//	Transform t("D:\\txt\\reco_trlog"+QPsuffix,false); 
//	Quant q("D:\\txt\\reco_qlog"+QPsuffix,false); 
//
//	//wylaczamy detale typu podawanie shiftow, offsetow itp
//	q.log.includeDetails=true;
//	t.log.includeDetails=false;
//
//	TrQuantLoop trq("D:\\txt\\reco_trq"+QPsuffix,false,&t,&q);
//
//	Short** block=0;
//	Short** residuals=0;
//	Short** pred=0;
//	Short** tmp=0;
//
//	Short** orgOutputResi=0;
//	Short** orgresult=0;
//	Short** result=0;
//
//	Short** block4in8x8;
//	block4in8x8=new Short*[8];
//	Short** pred4in8x8;
//	pred4in8x8=new Short*[8];
//	Short** resi4in8x8;
//	resi4in8x8=new Short*[8];
//	Short** hortrans4in8x8;
//	hortrans4in8x8=new Short*[8];
//	Short** vertrans4in8x8;
//	vertrans4in8x8=new Short*[8];
//	Short** quant4in8x8;
//	quant4in8x8=new Short*[8];
//	Short** dequant4in8x8;
//	dequant4in8x8=new Short*[8];
//	Short** vertinv4in8x8;
//	vertinv4in8x8=new Short*[8];
//	Short** horinv4in8x8;
//	horinv4in8x8=new Short*[8];
//	Short** reco4in8x8;
//	reco4in8x8=new Short*[8];
//	Short** delta4in8x8;
//	delta4in8x8=new Short*[8];
//	Short** residualsComparison;
//	residualsComparison=new Short*[8];
//	Short** orgOutputResi4in8x8;
//	orgOutputResi4in8x8=new Short*[8];
//
//	for(int i=0;i<8;++i)
//	{
//		block4in8x8[i]=new Short[8];
//		pred4in8x8[i]=new Short[8];
//		resi4in8x8[i]=new Short[8];
//		hortrans4in8x8[i]=new Short[8];
//		vertrans4in8x8[i]=new Short[8];
//		quant4in8x8[i]=new Short[8];
//		dequant4in8x8[i]=new Short[8];
//		vertinv4in8x8[i]=new Short[8];
//		horinv4in8x8[i]=new Short[8];
//		reco4in8x8[i]=new Short[8];
//		delta4in8x8[i]=new Short[8];
//		residualsComparison[i]=new Short[8];
//		orgOutputResi4in8x8[i]=new Short[8];
//	}
//
//	int ofs1=0,ofs2=0,ofsum=0;
//
//	Int counter=0;
//	bool end=false;
//	while(!end)
//	{
//		cout << counter << endl;
//		++counter;
//
//		loadNextMatrix(predInput,block,bok,QP);//block
//		loadNextMatrix(predInput,pred,bok,QP);//prediction
//		loadNextMatrix(predInput,residuals,bok,QP);//residuals
//
//		loadNextMatrix(quantInput,tmp,bok,QP);//coefficients, tylko do odczytania QP
//		loadNextMatrix(quantInput,tmp,bok,QP);//quantization
//
//		loadNextMatrix(origRecon,tmp,bok,QP);//prediction
//		loadNextMatrix(origRecon,orgOutputResi,bok,QP);//residuals
//		loadNextMatrix(origRecon,orgresult,bok,QP);//reconstruction
//
//		result=new Short*[bok];
//		for(int i=0;i<bok;++i)result[i]=new Short[bok];
//
//		trq.TrQuantDequantItr(block,residuals,result,QP,8,bok,false,false);
//
//		if(bok==4)
//		{
//			for(int i=ofs1;i<bok+ofs1;++i)
//			{
//				for(int j=ofs2;j<bok+ofs2;++j)
//				{
//					block4in8x8[i][j]=block[i-ofs1][j-ofs2];
//					pred4in8x8[i][j]=pred[i-ofs1][j-ofs2];
//					resi4in8x8[i][j]=residuals[i-ofs1][j-ofs2];
//					hortrans4in8x8[i][j]=trq.horTrans[i-ofs1][j-ofs2];
//					vertrans4in8x8[i][j]=trq.vertTrans[i-ofs1][j-ofs2];
//					quant4in8x8[i][j]=trq.quant[i-ofs1][j-ofs2];
//					dequant4in8x8[i][j]=trq.dequant[i-ofs1][j-ofs2];
//					vertinv4in8x8[i][j]=trq.invVertTrans[i-ofs1][j-ofs2];
//					horinv4in8x8[i][j]=trq.invHorTrans[i-ofs1][j-ofs2];
//					reco4in8x8[i][j]=result[i-ofs1][j-ofs2];
//					delta4in8x8[i][j]=trq.delta[i-ofs1][j-ofs2];
//					orgOutputResi4in8x8[i][j]=orgOutputResi[i-ofs1][j-ofs2];
//				}
//			}
//
//			if(ofs1==4 && ofs2==4) // gdy tablica 8x8 jest pelna
//			{
//				reconstructionLog << "*** input block "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(block4in8x8,8,reconstructionLog);
//				reconstructionLog << "*** input residuals "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(resi4in8x8,8,reconstructionLog);
//				reconstructionLog << "*** output residuals "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(horinv4in8x8,8,reconstructionLog);
//				reconstructionLog << "*** reconstructed "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(reco4in8x8,8,reconstructionLog);
//
//				transformationLog << "*** residuals "<< bok << "x" << bok << " ***\n";
//				printMatrix(resi4in8x8,8,transformationLog);
//				transformationLog << "*** horizontal transform "<< bok << "x" << bok << " ***\n";
//				printMatrix(hortrans4in8x8,8,transformationLog);
//				transformationLog << "*** vertical transform "<< bok << "x" << bok << " ***\n";
//				printMatrix(vertrans4in8x8,8,transformationLog);
//
//				quantizationLog << "*** coefficients "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(vertrans4in8x8,8,quantizationLog);
//				quantizationLog << "*** quantization "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(quant4in8x8,8,quantizationLog);
//				quantizationLog << "*** dequantization "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(dequant4in8x8,8,quantizationLog);
//
//				transformationLog << "*** coefficients "<< bok << "x" << bok << " ***\n";
//				printMatrix(dequant4in8x8,8,transformationLog);
//				transformationLog << "*** vertical inverse transform "<< bok << "x" << bok << " ***\n";
//				printMatrix(vertinv4in8x8,8,transformationLog);
//				transformationLog << "*** horizontal inverse transform "<< bok << "x" << bok << " ***\n";
//				printMatrix(horinv4in8x8,8,transformationLog);
//
//				comparisonLog << "*** input residuals "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(resi4in8x8,8,comparisonLog);
//				comparisonLog << "*** output residuals HM "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(orgOutputResi4in8x8,8,comparisonLog);
//				comparisonLog << "*** output residuals custom "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				printMatrix(horinv4in8x8,8,comparisonLog);
//				comparisonLog << "*** difference "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//				for(int i=0;i<8;++i)
//				{
//					for(int j=0;j<8;++j)
//					{
//						residualsComparison[i][j]=orgOutputResi4in8x8[i][j]-horinv4in8x8[i][j];
//					}
//				}
//				printMatrix(residualsComparison,8,comparisonLog);
//			}
//			ofsum++;
//			ofs1=4*(ofsum%2);
//			ofs2=4*(ofsum/2);
//			if(ofsum==4) //kolekcjonujemy od nowa zestaw czterech macierzy 4x4
//			{
//				ofs1=0;ofs2=0;ofsum=0;
//			}
//		}
//		else
//		{
//			reconstructionLog << "*** input block "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(block,bok,reconstructionLog);
//			reconstructionLog << "*** input residuals "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(residuals,bok,reconstructionLog);
//			reconstructionLog << "*** output residuals "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(trq.invHorTrans,bok,reconstructionLog);
//			reconstructionLog << "*** reconstructed "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(result,bok,reconstructionLog);
//
//			transformationLog << "*** residuals "<< bok << "x" << bok << " ***\n";
//			printMatrix(residuals,bok,transformationLog);
//			transformationLog << "*** horizontal transform "<< bok << "x" << bok << " ***\n";
//			printMatrix(trq.horTrans,bok,transformationLog);
//			transformationLog << "*** vertical transform "<< bok << "x" << bok << " ***\n";
//			printMatrix(trq.vertTrans,bok,transformationLog);
//
//			quantizationLog << "*** coefficients "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(trq.vertTrans,bok,quantizationLog);
//			quantizationLog << "*** quantization "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(trq.quant,bok,quantizationLog);
//			quantizationLog << "*** dequantization "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(trq.dequant,bok,quantizationLog);
//
//			transformationLog << "*** coefficients "<< bok << "x" << bok << " ***\n";
//			printMatrix(trq.dequant,bok,transformationLog);
//			transformationLog << "*** vertical inverse transform "<< bok << "x" << bok << " ***\n";
//			printMatrix(trq.invVertTrans,bok,transformationLog);
//			transformationLog << "*** horizontal inverse transform "<< bok << "x" << bok << " ***\n";
//			printMatrix(trq.invHorTrans,bok,transformationLog);
//
//			comparisonLog << "*** input residuals "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(residuals,bok,comparisonLog);
//			comparisonLog << "*** output residuals HM "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(orgOutputResi,bok,comparisonLog);
//			comparisonLog << "*** output residuals custom "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			printMatrix(trq.invHorTrans,bok,comparisonLog);
//
//			comparisonLog << "*** difference "<< bok << "x" << bok << " QP = " << QP << " ***\n";
//			tmp=new Short*[bok];
//			for(int i=0;i<bok;++i)tmp[i]=new Short[bok];
//			for(int i=0;i<bok;++i)
//			{
//				for(int j=0;j<bok;++j)
//				{
//					tmp[i][j]=orgOutputResi[i][j]-trq.invHorTrans[i][j];
//				}
//			}
//			printMatrix(tmp,bok,comparisonLog);
//		}
//
//		char c = predInput.get();
//		c = predInput.get();
//		c = predInput.peek();
//		if(c!='*')end=true;
//
//		for(int i=0;i<bok;++i)
//		{
//			delete[] block[i];
//			delete[] residuals[i];
//			delete[] pred[i];
//			delete[] tmp[i];
//			delete[] result[i];
//			delete[] orgOutputResi[i];
//			delete[] orgresult[i];
//		}
//		delete[] block;
//		delete[] residuals;
//		delete[] pred;
//		delete[] tmp;
//		delete[] result;
//		delete[] orgOutputResi;
//		delete[] orgresult;
//
//		if(counter>1000)break;
//	}
//
//	for(int i=0;i<bok;++i)
//	{
//		delete[] block4in8x8[i];
//		delete[] pred4in8x8[i];
//		delete[] resi4in8x8[i];
//		delete[] hortrans4in8x8[i];
//		delete[] vertrans4in8x8[i];
//		delete[] quant4in8x8[i];
//		delete[] dequant4in8x8[i];
//		delete[] vertinv4in8x8[i];
//		delete[] horinv4in8x8[i];
//		delete[] reco4in8x8[i];
//		delete[] delta4in8x8[i];
//		delete[] orgOutputResi4in8x8[i];
//	}
//	delete[] block4in8x8;
//	delete[] pred4in8x8;
//	delete[] resi4in8x8;
//	delete[] hortrans4in8x8;
//	delete[] vertrans4in8x8;
//	delete[] quant4in8x8;
//	delete[] dequant4in8x8;
//	delete[] vertinv4in8x8;
//	delete[] horinv4in8x8;
//	delete[] reco4in8x8;
//	delete[] delta4in8x8;
//	delete[] orgOutputResi4in8x8;
//
//	predInput.close();
//	quantInput.close();
//	origRecon.close();
//	reconstructionLog.close();
//	transformationLog.close();
//	quantizationLog.close();
//	comparisonLog.close();
//}

//void frameTest()
//{
//	/*int size=8;
//	int imgH=176;
//	int imgW=144;*/
//	int size=4;
//	int imgH=20;
//	int imgW=20;
//
//	Frame f;
//	f.setSize(imgW,imgH);
//	std::ifstream yuv("D:\\h265\\yuv\\akiyo_qcif.yuv", ios::in|ios::binary);
//
//	int** reco = getEmptyMatrix<int>(imgW,imgH);
//
//	int* leftRefs = new int[2*size];
//	int* topRefs = new int[2*size];
//	int corner;
//
//	f.loadFrameFromYuv(yuv);
//	//short** img = f.getLumaBlock();
//	short** img = new short*[imgW];
//	for(int i=0;i<imgW;++i)
//	{
//		img[i]=new Short[imgH];
//		for(int j=0;j<imgH;++j)
//		{
//			img[i][j]=rand()%256;
//		}
//	}
//	//printMatrix<short>(img,imgH,std::cout);
//
//	SeqParams* SPM = SeqParams::getInstance();
//	SPM->setBitDepthChroma(8);
//	SPM->setBitDepthLuma(8);
//	SPM->setMaxCuSize(64);
//	SPM->setPicHeight(imgH);
//	SPM->setPicWidth(imgW);
//	SPM->setSmoothEn(false);
//
//	IntraPu ipu(NULL);
//	ipu.setImgComp(LUMA);
//	ipu.setPuSize(size);
//
//	int offsetX=0,offsetY=0;
//	bool end=false;
//	while(offsetY<imgH)
//	{
//		int actualSize = size;
//		while(offsetY+size>imgH || size==2)
//		{
//			size>>=1;
//		}
//		if(size==2)break;
//
//		while(offsetX<imgW)
//		{
//			if(offsetX+actualSize>imgW)break;
//
//			if(offsetX>0)
//			{
//				for(int i=offsetY;i<offsetY+2*actualSize;++i)
//				{
//					if(i>=imgH) leftRefs[i-offsetY]=128;
//					else leftRefs[i-offsetY]=reco[i][offsetX-1];
//				}
//			}
//			else
//			{
//				for(int i=0;i<2*actualSize;++i)
//				{
//					leftRefs[i]=128;
//				}
//			}
//
//			if(offsetY>0)
//			{
//				for(int i=offsetX;i<offsetX+2*actualSize;++i)
//				{
//					if(i>=imgW) topRefs[i-offsetX]=128;
//					else topRefs[i-offsetX]=reco[offsetY-1][i];
//				}
//			}
//			else
//			{
//				for(int i=0;i<2*actualSize;++i)
//				{
//					topRefs[i]=128;
//				}
//			}
//
//			if(offsetX>0 && offsetY>0)corner=reco[offsetX-1][offsetY-1];
//			else corner=128;
//
//			std::cout << "(" << offsetX << "," << offsetY << ")" << std::endl;
//			std::cout << "baza:" << std::endl;
//			std::cout << corner << " ";
//			for(int i=0;i<2*actualSize;++i)std::cout << topRefs[i] << " ";
//			std::cout << std::endl;
//			for(int i=0;i<2*actualSize;++i)std::cout << leftRefs[i] << std::endl;
//			int modes[]={0};
//
//			Short** block = getEmptyMatrix<Short>(actualSize,actualSize);
//			for(int i=0;i<actualSize;++i)
//			{
//				for(int j=0;j<actualSize;++j)
//				{
//					block[i][j] = img[i+offsetY][j+offsetX];
//				}
//			}
//
//			std::cout << "blok:" << std::endl;
//			printMatrix(block,actualSize,std::cout);
//
//			for(int i=0;i<1;++i)
//			{
//				ipu.setPuSize(actualSize);
//				ipu.setModeIdx(modes[i]);
//				int ** pred = ipu.getPredForceRef(leftRefs,topRefs,corner);
//				Short** sPred = new Short*[actualSize];
//				for(int j=0;j<actualSize;++j)
//				{
//					sPred[j]= new Short[actualSize];
//					for(int k=0;k<actualSize;++k)
//					{
//						sPred[j][k]=pred[j][k];
//					}
//				}
//				std::cout << "predykcja " << modes[i] << ":" << std::endl;
//				printMatrix(sPred,actualSize,std::cout);
//
//				Short** residuals = new Short*[actualSize];
//				for(int j=0;j<actualSize;++j)
//				{
//					residuals[j]= new Short[actualSize];
//					for(int k=0;k<actualSize;++k)
//					{
//						residuals[j][k]=block[j][k]-sPred[j][k];
//					}
//				}
//				std::cout << "residua " << modes[i] << ":" << std::endl;
//				printMatrix(residuals,actualSize,std::cout);
//				Short** recoResult = getEmptyMatrix<Short>(actualSize,actualSize);
//				TrQuantLoop tr("",false);
//				tr.TrQuantDequantItr(block,residuals,recoResult,3,8,actualSize,false,false);
//				std::cout << "rekonstrukcja bloku" << modes[i] << ":" << std::endl;
//				printMatrix(recoResult,actualSize,std::cout);
//				for(int j=offsetY;j<offsetY+actualSize;++j)
//				{
//					for(int k=offsetX;k<offsetX+actualSize;++k)
//					{
//						reco[j][k]=recoResult[j-offsetY][k-offsetX];
//					}
//				}
//				std::cout << "obraz:" << std::endl;
//				printMatrix(img,imgH,std::cout);
//				std::cout << "rekonstrukcja obrazu:" << std::endl;
//				printMatrix(reco,imgH,std::cout);
//			}
//
//			offsetX+=actualSize;
//		}
//		offsetX=0;
//		offsetY+=actualSize;
//	}
//
//	for(int j=0;j<imgW;++j)
//	{
//		for(int k=0;k<imgH;++k)
//		{
//			reco[j][k]=img[j][k]-reco[j][k];
//		}
//	}
//	std::cout << "roznica:" << std::endl;
//	printMatrix(reco,imgH,std::cout);
//}

//void predictionTest( )
//{
//	/*int size=8;
//	int imgH=176;
//	int imgW=144;*/
//	int size = 4;
//	UInt imgH = 20;
//	int imgW = 20;
//
//	Picture f;
//	f.setSize( imgW, imgH );
//	std::ifstream yuv( "D:\\h265\\yuv\\akiyo_qcif.yuv", ios::in | ios::binary );
//
//	int** reco = getEmptyMatrix<int>( imgW, imgH );
//
//	int* leftRefs = new int[ 2 * size ];
//	int* topRefs = new int[ 2 * size ];
//	int corner;
//
//	f.loadFrameFromYuv( yuv );
//	int** img = f.reconMatrix( LUMA );
//	/*for(int i=0;i<imgW;++i)
//	{
//	img[i][0]=rand()%256;
//	}*/
//	img[ 0 ][ 5 ] = 60;
//	img[ 5 ][ 0 ] = 100;
//
//	printMatrix<UShort>( f.samples( LUMA ), imgH, std::cout );
//	std::cout << std::endl;
//	printMatrix<int>( img, imgH, std::cout );
//	std::cout << std::endl;
//
//	SequenceParameters* SPM = SequenceParameters::getInstance( );
//	SPM->setBitDepthChroma( 8 );
//	SPM->setBitDepthLuma( 8 );
//	SPM->setMaxCuSize( 64 );
//	SPM->setPicHeight( imgH );
//	SPM->setPicWidth( imgW );
//	SPM->setIntraSmoothingEnabled( false );
//
//	CU cu( &f, 1, 1, 8 );
//	PU* pu = new PU( &cu, 1, 1 );
//	pu->setPuSize( 8 );
//	cu.addPu( *pu );
//	pu->setModeIdx( 1 );
//	int** res = pu->getPred( LUMA );
//	printMatrix<int>( res, 8, std::cout );
//}

void binarizationTest( )
{
	Binarization b( "D:\\txt\\binlog.txt", true );

	ifstream input( "D:\\txt\\ec_summary.txt" ); //strumien z macierzami wejsciowymi
	UInt bins;

	Short** matrix;
	do
	{
		loadNextMatrix( input, matrix, bins, 4 );

		UInt res = b.countBinsIn4x4TU( matrix, new DiagonalScanningMode( ) );
		cout << "modelowo: " << bins << std::endl;
		cout << std::endl;
	}
	while( !input.eof( ) );

	deleteMatrix( matrix, 4 );
}

void pictureTest( )
{
	SeqParams( )->setPicWidth( 16 );
	SeqParams( )->setPicHeight( 16 );
	ZScanArray::reset( );
	std::auto_ptr<Picture> f( new Picture( ) );
	std::ifstream yuv( "D:\\h265\\yuv\\akiyo_qcif.yuv", ios::in | ios::binary );
	std::ofstream out;
	out.open( "D:\\txt\\reconstruction.txt", std::fstream::out | std::fstream::ate );
	f->loadFrameFromYuv( yuv );
	f->initPartitions( DefaultSmallPartitions::getStrategy( ) );
	f->printDescription( );

	f->reconstructionLoop( );
}

/**
*	Funkcja glowna, dokonuje testow na macierzach z podanego pliku
*/
Int main( Int argc, Char* argv[] )
{
	//classicTest();

	//forwardAndBackwardTest();

	//predictionTest();

	//binarizationTest( );

	pictureTest( );

	system( "PAUSE" );

	return EXIT_SUCCESS;
}

#endif