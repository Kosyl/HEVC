#include "Binarization.h"
#include <bitset>
#include <fstream>
#include <stdio.h>

const Int Binarization::COEFF_REMAIN_THRESHOLD = 3; //przejœcie z GR na TU+EG(k)

Binarization::Binarization():
	log()
{
}

Binarization::Binarization(std::string logPath, Bool isLogging):
	log(logPath,isLogging)
{
}

Binarization::~Binarization()
{
}

UInt Binarization::binarizeCoefficientRemainingLevel_NumBits(UInt symbol, UInt &rParam)
{
	Int codeNumber  = (Int)symbol;
	UInt length;

	UInt prefixBins, prefixLength, suffixBins, suffixLength;

	log << "(" << symbol << "," << rParam << ")" << std::endl;
	if (codeNumber < (COEFF_REMAIN_THRESHOLD << rParam))
	{
		log << "kodowanie GR, remain: " << codeNumber << " < " << (COEFF_REMAIN_THRESHOLD << rParam) << std::endl;
		++log;

		length = codeNumber>>rParam;

		prefixBins = (1<<(length+1))-2;
		prefixLength = length+1;
		suffixBins = codeNumber%(1<<rParam);
		suffixLength = rParam;

		std::bitset<(sizeof Int)*8> first(prefixBins);
		std::bitset<(sizeof Int)*8> second(suffixBins);
		for(int i=prefixLength-1;i>=0;--i)
			log << first[i];
		log << " ";
		for(int i=suffixLength-1;i>=0;--i)
			log << second[i];
		log << std::endl;
		--log;
	}
	else
	{
		log << "kodowanie TU+EG(k), remain: " << codeNumber << " >= " << (COEFF_REMAIN_THRESHOLD << rParam) << std::endl;
		++log;
		length = rParam;
		codeNumber  = codeNumber - ( COEFF_REMAIN_THRESHOLD << rParam);

		log << "poprawka remain: " << codeNumber << std::endl;

		++log;
		while (codeNumber >= (1<<length))
		{
			log << "remain = " << codeNumber << " >= 1<<length = " << (1<<length) << std::endl;
			codeNumber -=  (1<<(length++));
			log << "remain = " << codeNumber << ", length = " << length << std::endl;
		}
		--log;

		prefixBins = (1<<(COEFF_REMAIN_THRESHOLD+length+1-rParam))-2;
		prefixLength = COEFF_REMAIN_THRESHOLD+length+1-rParam;
		suffixBins = codeNumber;
		suffixLength = length;

		std::bitset<(sizeof Int)*8> first(prefixBins);
		std::bitset<(sizeof Int)*8> second(suffixBins);
		for(int i=prefixLength-1;i>=0;--i)
			log << first[i];
		log << " ";
		for(int i=suffixLength-1;i>=0;--i)
			log << second[i];
		log << std::endl;
		--log;
	}
	log << "(" << symbol << "," << rParam << ") -> " << (prefixLength+suffixLength) << " bitow" << std::endl;
	return prefixLength+suffixLength;
}