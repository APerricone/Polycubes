#include "hole.h"
#include "game.h"
#include "systemInterface.h"

#include <float.h>

//#define GETTIME
#define N_COLOR 5
#define ENLARGE 1/100.f

extern const Color colorArray[N_COLOR];

const Color colorArray[N_COLOR]= {
	Color( 32,240, 32,255),
	Color( 32,136,136,255),
	Color( 32, 32,240,255),
	Color(136, 32,136,255),
	Color(240, 32, 32,255)};

//HOLE
void Hole::Deinit()
{
	SAFE_DELETE_ARRAY(holeFaces[0]);
	SAFE_DELETE_ARRAY(holeFaces[1]);
	SAFE_DELETE_ARRAY(holeFaces[2]);
	SAFE_DELETE_ARRAY(holeFaces[3]);
	SAFE_DELETE_ARRAY(holeFaces[4]);
	SAFE_DELETE_ARRAY(bitsTmp);
	SAFE_DELETE_TEX(holeTex[0]);
	SAFE_DELETE_TEX(holeTex[1]);
	SAFE_DELETE_TEX(holeTex[2]);
	SAFE_DELETE_TEX(holeTex[3]);
	SAFE_DELETE_TEX(holeTex[4]);
	SAFE_DELETE_ARRAY(cubes);
	SAFE_DELETE_ARRAY(xPlanes);
	SAFE_DELETE_ARRAY(yPlanes);
	SAFE_DELETE_ARRAY(zPlanes);
	SAFE_DELETE_ARRAY(grid[0]);
	SAFE_DELETE_ARRAY(grid[1]);
	SAFE_DELETE_ARRAY(grid[2]);
	SAFE_DELETE_ARRAY(grid[3]);
	SAFE_DELETE_ARRAY(grid[4]);
	radMan.DelAllFaces();

}


void Hole::Init(
	unsigned char _sideA,
	unsigned char _sideB,
	unsigned char _depth)
{
	radMan.SetParent(this);
	//Set dimensions
	//every side need a space of 3*scale
	// considering the screen 2*RenderDimX/RenderDimY x 2
	//the side is:
	/*sideA=_sideA+4; sideB=_sideB;
	Game *g=Game::Get();
	if( sideA*g->RenderDimY>
		sideB*g->RenderDimX)
	{
		scale=2.f*g->RenderDimX/(g->RenderDimY*sideA);
	} else
	{
		scale=2.f/sideB;
	}*/
	sideA=_sideA; sideB=_sideB; depth=_depth;
	nCube=
		((unsigned long)(sideA+2))*
		((unsigned long)(sideB+2))*
		((unsigned long)(depth+2));
	inf.x=-(sup.x=sideA/2.f);
	inf.y=-(sup.y=sideB/2.f);
	sup.z=-1;
	inf.z=-1-(float)depth;
	V3D mys=sup+V3D(ENLARGE,ENLARGE,ENLARGE);
	V3D myi=inf-V3D(ENLARGE,ENLARGE,ENLARGE);
	holeVertices[2][0].pos.Set(mys.x,myi.y,mys.z);
	holeVertices[2][1].pos.Set(mys.x,mys.y,mys.z);
	holeVertices[2][2].pos.Set(mys.x,myi.y,myi.z);
	holeVertices[2][3].pos.Set(mys.x,mys.y,myi.z);
	holeVertices[3][0].pos.Set(myi.x,myi.y,mys.z);
	holeVertices[3][1].pos.Set(myi.x,mys.y,mys.z);
	holeVertices[3][2].pos.Set(myi.x,myi.y,myi.z);
	holeVertices[3][3].pos.Set(myi.x,mys.y,myi.z);
	holeVertices[0][0].pos.Set(myi.x,mys.y,mys.z);
	holeVertices[0][1].pos.Set(mys.x,mys.y,mys.z);
	holeVertices[0][2].pos.Set(myi.x,mys.y,myi.z);
	holeVertices[0][3].pos.Set(mys.x,mys.y,myi.z);
	holeVertices[1][0].pos.Set(myi.x,myi.y,mys.z);
	holeVertices[1][1].pos.Set(mys.x,myi.y,mys.z);
	holeVertices[1][2].pos.Set(myi.x,myi.y,myi.z);
	holeVertices[1][3].pos.Set(mys.x,myi.y,myi.z);
	holeVertices[4][0].pos.Set(myi.x,myi.y,myi.z);
	holeVertices[4][1].pos.Set(mys.x,myi.y,myi.z);
	holeVertices[4][2].pos.Set(myi.x,mys.y,myi.z);
	holeVertices[4][3].pos.Set(mys.x,mys.y,myi.z);
	//Imposto le tex
	texSize[0][0]=sideA*radMan.nDivision;
	texSize[0][1]=depth*radMan.nDivision;
	texSize[1][0]=sideB*radMan.nDivision;
	texSize[1][1]=depth*radMan.nDivision;
	texSize[2][0]=sideA*radMan.nDivision;
	texSize[2][1]=sideB*radMan.nDivision;
	unsigned char i=0;
	for(i=0;i<6;i++)
	{
#define TRY(v) if(texSize[i/2][i%2]<=v) texSize[i/2][i%2]=v; else
		TRY(16) TRY(32) TRY(64) TRY(128) TRY(256) TRY(512) TRY(1024)
			message("eh?","lato troppo grande");
#undef TRY
	}
#ifndef NOGL
	glGenTextures(5,holeTex);
	glBindTexture(GL_TEXTURE_2D,holeTex[0]);
	glTexImage2D(GL_TEXTURE_2D,0,3,texSize[0][0],texSize[0][1],0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D,holeTex[1]);
	glTexImage2D(GL_TEXTURE_2D,0,3,texSize[0][0],texSize[0][1],0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D,holeTex[2]);
	glTexImage2D(GL_TEXTURE_2D,0,3,texSize[1][0],texSize[1][1],0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D,holeTex[3]);
	glTexImage2D(GL_TEXTURE_2D,0,3,texSize[1][0],texSize[1][1],0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D,holeTex[4]);
	glTexImage2D(GL_TEXTURE_2D,0,3,texSize[2][0],texSize[2][1],0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
#endif
	unsigned long max=texSize[0][0]*texSize[0][1],ck;
	ck=texSize[1][0]*texSize[1][1]; if(ck>max) max=ck;
	ck=texSize[2][0]*texSize[2][1]; if(ck>max) max=ck;
	bitsTmp=new unsigned char[max*3];
	{
		float v[3][2];
		float z[3][2];
		z[0][0]=0.5f/(float)texSize[0][0];
		z[0][1]=0.5f/(float)texSize[0][1];
		z[1][0]=0.5f/(float)texSize[1][0];
		z[1][1]=0.5f/(float)texSize[1][1];
		z[2][0]=0.5f/(float)texSize[2][0];
		z[2][1]=0.5f/(float)texSize[2][1];

		v[0][0]=(float)(sideA*radMan.nDivision)/(float)texSize[0][0]-z[0][0];
		v[0][1]=(float)(depth*radMan.nDivision)/(float)texSize[0][1]-z[0][1];
		v[1][0]=(float)(sideB*radMan.nDivision)/(float)texSize[1][0]-z[1][0];
		v[1][1]=(float)(depth*radMan.nDivision)/(float)texSize[1][1]-z[1][1];
		v[2][0]=(float)(sideA*radMan.nDivision)/(float)texSize[2][0]-z[2][0];
		v[2][1]=(float)(sideB*radMan.nDivision)/(float)texSize[2][1]-z[2][1];

		holeVertices[0][0].tex.Set(v[0][0],z[0][1]);
		holeVertices[0][1].tex.Set(z[0][0],z[0][1]);
		holeVertices[0][2].tex.Set(v[0][0],v[0][1]);
		holeVertices[0][3].tex.Set(z[0][0],v[0][1]);
		holeVertices[1][0].tex.Set(z[0][0],z[0][1]);
		holeVertices[1][1].tex.Set(v[0][0],z[0][1]);
		holeVertices[1][2].tex.Set(z[0][0],v[0][1]);
		holeVertices[1][3].tex.Set(v[0][0],v[0][1]);
		holeVertices[2][0].tex.Set(z[1][0],z[1][1]);
		holeVertices[2][1].tex.Set(v[1][0],z[1][1]);
		holeVertices[2][2].tex.Set(z[1][0],v[1][1]);
		holeVertices[2][3].tex.Set(v[1][0],v[1][1]);
		holeVertices[3][0].tex.Set(v[1][0],z[1][1]);
		holeVertices[3][1].tex.Set(z[1][0],z[1][1]);
		holeVertices[3][2].tex.Set(v[1][0],v[1][1]);
		holeVertices[3][3].tex.Set(z[1][0],v[1][1]);
		holeVertices[4][0].tex.Set(z[2][0],z[2][1]);
		holeVertices[4][1].tex.Set(v[2][0],z[2][1]);
		holeVertices[4][2].tex.Set(z[2][0],v[2][1]);
		holeVertices[4][3].tex.Set(v[2][0],v[2][1]);
	}
	//aggiungo le facce al radiosity
	holeFaces[0]=new RadiosityMan::BOFace*[sideA*depth];
	holeFaces[1]=new RadiosityMan::BOFace*[sideA*depth];
	holeFaces[2]=new RadiosityMan::BOFace*[sideB*depth];
	holeFaces[3]=new RadiosityMan::BOFace*[sideB*depth];
	holeFaces[4]=new RadiosityMan::BOFace*[sideA*sideB];
	unsigned char x,y;
	V3D center;
	for(x=0;x<sideA;x++)
		for(y=0;y<depth;y++)
		{
			center=Get3D(B3D(x,0,y));
			center.y=sup.y-0.01f;
			holeFaces[0][(sideA-1-x)+y*sideA]=radMan.AddFace(center,V3D(0,-1,0),cWhite,1);
			center.y=inf.y+0.01f;
			holeFaces[1][x+y*sideA]=radMan.AddFace(center,V3D(0, 1,0),cWhite,1);
		}
	for(x=0;x<sideB;x++)
		for(y=0;y<depth;y++)
		{
			center=Get3D(B3D(0,x,y));
			center.x=sup.x-0.01f;
			holeFaces[2][x+y*sideB]=radMan.AddFace(center,V3D(-1,0,0),cWhite,1);
			center.x=inf.x+0.01f;
			holeFaces[3][(sideB-1-x)+y*sideB]=radMan.AddFace(center,V3D( 1,0,0),cWhite,1);
		}
	for(x=0;x<sideA;x++)
		for(y=0;y<sideB;y++)
		{
			center=Get3D(B3D(x,y,0));
			center.z=inf.z+0.01f;
			holeFaces[4][x+y*sideA]=radMan.AddFace(center,V3D(0,0,1),cWhite,1);
		}
	cubes=new BOCube[(sideA+2)*(sideB+2)*(depth+2)];
	BOCube *curr;
	unsigned char z;
	for(x=0;x<sideA+2;x++)
		for(y=0;y<sideB+2;y++)
			for(z=0;z<depth+2;z++)
			{
				curr=&cubes[x+(y+z*(sideB+2))*(sideA+2)];
				curr->on=0;
				if( (x==0)||(x==sideA+1)||
					(y==0)||(y==sideB+1))
					curr->on=2;
				if((z==0)||(z==depth+1))
					curr->on=3;
				curr->p.x=x-1;
				curr->p.y=y-1;
				curr->p.z=z-1;
				curr->center=Get3D(curr->p);
				curr->inf=curr->center-V3D(0.5f,0.5f,0.5f);
				curr->sup=curr->center+V3D(0.5f,0.5f,0.5f);
				curr->color=colorArray[(z%N_COLOR)];
				//curr->color=cWhite;
			}
	nLineInGrid[0]=nLineInGrid[1]=(sideB+depth)*2;
	nLineInGrid[2]=nLineInGrid[3]=(sideA+depth)*2;
	nLineInGrid[4]=(sideA+sideB)*2;
	// total=(sideA+sideB)*6+depth*4...bho
	for(x=0;x<5;x++)
	{
		grid[x]=new V3D[nLineInGrid[x]];
		nLineInGrid[x]=0;
	}
	float v;
	for(x=1;x<sideA;x++)
	{
		v=(x-sideA/2.f);
		grid[0][nLineInGrid[0]++].Set(v,sup.y,sup.z);
		grid[0][nLineInGrid[0]++].Set(v,sup.y,inf.z);
		grid[1][nLineInGrid[1]++].Set(v,inf.y,sup.z);
		grid[1][nLineInGrid[1]++].Set(v,inf.y,inf.z);
		grid[4][nLineInGrid[4]++].Set(v,sup.y,inf.z);
		grid[4][nLineInGrid[4]++].Set(v,inf.y,inf.z);
	}
	for(x=1;x<sideB;x++)
	{
		v=(x-sideB/2.f);
		grid[2][nLineInGrid[2]++].Set(sup.x,v,sup.z);
		grid[2][nLineInGrid[2]++].Set(sup.x,v,inf.z);
		grid[3][nLineInGrid[3]++].Set(inf.x,v,sup.z);
		grid[3][nLineInGrid[3]++].Set(inf.x,v,inf.z);
		grid[4][nLineInGrid[4]++].Set(sup.x,v,inf.z);
		grid[4][nLineInGrid[4]++].Set(inf.x,v,inf.z);
	}
	for(x=1;x<depth;x++)
	{
		v=-1.f-((float)x);
		grid[0][nLineInGrid[0]++].Set(sup.x,sup.y,v);
		grid[0][nLineInGrid[0]++].Set(inf.x,sup.y,v);
		grid[1][nLineInGrid[1]++].Set(sup.x,inf.y,v);
		grid[1][nLineInGrid[1]++].Set(inf.x,inf.y,v);
		grid[2][nLineInGrid[2]++].Set(sup.x,sup.y,v);
		grid[2][nLineInGrid[2]++].Set(sup.x,inf.y,v);
		grid[3][nLineInGrid[3]++].Set(inf.x,sup.y,v);
		grid[3][nLineInGrid[3]++].Set(inf.x,inf.y,v);
	}

	InitCollision();
	if((_sideA==5)&&(_sideB==7)&&(_depth==2))
		return;
	UpdateAll();
}

void Hole::InitForPreview()
{
	Init(5,7,2);
	unsigned char i,x,y,mx;
	for(i=0;i<4;i++)
	{
		SAFE_DELETE_TEX(holeTex[i]);
		//delete faces for radiosity
		if(i<2) mx=sideA; else mx=sideB;
		for(x=0;x<mx;x++)
			for(y=0;y<depth;y++)
				radMan.DelFace(holeFaces[i][x+y*mx]);
		SAFE_DELETE_ARRAY(holeFaces[i]);
	}
	unsigned long j;
	for(j=0;j<nCube;j++)
		if(cubes[j].on==2)
			cubes[j].on=3;

//*
	for(x=0;x<5;x++)
	{
		SAFE_DELETE(grid[x]);
		nLineInGrid[x]=0;
	}//*/
}

void Hole::SetPreview(unsigned char piece,bool calcRad)
{
	char i,j;
	BOCube *c[5];
	Empty();
	BMatrix bm; bm.p.Set(2,3,1);
	if(piece==16) bm.p.x=3;
	c[0]=GetBOCube(bm.p);
	for(i=0;i<colletion[piece].nPiece-1;i++)
		c[i+1]=GetBOCube(colletion[piece].dirs[i]*bm);
	B3D d;
	for(i=0;i<colletion[piece].nPiece;i++)
	{
		c[i]->on=true;
		c[i]->color=cWhite; //colorArray[((c[i]->p.x+c[i]->p.y)%2)+(c[i]->p.z%2)*2];
		c[i]->unionXpos=c[i]->unionXneg=
		c[i]->unionYpos=c[i]->unionYneg=
		c[i]->unionZpos=c[i]->unionZneg=false;
		for(j=0;j<colletion[piece].nPiece;j++)
			if(j!=i)
			{
				d=c[j]->p-c[i]->p;
				if((abs(d.x)+abs(d.y)+abs(d.z))==1)
				{
					if(d.x== 1) c[i]->unionXpos=true;
					if(d.x==-1) c[i]->unionXneg=true;
					if(d.y== 1) c[i]->unionYpos=true;
					if(d.y==-1) c[i]->unionYneg=true;
					if(d.z==-1) c[i]->unionZpos=true;
					if(d.z== 1) c[i]->unionZneg=true;
				}
			}
	}
	if(calcRad)
		UpdateAll();
}

void Hole::UpdateAll(bool calcRad)
{
#ifdef GETTIME
	DWORD t1=GetTickCount();
#endif
	onCubes=0;
	unsigned long i;
	BOCube *c=cubes;
	for(i=0;i<nCube;i++)
	{
		c->UpdateGeom(this);
		if(c->on==1)
		{
			c->next=onCubes;
			onCubes=c;
		}
		c++;
	}
	UpdateCollision();
	if(calcRad) radMan.MakeRadiosity(true);
	UpdateTexs();
#ifdef GETTIME
	DWORD t2=GetTickCount();
	message("profile","tempo di radiosity: %i msec",t2-t1);
#endif
}

void Hole::UpdateTexs()
{
	BOCube *c=onCubes;
	while(c)
	{
		c->UpdateTex(this);
		c=c->next;
	}

	unsigned short fx,fy,tx,ty,nfx,nfy,dimX,dimY,u;
	unsigned char i,*b;
	Color p;
	for(i=0;i<5;i++)
		if(holeTex[i])
		{
			switch(i)
			{
			case 0: case 1: nfx=sideA; nfy=depth; dimX=texSize[0][0]; dimY=texSize[0][1]; break;
			case 2: case 3: nfx=sideB; nfy=depth; dimX=texSize[1][0]; dimY=texSize[1][1]; break;
			case 4:			nfx=sideA; nfy=sideB; dimX=texSize[2][0]; dimY=texSize[2][1]; break;
			}
			V3D v;
			RadiosityMan::BOFace *f;
			memset(bitsTmp,0,dimX*dimY*3);
			for(fy=0;fy<nfy;fy++)
				for(fx=0;fx<nfx;fx++)
				{
					f=holeFaces[i][fx+fy*nfx];
					for(ty=0;ty<radMan.nDivision;ty++)
						for(tx=0;tx<radMan.nDivision;tx++)
						{
							b=&bitsTmp[(tx+fx*radMan.nDivision+(ty+fy*radMan.nDivision)*dimX)*3];
							u=	f->pixelDirect[tx][ty].r+
								f->pixelBounce[tx/PIXEL_STEP][ty/PIXEL_STEP].r;
							if(u<255) p.r=(unsigned char)u; else p.r=255;
							u=f->pixelDirect[tx][ty].g+
								f->pixelBounce[tx/PIXEL_STEP][ty/PIXEL_STEP].g;
							if(u<255) p.g=(unsigned char)u; else p.g=255;
							u=	f->pixelDirect[tx][ty].b+
								f->pixelBounce[tx/PIXEL_STEP][ty/PIXEL_STEP].b;
							if(u<255) p.b=(unsigned char)u; else p.b=255;
							b[0]=p.r; if(b[0]<32) b[0]=32;
							b[1]=p.g; if(b[1]<32) b[1]=32;
							b[2]=p.b; if(b[2]<32) b[2]=32;
	/*
							v=f->positions[tx-1][ty-1];
							b[0]=(v.x+3)*255.f/6;
							b[1]=(v.y+3)*255.f/6;
							b[2]=(-1-v.z)*255.f/6;*/
						}
				}

	#ifndef NOGL
			glBindTexture(GL_TEXTURE_2D,holeTex[i]);
			glTexSubImage2D(
				GL_TEXTURE_2D,0,0,0,dimX,dimY,
				GL_RGB,GL_UNSIGNED_BYTE,bitsTmp);
	#endif
		}

}

void Hole::Draw(bool xpos,bool ypos,bool xneg,bool yneg,bool drawCubes)
{
#ifndef NOGL
	unsigned char i,x,y,z;
	if(drawCubes)
	{
		for(x=1;x<=sideA;x++)
			for(y=1;y<=sideB;y++)
				for(z=1;z<=depth;z++)
					cubes[x+(y+z*(sideB+2))*(sideA+2)].Draw();
	}
	//*
	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );
	bool draw[5]={ ypos,yneg,xpos,xneg,true};
	for(i=0;i<5;i++)
		if(draw[i])
		{
			glBindTexture(GL_TEXTURE_2D,holeTex[i]);
			glVertexPointer(3,GL_FLOAT,sizeof(DynamicMesh::Vertex),&holeVertices[i]->pos);
			glTexCoordPointer(2,GL_FLOAT,sizeof(DynamicMesh::Vertex),&holeVertices[i]->tex);
			glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		}
		//*

//	glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
//	glColor3f(1,1,1);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
	glColor3f(0,0,0);
	for(i=0;i<5;i++)
		if(draw[i])
		{
			glVertexPointer(3,GL_FLOAT,sizeof(V3D),grid[i]);
			glDrawArrays(GL_LINES,0,nLineInGrid[i]);
		}//*/
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
#endif
}

BOCube *Hole::GetBOCube(const B3D &p)
{
	return &cubes[(p.x+1)+((p.y+1)+(p.z+1)*(sideB+2))*(sideA+2)];
}
void Hole::Empty()
{
	BOCube *c=cubes;
	unsigned long i;
	for(i=0;i<nCube;i++)
	{
		if(c->on<2) c->on=0;
		c->UpdateGeom(this);
		c++;
	}
	onCubes=0;
}

V3D Hole::Get3D(const B3D &p)
{
	return V3D(
		(p.x-sideA/2.f+0.5f),
		(p.y-sideB/2.f+0.5f),
		//-1.f-depth+p.z+0.5f);
		-1.f-p.z-0.5f);
}

B3D Hole::Revert3D(const V3D &p)
{
	B3D t;
	float v;
	v=p.x+sideA/2.f; t.x=(char)v;
	v=p.y+sideB/2.f; t.y=(char)v;
	v=(-p.z-1);		 t.z=(char)v;
	if(v<0) t.z=-1;
	return t;
}

bool Hole::Check(const B3D &p)
{
	if(p.x<0)				return false;
	if(p.x>=sideA)			return false;
	if(p.y<0)				return false;
	if(p.y>=sideB)			return false;
	if(p.z<0)				return false;
	if(p.z>=depth)			return false;
	if(GetBOCube(p)->on)	return false;
	return true;
}

unsigned char Hole::CheckForPlanes()
{
	B3D p;
	unsigned char planeToDelete;
	unsigned char nBoxIn;
	unsigned char totalInPlane=sideA*sideB;
	for(p.z=depth-1;p.z>=0;p.z--)
	{
		nBoxIn=0;
		for(p.x=0;p.x<sideA;p.x++)
			for(p.y=0;p.y<sideB;p.y++)
			{
				if(GetBOCube(p)->on)
					nBoxIn++;
			}
		if(nBoxIn==totalInPlane)
		{
			planeToDelete=p.z;
			break;
		}
	}
	if(p.z>=0)
	{
		B3D p2;
		for(;p.z>0;p.z--)
		{
			for(p.x=0;p.x<sideA;p.x++)
				for(p.y=0;p.y<sideB;p.y++)
				{
					p2=p; p2.z--;
					GetBOCube(p)->flags=GetBOCube(p2)->flags;
					SAFE_DELETE(GetBOCube(p)->mesh);
					GetBOCube(p)->unionZneg=false;
					if(p.z!=depth-1)
					{
						p2.z=p.z+1;
						if(GetBOCube(p2)->unionZpos)
						{
							GetBOCube(p2)->unionZpos=false;
							SAFE_DELETE(GetBOCube(p2)->mesh);
						}
					}
				}
		}
		p.z=0;
		for(p.x=0;p.x<sideA;p.x++)
			for(p.y=0;p.y<sideB;p.y++)
			{
				GetBOCube(p)->on=false;
			}
		UpdateAll();
		return 1;
	}
	return 0;
}

#ifdef USE_COMPRESS
#pragma comment(lib,"zlibwapi.lib")
#endif

void Hole::Load(const char *nameFile)
{
#ifdef USE_COMPRESS
	gzFile f=gzopen(nameFile,"rb");
#else
	FILE *f=fopen(nameFile,"rb");
#endif
	if(f==0)
	{
		message("Hole::Load","non posso aprire in letura il file %s",nameFile);
		return;
	}
	if((sideA!=0)||(sideB!=0)||(depth!=0))
		Deinit();
	InitForPreview();

	unsigned short dimX,dimY;
	unsigned char i=0;
	for(i=0;i<5;i++)
		if(holeTex[i])
		{
			switch(i)
			{
			case 0: case 1: dimX=texSize[0][0]; dimY=texSize[0][1]; break;
			case 2: case 3: dimX=texSize[1][0]; dimY=texSize[1][1]; break;
			case 4:			dimX=texSize[2][0]; dimY=texSize[2][1]; break;
			}

			glBindTexture(GL_TEXTURE_2D,holeTex[i]);
			READ(bitsTmp,dimX*dimY*3,f)
			glTexSubImage2D(
				GL_TEXTURE_2D,0,0,0,dimX,dimY,
				GL_RGB,GL_UNSIGNED_BYTE,bitsTmp);

		}
	unsigned char n;
	READ(&n,1,f)
	B3D p;
	for(i=0;i<n;i++)
	{
		READ(&p.x,1,f)
		READ(&p.y,1,f)
		READ(&p.z,1,f)
		GetBOCube(p)->Load(f,this);
	}
#ifdef USE_COMPRESS
	gzclose(f);
#else
	fclose(f);
#endif
};

void Hole::Save(const char *nameFile)
{
	if((sideA!=5)||(sideB!=7)||(depth!=2))
	{
		message("Hole::Save","I can save only preview holes");
		return;
	}
#ifdef USE_COMPRESS
	gzFile f=gzopen(nameFile,"wb");
#else
	FILE *f=fopen(nameFile,"wb");
#endif
	if(f==0)
	{
		message("Hole::Save","I can't write file %s",nameFile);
		return;
	}

	unsigned short dimX,dimY;
	unsigned char i=0;
	for(i=0;i<5;i++)
		if(holeTex[i])
		{
			if(i!=4)
			{
				message("Hole::Save","I can save only preview holes, and they don't have the side %i",i);
				return;
			}
			switch(i)
			{
			case 0: case 1: dimX=texSize[0][0]; dimY=texSize[0][1]; break;
			case 2: case 3: dimX=texSize[1][0]; dimY=texSize[1][1]; break;
			case 4:			dimX=texSize[2][0]; dimY=texSize[2][1]; break;
			}

			glBindTexture(GL_TEXTURE_2D,holeTex[i]);
			glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,bitsTmp);
			WRITE(bitsTmp,dimX*dimY*3,f);

		}

	BOCube *c=onCubes; i=0;
	while(c) { i++; c=c->next; }
	WRITE(&i,1,f);
	c=onCubes;
	while(c) { c->Save(f,this); c=c->next; }
#ifdef USE_COMPRESS
	gzclose(f);
#else
	fclose(f);
#endif
}
