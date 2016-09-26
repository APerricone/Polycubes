#ifndef __DYNAMICMESH_H__
#define __DYNAMICMESH_H__
#include "primitives.h"


class DynamicMesh
{
public:
	struct Vertex
	{
		V3D pos;
		V2D tex;
		Vertex() {};
		Vertex(float px,float py,float pz,float tx,float ty) { pos.Set(px,py,pz); tex.Set(tx,ty); }
		Vertex(const V3D &p,const V2D &t) { pos=p; tex=t; }
		void Set(float px,float py,float pz,float tx,float ty) { pos.Set(px,py,pz); tex.Set(tx,ty); }
		void Set(const V3D &p,const V2D &t) { pos=p; tex=t; }
		friend inline bool operator==(const Vertex &a,const Vertex &b) { return (a.pos==b.pos)&&(a.tex==b.tex); }
	};
	unsigned short nVertices;
	Vertex *vertices;
	unsigned short nTriangle;
	unsigned short *indices;
private:

	bool Create;

	struct listVertex
	{
		Vertex v;
		listVertex *next;
	} *tmpVertices;
	struct listTriangle
	{
		listVertex *a,*b,*c;
		listTriangle *next;
	} *tmpTriangles;

	listVertex *AddVertexToTempList(const Vertex &v);
	listTriangle *AddTriangleToTempList(listVertex *a,listVertex *b,listVertex *c);

	unsigned short FindVertex(const Vertex &v);
public:

	DynamicMesh() { Create=false; vertices=0; indices=0; nVertices=0; nTriangle=0; tmpVertices=0; tmpTriangles=0; }
	~DynamicMesh() { Deinit(); }

	void Begin();
	void AddTrianglePos(const Vertex &a,const Vertex &b,const Vertex &c);
	void TransformAll(const Matrix&);
	void End();

	void CalcNormal();
	void CalcBox(Box3 *d);
	void Draw();

	void Deinit();
};

class DynamicCage
{
public:
	unsigned short nVertices;
	V3D *vertices;
	unsigned short nLines;
	unsigned short *indices;
private:
	bool Create;
	struct listVertex
	{
		V3D v;
		listVertex *next;
	} *tmpVertices;
	struct listLines
	{
		listVertex *a,*b;
		listLines *next;
	} *tmpLines;

	listVertex *AddVertexToTempList(const V3D &v);
	listLines *AddLineToTempList(listVertex *a,listVertex *b);

	unsigned short FindVertex(const V3D &v);
public:

	DynamicCage() { Create=false; vertices=0; indices=0; nVertices=0; nLines=0; tmpVertices=0; tmpLines=0; }
	~DynamicCage() { Deinit(); }

	void Begin();
	void AddLine(const V3D &a,const V3D &b);
	void TransformAll(const Matrix&);
	void End();

	void CalcBox(Box3 *d);
	void Draw();

	void Deinit();
};

#endif //__DYNAMICMESH_H__
