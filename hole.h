#ifndef __HOLE_H__
#define __HOLE_H__

#define USE_COMPRESS

#include "primitives.h"
#include "DynamicMesh.h"
#include "radiosityMan.h"

#ifdef USE_COMPRESS
#define ZLIB_DLL
#define ZLIB_WINAPI
	#include "zconf.h"
	#include "zlib.h"
	#define DFILE gzFile
	#define READ(what,how,where) gzread(where,what,how);
	#define WRITE(what,how,where) gzwrite(where,what,how);
#else
	#include <stdio.h>
	#define DFILE FILE*
	#define READ(what,how,where) fread(what,1,how,where);
	#define WRITE(what,how,where) fwrite(what,1,how,where);
#endif

class Hole;
class BOCube
{
public:
	union
	{
		short flags;
		struct
		{
			unsigned char on; //0 off, 1 on, 2 borderd, 3 upper border
			bool unionXpos:1;
			bool unionXneg:1;
			bool unionYpos:1;
			bool unionYneg:1;
			bool unionZpos:1;
			bool unionZneg:1;
		};
	};
	B3D p;
	Color color;
	V3D center,inf,sup;
private:
	RadiosityMan::BOFace *faces[6];
	unsigned int tex;

	void AddFace(V3D ce,V3D d1,V3D d2,float minu,float minv,float maxu,float maxv);
	void AddEdge(
		V3D p1,V2D t1,V3D p2,V2D t2,
		V3D d1,V2D dt1,V3D d2,V2D dt2,bool round);
	void AddCorner(V3D p,char dx,char dy,char dz,float delta,V2D t,V2D tx,V2D ty,V2D tz,
		bool xFill,bool yFill,bool zFill);
public:
	DynamicMesh *mesh;
	DynamicCage *wire;

	BOCube();
	~BOCube();
	void UpdateGeom(Hole *h);
	void UpdateTex(Hole *h);
	void Draw();

	BOCube *next;

	void Load(DFILE f,Hole *h);
	void Save(DFILE f,Hole *h);

};
//! Descrizione della classe
class Hole
{
	friend class BOCube;
private:
	unsigned char cubeTexBits[N_DIVISION_MAX*N_DIVISION_MAX*16*3];
private:
	DynamicMesh::Vertex holeVertices[5][4];
	unsigned char *bitsTmp;
	unsigned short texSize[3][2];
	unsigned int holeTex[5];
	RadiosityMan::BOFace **holeFaces[5];
	char sideA,sideB,depth;
	unsigned long nCube;
	BOCube *cubes;
	BOCube *onCubes;

	bool *xPlanes;
	bool *yPlanes;
	bool *zPlanes;
	B3D infOn,supOn;
	void InitCollision();
	void UpdateCollision();

	unsigned char nLineInGrid[5];
	V3D *grid[5];
public:
	RadiosityMan radMan;

	void UpdateTexs();

	BOCube *GetBOCube(const B3D &p);
	V3D inf,sup;

	Hole() {
		sideA=sideB=depth=0;
		bitsTmp=0;
		holeTex[0]=holeTex[1]=holeTex[2]=holeTex[3]=holeTex[4]=0;
		holeFaces[0]=holeFaces[1]=holeFaces[2]=holeFaces[3]=holeFaces[4]=0;
		xPlanes=yPlanes=zPlanes=0;
		cubes=0;
		grid[0]=grid[1]=grid[2]=grid[3]=grid[4]=0;
	}
	~Hole() { Deinit(); }
	void Deinit();

	void Init(unsigned char sideA,unsigned char sideB,unsigned char depth);
	void InitForPreview();
	void SetPreview(unsigned char piece,bool calcRad=true);

	void UpdateAll(bool calcRad=true);
	void Draw(bool xpos=true,bool ypos=true,bool xneg=true,bool yneg=true,bool cubes=true);

	unsigned char GetSideA() const { return sideA; }
	unsigned char GetSideB() const { return sideB; }
	unsigned char GetDepth() const { return depth; }

	bool CheckCollision(
		const V3D &a,const B3D &cb3d,
		const V3D &b,const B3D &bb3d,bool side);

	V3D Get3D(const B3D &p);
	B3D Revert3D(const V3D &p);
	bool Check(const B3D &p);
	unsigned char CheckForPlanes();

	void Empty();

	void Load(const char *);
	void Save(const char *);
};

#endif //__HOLE_H__
