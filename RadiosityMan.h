#ifndef __RADIOSITYMAN_H__
#define __RADIOSITYMAN_H__

#include "primitives.h"

#define N_DIVISION_MAX 64
#define PIXEL_STEP 4

struct B3D
{
	char x,y,z;

	B3D() { x=y=z=0; }
	B3D(char _x,char _y,char _z) { x=_x; y=_y; z=_z; }

	void Reset() { x=y=z=0; }
	void Set(char _x,char _y,char _z) { x=_x; y=_y; z=_z; }

	friend inline B3D operator+(const B3D &a,const B3D &b) { return B3D(a.x+b.x,a.y+b.y,a.z+b.z); }
	friend inline B3D operator-(const B3D &a) { return B3D(-a.x,-a.y,-a.z); }
	friend inline B3D operator-(const B3D &a,const B3D &b) { return B3D(a.x-b.x,a.y-b.y,a.z-b.z); }
	inline B3D& operator+=(const B3D &b) { x+=b.x; y+=b.y; z+=b.z; return *this; }
	inline B3D& operator-=(const B3D &b) { x-=b.x; y-=b.y; z-=b.z; return *this; }
	friend inline B3D operator*(const B3D &a,const char &b) { return B3D(a.x*b,a.y*b,a.z*b); }

	//! [] operator for use with for loop
	char &operator[](int i) { return (i==0)? x :(i==1)? y : z;}
	const char operator[](int i) const { return (i==0)? x :(i==1)? y : z;}
};

class Hole;
//! radiosity class di super optimized for polycubes
class RadiosityMan
{
public:
	class BOFace
	{
	public:
		B3D posChar;
		//! on/off of face (if 2 are in contact)
		bool on;
		bool ok[4];
		//! certer 3D of face
		V3D center;
		//! plane normal
		V3D normal;
		//! total light in the face
		short totalr;
		short totalg;
		short totalb;
		//! color bounced in radiosity calcutation
		//! \note in base of #RadiosityMan::actual_bounce
		Color bounce[2];
		//! pixel color on direct light
		Color pixelDirect[N_DIVISION_MAX][N_DIVISION_MAX];
		//! pixel color on indirect light
		Color pixelBounce[N_DIVISION_MAX][N_DIVISION_MAX];
		//! pixel 3D positions
		V3D positionsDirect[N_DIVISION_MAX][N_DIVISION_MAX];
		V3D positionsBounce[N_DIVISION_MAX][N_DIVISION_MAX];
		//! material color of face
		Color material;
		//! next face in #RadiosityMan::faces list
		BOFace *next;
		//! next face in #RadiosityMan::facesOn list
		BOFace *nextOn;
	};
private:
	//! all faces
	BOFace *faces;
	//! the "on" faces
	BOFace *facesOn;
	//! index of #BOFace::bounce where it writes in #DoBounce
	unsigned char actual_bounce;

	Hole *parent;
public:

	unsigned long nDivision;

	RadiosityMan() { faces=0; nDivision=8; }
	~RadiosityMan() { DelAllFaces(); }

	void SetParent(Hole *h) { parent=h; }
	//! add a face
	BOFace *AddFace(const V3D &pos,const V3D &nor,const Color &materialColor,float scale);
	//! del a face
	void DelFace(BOFace *toDel);
	//! dell all faces
	void DelAllFaces();

	void MakeRadiosity(bool bounce);

	BOFace *GetNextFace();
	void DoDirect(BOFace *f);
	void DoBounce(BOFace *f,unsigned char id);

	unsigned char radState;
	bool iCalc[4];
	bool iRadiosity;

	void *hMutex;

	unsigned char idTh;
private:
	BOFace *currFace;

};

#endif //__RADIOSITYMAN_H__
