#ifndef __MATCH_H__
#define __MATCH_H__

#include "primitives.h"
#include "hole.h"
#include "RadiosityMan.h"
#include "pieces.h"
#include "systemInterface.h"
#include "gameSet.h"
#include "menus.h"
#include "wave.h"

//! an int matrix
struct BMatrix
{
	B3D r,u,a,p;
	BMatrix() :r(1,0,0),u(0,1,0),a(0,0,1),p() {};
	void Reset()
	{
		r.Set(1,0,0);
		u.Set(0,1,0);
		a.Set(0,0,1);
		p.Reset();
	}

	friend inline B3D operator*(const B3D &a,const BMatrix &b)
	{
		/*return B3D(
			b.r.x*a.x+b.r.y*a.y+b.r.z*a.z+b.p.x,
			b.u.x*a.x+b.u.y*a.y+b.u.z*a.z+b.p.y,
			b.a.x*a.x+b.a.y*a.y+b.a.z*a.z+b.p.z);/*/
		return B3D(
			 b.r.x*a.x+b.u.x*a.y-b.a.x*a.z+b.p.x,
			 b.r.y*a.x+b.u.y*a.y-b.a.y*a.z+b.p.y,
			-b.r.z*a.x-b.u.z*a.y+b.a.z*a.z+b.p.z);/*/
		return b.p+B3D(b.r*a.x+b.u*a.y+b.a*a.z); //*/
	}

	BMatrix GetTranslate(const B3D &d) { BMatrix m=*this; m.p+=d; return m; }
	//! Sets matrix like X rotation of #rot*90°
	BMatrix SetXRotate(char rot)
	{
		rot=rot%4;
		p.Reset(); r.Set(1,0,0);
		u.x=a.x=0;
		u.y=a.z=(rot%2)? 0 : (rot==0)? 1 : -1;
		a.y=(rot%2)? (rot==1)? -1 : 1 : 0;
		u.z=-a.y;
		return *this;
	}

	//! Sets matrix like Y rotation of #rot*90°
	BMatrix SetYRotate(char rot)
	{
		rot=rot%4;
		p.Reset(); u.Set(0,1,0);
		r.y=a.y=0;
		r.x=a.z=(rot%2)? 0 : (rot==0)? 1 : -1;
		a.x=(rot%2)? (rot==1)? 1 : -1 : 0;
		r.z=-a.x;
		return *this;
	}

	//! Sets matrix like Z rotation of #rot*90°
	BMatrix SetZRotate(char rot)
	{
		rot=rot%4;
		p.Reset(); a.Set(0,0,1);
		r.z=u.z=0;
		r.x=u.y=(rot%2)? 0 : (rot==0)? 1 : -1;
		u.x=(rot%2)? (rot==1)? -1 : 1 : 0;
		r.y=-u.x;
		return *this;
	}
	friend BMatrix operator*(const BMatrix &a,const BMatrix &b)
	{
		BMatrix m;
		m.r.x=a.r.x*b.r.x+a.r.y*b.u.x+a.r.z*b.a.x;
		m.r.y=a.r.x*b.r.y+a.r.y*b.u.y+a.r.z*b.a.y;
		m.r.z=a.r.x*b.r.z+a.r.y*b.u.z+a.r.z*b.a.z;
		m.u.x=a.u.x*b.r.x+a.u.y*b.u.x+a.u.z*b.a.x;
		m.u.y=a.u.x*b.r.y+a.u.y*b.u.y+a.u.z*b.a.y;
		m.u.z=a.u.x*b.r.z+a.u.y*b.u.z+a.u.z*b.a.z;
		m.a.x=a.a.x*b.r.x+a.a.y*b.u.x+a.a.z*b.a.x;
		m.a.y=a.a.x*b.r.y+a.a.y*b.u.y+a.a.z*b.a.y;
		m.a.z=a.a.x*b.r.z+a.a.y*b.u.z+a.a.z*b.a.z;
		m.p=a.p+b.p;
		return m;
	}
};

//! Descrizione della classe
class Match
{
private:
	enum State
	{
		NORMAL,				//!< playing
		WAIT_NEXT_PIECE,	//!< wait for piece or plane disappearing
		IN_MENU,			//!< pause
		GAME_OVER,			//!< end of game
		INVALID
	} state;
	unsigned char timeToNextPiece;
	unsigned char nLines;

	unsigned char currPiece;
	unsigned char nextPiece;
	char shake;
	char totalShake;
	B3D v,v_time;
	B3D r,r_time;
	char realz;

	Matrix m;
	BMatrix bm;
	bool fall;

	bool myButtons[NUM_BUTTON];
	void AddShake(char shakeFact);

	unsigned short miniHoleAnglePos;
	//! Size in 3D units of side of cubes
	float sideSize;
	//! percent of free space in half screen
	float spaceForText;
	float moveBack;
	void SetUpProjMatrix();

	unsigned short timeToNextDown;
	short timeToPlacePiece;
	bool hasDown;
	unsigned char level;
	unsigned short nLineMade;
	unsigned short score;

	Hole previewHole;
	Hole hole;

	void InitPiece();
	bool Check(const BMatrix &p,B3D *mPtr=0);
	void PlacePiece();
	void UpdateTex();

	GameSet *current;
	GameSet::Records *record;
	char message[NAME_LEN];
	Dialog gameOver;
	DialogEdit insertName;


	unsigned char nextPlacePieceSourceToPlay;
	//! Sound sources of plane disappearing
	unsigned int doneLinesSource[5];
	//! Sound buffers of plane disappearing
	unsigned int doneLinesBuffer[5];
	//! Sound sources of piece placing
	unsigned int placePieceSource[2];
	//! Sound buffer of piece placing
	unsigned int placePieceBuffer;

	//! reverbe effect slot
	unsigned int effectSlot;
	//! reverbe effect
	unsigned int effect;

	void Deinit(bool total);

	unsigned char lastMusic;
	void PlayMusic();
public:
	MyOggVorbisData *music;

	bool IsOn() { return (state!=INVALID); }

	Pieces pieces,nextPieceLines;
	Match() { state=INVALID; doneLinesSource[0]=0; music=0; }
	~Match() { Deinit(true); }

	void Init();
	void Update();
	void Draw2D();
	void Draw();

	bool End(bool);

	void CheckSyncMatrix();
	float Get2DScreenWidth();
};

#endif //__MATCH_H__
