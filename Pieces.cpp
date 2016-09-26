#include "Pieces.h"
#include "hole.h"
#include "game.h"

void Pieces::ResetLines()
{
	unsigned char i;
	for(i=0;i<60;i++)
		lines[i].on=false;
}

void Pieces::AddLineInWireframe(const V3D &a,const V3D &b)
{
	//check if i already have it
	unsigned char i;
	for(i=0;i<60;i++)
		if(lines[i].on)
		{
			if(lines[i].a==a && lines[i].b==b)
			{
				lines[i].on=false;
				return;
			}
		}
	for(i=0;i<60;i++)
		if(!lines[i].on)
		{
			lines[i].a=a;
			lines[i].b=b;
			lines[i].on=true;
				return;
		}
}

void Pieces::AddBoxInWireframe(const V3D &s,const V3D &i)
{
	AddLineInWireframe(V3D(s.x,s.y,s.z),V3D(i.x,s.y,s.z));
	AddLineInWireframe(V3D(s.x,i.y,s.z),V3D(i.x,i.y,s.z));
	AddLineInWireframe(V3D(s.x,s.y,i.z),V3D(i.x,s.y,i.z));
	AddLineInWireframe(V3D(s.x,i.y,i.z),V3D(i.x,i.y,i.z));

	AddLineInWireframe(V3D(s.x,s.y,s.z),V3D(s.x,i.y,s.z));
	AddLineInWireframe(V3D(i.x,s.y,s.z),V3D(i.x,i.y,s.z));
	AddLineInWireframe(V3D(s.x,s.y,i.z),V3D(s.x,i.y,i.z));
	AddLineInWireframe(V3D(i.x,s.y,i.z),V3D(i.x,i.y,i.z));

	AddLineInWireframe(V3D(s.x,s.y,s.z),V3D(s.x,s.y,i.z));
	AddLineInWireframe(V3D(i.x,s.y,s.z),V3D(i.x,s.y,i.z));
	AddLineInWireframe(V3D(s.x,i.y,s.z),V3D(s.x,i.y,i.z));
	AddLineInWireframe(V3D(i.x,i.y,s.z),V3D(i.x,i.y,i.z));
}

void Pieces::SetupDraw(unsigned char i)
{
	char j;
	V3D c;
	ResetLines();
	AddBoxInWireframe(V3D(0.5f,0.5f,0.5f),V3D(-0.5f,-0.5f,-0.5f));
	for(j=0;j<colletion[i].nPiece-1;j++)
	{
		c.x=colletion[i].dirs[j].x;
		c.y=colletion[i].dirs[j].y;
		c.z=(float)(-colletion[i].dirs[j].z);
		AddBoxInWireframe(c+V3D(0.5f,0.5f,0.5f),c-V3D(0.5f,0.5f,0.5f));
	}
	nLines=0;
	box.Reset();
	for(j=0;j<60;j++)
	{
		if(lines[j].on)
		{
			box.Add(lines[j].a);
			box.Add(lines[j].b);
			v[nLines]=lines[j].a; nLines++;
			v[nLines]=lines[j].b; nLines++;
		}
	}

}

void Pieces::Draw()
{
#ifndef NOGL
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(V3D),v);
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glColor3f(0,0,0);
	glDrawArrays(GL_LINES,0,nLines);
	glLineWidth(1);
	glColor3f(1,1,1);
	glDrawArrays(GL_LINES,0,nLines);
#endif
}

PieceDef prova=PieceDef(1,B3D(0,0,0),B3D(0,0,0),B3D(0,0,0),B3D(0,0,0));
PieceDef colletion[N_PIECES]= {
//0
	PieceDef(1,B3D(0,0,0),B3D( 0,0,0),B3D(0, 0,0),B3D(0,0,0)),
	PieceDef(2,B3D(0,1,0),B3D( 0,0,0),B3D(0, 0,0),B3D(0,0,0)),
	PieceDef(3,B3D(0,1,0),B3D(0,-1,0),B3D(0, 0,0),B3D(0,0,0)),
	PieceDef(3,B3D(0,1,0),B3D(-1,0,0),B3D(0, 0,0),B3D(0,0,0)),
	PieceDef(4,B3D(0,1,0),B3D(0,-1,0),B3D(0,-2,0),B3D(0,0,0)),
	PieceDef(4,B3D(0,1,0),B3D(0,-1,0),B3D(-1,1,0),B3D(0,0,0)),
	PieceDef(4,B3D(0,1,0),B3D(0,-1,0),B3D(-1,0,0),B3D(0,0,0)),
	PieceDef(4,B3D(0,1,0),B3D(-1,0,0),B3D(-1,-1,0),B3D(0,0,0)),
	PieceDef(4,B3D(0,1,0),B3D(-1,0,0),B3D(-1,1,0),B3D(0,0,0)),
	PieceDef(4,B3D(0,1,0),B3D(-1,0,0),B3D(0,1,-1),B3D(0,0,0)),
//10
	PieceDef(4,B3D(0,1,0),B3D(-1,0,0),B3D(0,0,-1),B3D(0,0,0)),
	PieceDef(4,B3D(0,1,0),B3D(-1,0,0),B3D(-1,0,-1),B3D(0,0,0)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(0,-2,0),B3D(0,2,0)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(0,-2,0),B3D(-1,1,0)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(0,-2,0),B3D(-1,0,0)),
	PieceDef(5,B3D(-1,0,0),B3D(-1,1,0),B3D(0,-1,0),B3D(0,-2,0)),
	PieceDef(5,B3D(0,1,0),B3D(-1,1,0),B3D(-2,1,0),B3D(0,-1,0)), //special case, it need to move right by 1!
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,1,0),B3D(-1,0,0)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,1,0),B3D(-1,-1,0)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,1,0),B3D(1,-1,0)),
//20
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,1,0),B3D(1,0,0)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,1,0),B3D(1,1,0)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,1,0),B3D(0,1,-1)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,1,0),B3D(-1,1,-1)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,-1,0),B3D(-1,-1,-1)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1, 1,0),B3D(0,0,-1)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,-1,0),B3D(0,0,-1)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1, 1,0),B3D(0,-1,-1)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,-1,0),B3D(0, 1,-1)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,0,0),B3D(1,0,0)),
//30
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,0,0),B3D(0,0,-1)),
	PieceDef(5,B3D(0,1,0),B3D(0,-1,0),B3D(-1,0,0),B3D(-1,0,-1)),
	PieceDef(5,B3D(0,1,0),B3D(-1,0,0),B3D(-1,-1,0),B3D(1,1,0)),
	PieceDef(5,B3D(0,1,0),B3D(-1,0,0),B3D(-1,-1,0),B3D(0,1,-1)),
	PieceDef(5,B3D(0,-1,0),B3D(-1,0,0),B3D(-1,1,0),B3D(-1,1,-1)),
	PieceDef(5,B3D(0,1,0),B3D(-1,0,0),B3D(-1,-1,0),B3D(0,0,-1)),
	PieceDef(5,B3D(0,-1,0),B3D(-1,0,0),B3D(-1,1,0),B3D(0,0,-1)),
	PieceDef(5,B3D(0,1,0),B3D(-1,0,0),B3D(-1,1,0),B3D(0,0,-1)),
	PieceDef(5,B3D(0,1,0),B3D(-1,0,0),B3D(-1,0,-1),B3D(-1,-1,-1)),
	PieceDef(5,B3D(0,1,0),B3D(0,1,-1),B3D(-1,0,0),B3D(-1,0,-1)),
//40
	PieceDef(5,B3D(-1,0,0),B3D(-1,1,0),B3D(0,0,-1),B3D(0,-1,-1))};
