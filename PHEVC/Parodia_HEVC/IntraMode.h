#ifndef INTRA_MODE_H
#define INTRA_MODE_H

#include <cassert>

class PBIntra;

class IntraMode
{
protected:
	PBIntra *itsCurrentPB;
	Sample itsCornerReference;
	Sample** itsReferenceValues;
	Sample** initPred( ) const;
public:
	IntraMode( );
	virtual ~IntraMode( );
	virtual Sample** calcPred( ) = 0;
	virtual Void setPb( PBIntra* PB );
	virtual Void setCorner( const Sample corner );
	virtual Void setSideRefs( const IntraDirection dir, Sample* ref );
};

class PlanarMode : public IntraMode
{
public:
	PlanarMode( );
	~PlanarMode( );
	Sample **calcPred( );
};

class DcMode : public IntraMode
{
private:
	Sample itsDCVal;
	Bool itsFiltreEdges;
	Void calcDcVal( );
	Sample getFiltCorner( ) const;
	Sample getFiltEdge( const IntraDirection dir, const UInt offset) const;
public:
	DcMode( );
	~DcMode( );
	Sample **calcPred( );
};

class LinearMode : public IntraMode
{
private:
	Sample getFiltEdge( const IntraDirection dir, const UInt offset );
public:
	LinearMode( );
	~LinearMode( );
	Sample **calcPred( );
};

class AngMode : public IntraMode
{
private:
	const static Int angles[];
	const static Int invAngles[];
	Sample* refsArray;
	Int getAngle( ) const;
	Int getInvAngle( ) const;
	Void getRefsArray( );
public:
	AngMode( );
	~AngMode( );
	Sample **calcPred( );
};

#endif