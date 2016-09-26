#ifndef __PIECES_H__
#define __PIECES_H__

#include "primitives.h"
#include "hole.h"

//! definition of a polycube or piece
struct PieceDef
{
	unsigned char nPiece;
	B3D dirs[4];

	PieceDef(unsigned char n,const B3D &dir0,const B3D &dir1,const B3D &dir2,const B3D &dir3)
	{
		nPiece=n;
		dirs[0]=dir0; dirs[1]=dir1;
		dirs[2]=dir2; dirs[3]=dir3;
	}
};

//! number of piece
#define N_PIECES 41
//! all pieces
extern PieceDef colletion[N_PIECES];

//! managing of a piece
class Pieces
{
	struct line
	{
		bool on;
		V3D a,b;
	} lines[60];
	void ResetLines();
	void AddLineInWireframe(const V3D &a,const V3D &b);
	void AddBoxInWireframe(const V3D &s,const V3D &i);
	unsigned char nLines;
	V3D v[120];
public:
	Box3 box;

	Pieces() {};
	~Pieces() {};

	void SetupDraw(unsigned char i);
	void Draw();
};

#endif //__PIECES_H__
