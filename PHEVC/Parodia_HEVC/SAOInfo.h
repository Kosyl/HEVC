#ifndef SAOInfo_h__
#define SAOInfo_h__

#include "Utils.h"
#include <bitset>

class SAOInfo
{
	Bool myMergeLeft, myMergeUp;
	SAOType mySAOType, mySAOTypeLuma, mySAOTypeChroma;
	std::bitset<4> myOffsetSign;
	Int myBandPosition;
	Int myEOClassLuma, myEOClassChroma;
};

#endif // SAOInfo_h__