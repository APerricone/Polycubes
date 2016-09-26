#include "hole.h"
#include "game.h"
#include "systemInterface.h"

#include <float.h>

BOCube::BOCube()
{
	faces[0]=faces[1]=faces[2]=faces[3]=faces[4]=faces[5]=0; tex=0;
	on=0;
	unionXpos=false;
	unionXneg=false;
	unionYpos=false;
	unionYneg=false;
	unionZpos=false;
	unionZneg=false;
	next=0;
	mesh=0;
	wire=0;
}

BOCube::~BOCube()
{
	SAFE_DELETE_TEX(tex);
	SAFE_DELETE(mesh);
	SAFE_DELETE(wire);
}

void BOCube::AddFace(V3D ce,V3D d1,V3D d2,float minu,float minv,float maxu,float maxv)
{
	DynamicMesh::Vertex a,b,c,d;
	a.Set(ce-d1-d2,V2D(minu,minv));
	b.Set(ce+d1-d2,V2D(maxu,minv));
	c.Set(ce-d1+d2,V2D(minu,maxv));
	d.Set(ce+d1+d2,V2D(maxu,maxv));
	mesh->AddTrianglePos(a,b,c);
	mesh->AddTrianglePos(c,b,d);
}
void BOCube::AddEdge(
		V3D p1,V2D t1,V3D p2,V2D t2,
		V3D d1,V2D dt1,V3D d2,V2D dt2,bool round)
{
	DynamicMesh::Vertex a,b,c,d;
	a.Set(p1+d1,t1+dt1);
	b.Set(p2+d1,t2+dt1);
	c.Set(p1+d2,t1+dt2);
	d.Set(p2+d2,t2+dt2);
	if(round)
	{
		mesh->AddTrianglePos(a,b,c);
		mesh->AddTrianglePos(c,b,d);
		wire->AddLine(
			p1+(d1+d2)*0.9f/2,
			p2+(d1+d2)*0.9f/2);
		return;
	}
	DynamicMesh::Vertex e,f,g,h;
	e.Set(p1,t1+dt2);
	f.Set(p2,t2+dt2);
	g.Set(p1,t1+dt1);
	h.Set(p2,t2+dt1);
	mesh->AddTrianglePos(a,b,g);
	mesh->AddTrianglePos(g,b,h);
	mesh->AddTrianglePos(e,f,c);
	mesh->AddTrianglePos(c,f,d);

	wire->AddLine(
		p1-(d1+d2)*0.1f/2,
		p2-(d1+d2)*0.1f/2);
}

void BOCube::AddCorner(V3D p,char dx,char dy,char dz,float delta,V2D t,V2D tx,V2D ty,V2D tz,
		bool xFill,bool yFill,bool zFill)
{
	DynamicMesh::Vertex a,b,c;
	a.Set(p+V3D(0,dy*delta,dz*delta),t+tx);
	b.Set(p+V3D(dx*delta,0,dz*delta),t+ty);
	c.Set(p+V3D(dx*delta,dy*delta,0),t+tz);
	if(!xFill && !yFill && !zFill)
	{
		mesh->AddTrianglePos(a,b,c);
		V3D m=p+(V3D(dx*delta,dy*delta,2*dz*delta))*0.9f/2;
		V3D n=p+(V3D(dx*delta,2*dy*delta,dz*delta))*0.9f/2;
		V3D o=p+(V3D(2*dx*delta,dy*delta,dz*delta))*0.9f/2;
		V3D q=(m+n+o)/3;
		wire->AddLine(m,q);
		wire->AddLine(n,q);
		wire->AddLine(o,q);
		return;
	}
	DynamicMesh::Vertex d,e,f;
	e.Set(p+V3D(0,dy*delta,0),t+tx);
	f.Set(p+V3D(0,0,dz*delta),t+tx);
	unsigned char n=0;
	if(xFill || (yFill && zFill))
	{
		mesh->AddTrianglePos(a,e,f);
		if(!yFill && !zFill)
		{
			e.tex=t+tz;
			f.tex=t+ty;
			mesh->AddTrianglePos(c,b,e);
			mesh->AddTrianglePos(e,b,f);

			e.pos=p+V3D(0,dy*delta,0)*0.9f;
			f.pos=p+V3D(0,0,dz*delta)*0.9f;
			V3D o=p+(V3D(2*dx*delta,dy*delta,dz*delta))*0.9f/2;
			wire->AddLine(e.pos,f.pos);
			wire->AddLine(o,(e.pos+f.pos)/2);
		}
		n++;
	}
	d.Set(p+V3D(dx*delta,0,0),t+ty);
	f.Set(p+V3D(0,0,dz*delta),t+ty);
	if(yFill || (xFill && zFill))
	{
		mesh->AddTrianglePos(b,d,f);
		if(!xFill && !zFill)
		{
			f.tex=t+tx;
			d.tex=t+tz;
			mesh->AddTrianglePos(a,c,f);
			mesh->AddTrianglePos(f,c,d);

			d.pos=p+V3D(dx*delta,0,0)*0.9f;
			f.pos=p+V3D(0,0,dz*delta)*0.9f;
			V3D n=p+(V3D(dx*delta,2*dy*delta,dz*delta))*0.9f/2;
			wire->AddLine(d.pos,f.pos);
			wire->AddLine(n,(d.pos+f.pos)/2);
		}
		n++;
	}
	d.Set(p+V3D(dx*delta,0,0),t+tz);
	e.Set(p+V3D(0,dy*delta,0),t+tz);
	if(zFill || (xFill && yFill))
	{
		mesh->AddTrianglePos(c,d,e);
		if(!xFill && !yFill)
		{
			e.tex=t+tx;
			d.tex=t+ty;
			mesh->AddTrianglePos(a,b,e);
			mesh->AddTrianglePos(e,b,d);

			d.pos=p+V3D(dx*delta,0,0)*0.9f;
			e.pos=p+V3D(0,dy*delta,0)*0.9f;
			V3D m=p+(V3D(dx*delta,dy*delta,2*dz*delta))*0.9f/2;
			wire->AddLine(d.pos,e.pos);
			wire->AddLine(m,(d.pos+e.pos)/2);
		}
		n++;
	}

	d.Set(p+V3D(dx*delta,0,0),t+ty+tz);
	e.Set(p+V3D(0,dy*delta,0),t+tx+tz);
	f.Set(p+V3D(0,0,dz*delta),t+tx+ty);
	if(n==3)
	{
		mesh->AddTrianglePos(d,e,f);

		d.pos=p+V3D(dx*delta,0,0)*0.9f;
		e.pos=p+V3D(0,dy*delta,0)*0.9f;
		f.pos=p+V3D(0,0,dz*delta)*0.9f;
		V3D m=(d.pos+e.pos+f.pos)/3;
		wire->AddLine(d.pos,m);
		wire->AddLine(e.pos,m);
		wire->AddLine(f.pos,m);

	}
}

void BOCube::UpdateGeom(Hole *h)
{
	unsigned char i;
	center=h->Get3D(p);
	inf=center-V3D(0.5f,0.5f,0.5f);
	sup=center+V3D(0.5f,0.5f,0.5f);
	unsigned long nDivision=h->radMan.nDivision;
	if(on==1)
	{
		if(tex==0)
		{
#ifdef NOGL
			tex=1;
#else
			glGenTextures(1,&tex);
			glBindTexture(GL_TEXTURE_2D,tex);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D,0,3,nDivision*4,nDivision*4,0,GL_RGB,GL_UNSIGNED_BYTE,0);
#endif
		}
		if(faces[0]==0) faces[0]=h->radMan.AddFace(center+V3D(0.501f,0,0),V3D( 1,0,0),color,0.9f);
		if(faces[1]==0) faces[1]=h->radMan.AddFace(center-V3D(0.501f,0,0),V3D(-1,0,0),color,0.9f);
		if(faces[2]==0) faces[2]=h->radMan.AddFace(center+V3D(0,0.501f,0),V3D(0, 1,0),color,0.9f);
		if(faces[3]==0) faces[3]=h->radMan.AddFace(center-V3D(0,0.501f,0),V3D(0,-1,0),color,0.9f);
		if(faces[4]==0) faces[4]=h->radMan.AddFace(center+V3D(0,0,0.501f),V3D(0,0, 1),color,0.9f);
		if(faces[5]==0) faces[5]=h->radMan.AddFace(center-V3D(0,0,0.501f),V3D(0,0,-1),color,0.9f);
		if(mesh==0)
		{
			wire=new DynamicCage();
			wire->Begin();
			mesh=new DynamicMesh();
			mesh->Begin();
#define T0B (0.125f/nDivision)
#define T1A 0.25f-T0B
#define T1B 0.25f+T0B
#define T2A 0.50f-T0B
#define T2B 0.50f+T0B
#define T3A 0.75f-T0B
#define T3B 0.75f+T0B
#define T4A 1-T0B
#define T4B 1+T0B
#define T1 0.25f
#define T2 0.50f
#define T3 0.75f
#define T4 1
			AddFace(V3D(sup.x,center.y,center.z),V3D(0,0,-0.4f),V3D(0,-0.4f,0),T2B,T1B,T3A,T2A);
			AddFace(V3D(inf.x,center.y,center.z),V3D(0,0, 0.4f),V3D(0,-0.4f,0),T0B,T1B,T1A,T2A);
			AddFace(V3D(center.x,sup.y,center.z),V3D( 0.4f,0,0),V3D(0,0, 0.4f),T1B,T0B,T2A,T1A);
			AddFace(V3D(center.x,inf.y,center.z),V3D( 0.4f,0,0),V3D(0,0,-0.4f),T1B,T2B,T2A,T3A);
			AddFace(V3D(center.x,center.y,sup.z),V3D( 0.4f,0,0),V3D(0,-0.4f,0),T1B,T1B,T2A,T2A);
			AddFace(V3D(center.x,center.y,inf.z),V3D(-0.4f,0,0),V3D(0,-0.4f,0),T3B,T1B,T4A,T2A);

			AddEdge(
				V3D(sup.x-0.1f,sup.y,sup.z),V2D(T2A,T1),
				V3D(inf.x+0.1f,sup.y,sup.z),V2D(T1B,T1),
				V3D(0,-0.1f,0),V2D(0, T0B),
				V3D(0,0,-0.1f),V2D(0,-T0B),!unionYpos && !unionZpos);
			AddEdge(
				V3D(sup.x-0.1f,inf.y,sup.z),V2D(T2A,T2),
				V3D(inf.x+0.1f,inf.y,sup.z),V2D(T1B,T2),
				V3D(0,0,-0.1f),V2D(0, T0B),
				V3D(0, 0.1f,0),V2D(0,-T0B),!unionYneg && !unionZpos);
			AddEdge(
				V3D(sup.x-0.1f,sup.y,inf.z),V2D(T3B,T1),
				V3D(inf.x+0.1f,sup.y,inf.z),V2D(T4A,T1),
				V3D(0,-0.1f,0),V2D(0, T0B),
				V3D(0,0, 0.1f),V2D(0,-T0B),!unionYpos && !unionZneg);
			AddEdge(
				V3D(sup.x-0.1f,inf.y,inf.z),V2D(T3B,T2),
				V3D(inf.x+0.1f,inf.y,inf.z),V2D(T4A,T2),
				V3D(0,0, 0.1f),V2D(0, T0B),
				V3D(0, 0.1f,0),V2D(0,-T0B),!unionYneg && !unionZneg);
			AddEdge(
				V3D(sup.x-0.1f,sup.y,sup.z),V2D(T2A,T1),
				V3D(inf.x+0.1f,sup.y,sup.z),V2D(T1B,T1),
				V3D(0,-0.1f,0),V2D(0, T0B),
				V3D(0,0,-0.1f),V2D(0,-T0B),!unionYpos && !unionZpos);

			AddEdge(
				V3D(sup.x,sup.y-0.1f,sup.z),V2D(T2,T1B),
				V3D(sup.x,inf.y+0.1f,sup.z),V2D(T2,T2A),
				V3D(0,0,-0.1f),V2D( T0B,0),
				V3D(-0.1f,0,0),V2D(-T0B,0),!unionXpos && !unionZpos);
			AddEdge(
				V3D(sup.x,sup.y-0.1f,inf.z),V2D(T3,T1B),
				V3D(sup.x,inf.y+0.1f,inf.z),V2D(T3,T2A),
				V3D(-0.1f,0,0),V2D( T0B,0),
				V3D(0,0, 0.1f),V2D(-T0B,0),!unionXpos && !unionZneg);
			AddEdge(
				V3D(inf.x,sup.y-0.1f,sup.z),V2D(T1,T1B),
				V3D(inf.x,inf.y+0.1f,sup.z),V2D(T1,T2A),
				V3D( 0.1f,0,0),V2D( T0B,0),
				V3D(0,0,-0.1f),V2D(-T0B,0),!unionXneg && !unionZpos);
			AddEdge(
				V3D(inf.x,sup.y-0.1f,inf.z),V2D(T4,T1B),
				V3D(inf.x,inf.y+0.1f,inf.z),V2D(T4,T2A),
				V3D(0,0, 0.1f),V2D( T0B,0),
				V3D( 0.1f,0,0),V2D(-T0B,0),!unionXneg && !unionZneg);

			AddEdge(
				V3D(sup.x,sup.y,sup.z-0.1f),V2D(T2B,T1),
				V3D(sup.x,sup.y,inf.z+0.1f),V2D(T3A,T1),
				V3D(0,-0.1f,0),V2D(0, T0B),
				V3D(-0.1f,0,0),V2D(0,-T0B),!unionXpos && !unionYpos);
			AddEdge(
				V3D(sup.x,inf.y,sup.z-0.1f),V2D(T2B,T2),
				V3D(sup.x,inf.y,inf.z+0.1f),V2D(T3A,T2),
				V3D(-0.1f,0,0),V2D(0, T0B),
				V3D(0, 0.1f,0),V2D(0,-T0B),!unionXpos && !unionYneg);
			AddEdge(
				V3D(inf.x,sup.y,sup.z-0.1f),V2D(T1A,T1),
				V3D(inf.x,sup.y,inf.z+0.1f),V2D(T0B,T1),
				V3D(0,-0.1f,0),V2D(0, T0B),
				V3D( 0.1f,0,0),V2D(0,-T0B),!unionXneg && !unionYpos);
			AddEdge(
				V3D(inf.x,inf.y,sup.z-0.1f),V2D(T1A,T2),
				V3D(inf.x,inf.y,inf.z+0.1f),V2D(T0B,T2),
				V3D( 0.1f,0,0),V2D(0, T0B),
				V3D(0, 0.1f,0),V2D(0,-T0B),!unionXneg && !unionYneg);

			AddCorner(
				V3D(sup.x,sup.y,sup.z),-1,-1,-1,0.1f,
				V2D(T2,T1B),V2D( T0B,0),V2D(0,-T0B*2),V2D(-T0B,0),unionXpos,unionYpos,unionZpos);
			AddCorner(
				V3D(inf.x,sup.y,sup.z), 1,-1,-1,0.1f,
				V2D(T1,T1B),V2D(-T0B,0),V2D(0,-T0B*2),V2D( T0B,0),unionXneg,unionYpos,unionZpos);
			AddCorner(
				V3D(sup.x,inf.y,sup.z),-1, 1,-1,0.1f,
				V2D(T2,T2A),V2D( T0B,0),V2D(0, T0B*2),V2D(-T0B,0),unionXpos,unionYneg,unionZpos);
			AddCorner(
				V3D(inf.x,inf.y,sup.z), 1, 1,-1,0.1f,
				V2D(T1,T2A),V2D(-T0B,0),V2D(0, T0B*2),V2D( T0B,0),unionXneg,unionYneg,unionZpos);
			AddCorner(
				V3D(sup.x,sup.y,inf.z),-1,-1, 1,0.1f,
				V2D(T3,T1B),V2D(-T0B,0),V2D(0,-T0B*2),V2D( T0B,0),unionXpos,unionYpos,unionZneg);
			AddCorner(
				V3D(inf.x,sup.y,inf.z), 1,-1, 1,0.1f,
				V2D(T4,T1B),V2D( T0B,0),V2D(0,-T0B*2),V2D(-T0B,0),unionXneg,unionYpos,unionZneg);
			AddCorner(
				V3D(sup.x,inf.y,inf.z),-1, 1, 1,0.1f,
				V2D(T3,T2A),V2D(-T0B,0),V2D(0, T0B*2),V2D( T0B,0),unionXpos,unionYneg,unionZneg);
			AddCorner(
				V3D(inf.x,inf.y,inf.z), 1, 1, 1,0.1f,
				V2D(T4,T2A),V2D( T0B,0),V2D(0, T0B*2),V2D(-T0B,0),unionXneg,unionYneg,unionZneg);
			mesh->End();
			wire->End();
		}
	} else
	{
		SAFE_DELETE_TEX(tex);
		for(i=0;i<6;i++)
		{
			if(faces[i]) h->radMan.DelFace(faces[i]);
			faces[i]=0;
		}
		SAFE_DELETE(mesh);
	}
}

void BOCube::UpdateTex(Hole *h)
{
	if(on!=1) return;
	unsigned short x,y,dim,tx,ty,sx,sy,u;
	unsigned char i,*b,*b2,*texb=h->cubeTexBits;
	Color p,min;
	unsigned long nDivision=h->radMan.nDivision;
	// tot col = (240+32+32)*x/255=96 x=96*255/304=81
	min.r=(unsigned char)(81*color.r/255.f);
	min.g=(unsigned char)(81*color.g/255.f);
	min.b=(unsigned char)(81*color.b/255.f);
	/*if(((int)min.r+(int)min.g+(int)min.b)>50)
	{ //REACTIVATE WITH SAVECUBES
		min.r=min.g=min.b=32;
	}*/
	dim=nDivision*4;
	V3D v;
	for(x=0;x<nDivision;x++)
		for(y=0;y<nDivision;y++)
			for(i=0;i<6;i++)
			{
				u=	faces[i]->pixelDirect[x][y].r+
					faces[i]->pixelBounce[x/PIXEL_STEP][y/PIXEL_STEP].r;
				if(u<255) p.r=(unsigned char)u; else p.r=255;
				u=	faces[i]->pixelDirect[x][y].g+
					faces[i]->pixelBounce[x/PIXEL_STEP][y/PIXEL_STEP].g;
				if(u<255) p.g=(unsigned char)u; else p.g=255;
				u=	faces[i]->pixelDirect[x][y].b+
					faces[i]->pixelBounce[x/PIXEL_STEP][y/PIXEL_STEP].b;
				if(u<255) p.b=(unsigned char)u; else p.b=255;
				switch(i)
				{
				case 0: tx= nDivision*2+y ; ty=nDivision+x; break;
				case 1: tx= nDivision-1-y ; ty=nDivision*2-1-x; break;
				case 2: tx=nDivision+x; ty= nDivision -1-y; break;
				case 3: tx=nDivision*2-x-1; ty=nDivision*2+y; break;
				case 4: tx=  nDivision+x  ; ty= nDivision*2-1-y; break;
				case 5: tx=nDivision*4-1-x; ty= nDivision*2-1-y; break;
				}
				b=&texb[(tx+ty*dim)*3];
				b[0]=p.r; if(b[0]<min.r) b[0]=min.r;
				b[1]=p.g; if(b[1]<min.g) b[1]=min.g;
				b[2]=p.b; if(b[2]<min.b) b[2]=min.b;
/*
				v=faces[i]->positionsBounce[x/PIXEL_STEP][y/PIXEL_STEP];
				b[0]=(v.x+3)*255.f/6;
				b[1]=(v.y+3)*255.f/6;
				b[2]=(-1-v.z)*255.f/6;//*/
			}
	for(i=0;i<nDivision;i++)
	{
		//0
		tx=i; ty=nDivision-1;
		sx=nDivision; sy=i;
		 b=&texb[(sx+sy*dim)*3]; b2=&texb[(tx+ty*dim)*3];
		b2[0]=b[0]; b2[1]=b[1]; b2[2]=b[2];
		//1
		tx=nDivision*2+i; ty=nDivision-1;
		sx=nDivision*2-1; sy=nDivision-1-i;
		 b=&texb[(sx+sy*dim)*3]; b2=&texb[(tx+ty*dim)*3];
		b2[0]=b[0]; b2[1]=b[1]; b2[2]=b[2];
		//2
		tx=nDivision*3+i; ty=nDivision-1;
		sx=nDivision*2-1-i; sy=0;
		 b=&texb[(sx+sy*dim)*3]; b2=&texb[(tx+ty*dim)*3];
		b2[0]=b[0]; b2[1]=b[1]; b2[2]=b[2];
		//3
		tx=i; ty=nDivision*2;
		sx=nDivision; sy=nDivision*3-1-i;
		 b=&texb[(sx+sy*dim)*3]; b2=&texb[(tx+ty*dim)*3];
		b2[0]=b[0]; b2[1]=b[1]; b2[2]=b[2];
		//4
		tx=nDivision*2+i; ty=nDivision*2;
		sx=nDivision*2-1; sy=nDivision*2+i;
		 b=&texb[(sx+sy*dim)*3]; b2=&texb[(tx+ty*dim)*3];
		b2[0]=b[0]; b2[1]=b[1]; b2[2]=b[2];
		//5
		tx=nDivision*3+i; ty=nDivision*2;
		sx=nDivision*2-1-i; sy=nDivision*3-1;
		 b=&texb[(sx+sy*dim)*3]; b2=&texb[(tx+ty*dim)*3];
		b2[0]=b[0]; b2[1]=b[1]; b2[2]=b[2];
	}
#ifndef NOGL
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexSubImage2D(
		GL_TEXTURE_2D,0,0,0,dim,dim,
		GL_RGB,GL_UNSIGNED_BYTE,texb);
#endif
}

void BOCube::Draw()
{
	if(on!=1) return;
#ifndef NOGL
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex);
	glColor3f(1,1,1);
#endif
	if(mesh)
		mesh->Draw();
#ifndef NOGL
	glDisable(GL_TEXTURE_2D);
	glColor3f(0,0,0);
	if(wire)
		wire->Draw();
/*
	glDisable(GL_TEXTURE_2D);
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	if(unionYneg || unionZneg ) { glVertex3f(inf.x,inf.y,inf.z); glVertex3f(sup.x,inf.y,inf.z); }
	if(unionYpos || unionZneg ) { glVertex3f(inf.x,sup.y,inf.z); glVertex3f(sup.x,sup.y,inf.z); }
	if(unionYneg || unionZpos ) { glVertex3f(inf.x,inf.y,sup.z); glVertex3f(sup.x,inf.y,sup.z); }
	if(unionYpos || unionZpos ) { glVertex3f(inf.x,sup.y,sup.z); glVertex3f(sup.x,sup.y,sup.z); }

	if(unionXneg || unionZneg ) { glVertex3f(inf.x,inf.y,inf.z); glVertex3f(inf.x,sup.y,inf.z); }
	if(unionXpos || unionZneg ) { glVertex3f(sup.x,inf.y,inf.z); glVertex3f(sup.x,sup.y,inf.z); }
	if(unionXneg || unionZpos ) { glVertex3f(inf.x,inf.y,sup.z); glVertex3f(inf.x,sup.y,sup.z); }
	if(unionXpos || unionZpos ) { glVertex3f(sup.x,inf.y,sup.z); glVertex3f(sup.x,sup.y,sup.z); }

	if(unionXneg || unionYneg ) { glVertex3f(inf.x,inf.y,inf.z); glVertex3f(inf.x,inf.y,sup.z); }
	if(unionXpos || unionYneg ) { glVertex3f(sup.x,inf.y,inf.z); glVertex3f(sup.x,inf.y,sup.z); }
	if(unionXneg || unionYpos ) { glVertex3f(inf.x,sup.y,inf.z); glVertex3f(inf.x,sup.y,sup.z); }
	if(unionXpos || unionYpos ) { glVertex3f(sup.x,sup.y,inf.z); glVertex3f(sup.x,sup.y,sup.z); }
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
/*
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,0); glVertex2f(0,0);
	glTexCoord2f(1,0); glVertex2f(1,0);
	glTexCoord2f(0,1); glVertex2f(0,1);
	glTexCoord2f(1,1); glVertex2f(1,1);
	glEnd();//*/
#endif
}

void BOCube::Save(DFILE f,Hole *h)
{
	if(on==0) return;
	WRITE(&p.x,1,f);
	WRITE(&p.y,1,f);
	WRITE(&p.z,1,f);
	WRITE(&flags,2,f);
	WRITE(&color,4,f);

	unsigned char *texb=h->cubeTexBits;
	unsigned short dim=h->radMan.nDivision*4;

	glBindTexture(GL_TEXTURE_2D,tex);
	glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,texb);
	WRITE(texb,dim*dim*3,f);
};

void BOCube::Load(DFILE f,Hole *h)
{
	on=1;
	/*
	fread(&p.x,1,1,f);
	fread(&p.y,1,1,f);
	fread(&p.z,1,1,f);*/
	//This is on Hole::Load, so that Hole can know witch cube still load
	READ(&flags,2,f);
	READ(&color,4,f);
	UpdateGeom(h);
	unsigned char *texb=h->cubeTexBits;
	unsigned short dim=h->radMan.nDivision*4;
	READ(texb,dim*dim*3,f);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTexSubImage2D(
		GL_TEXTURE_2D,0,0,0,dim,dim,
		GL_RGB,GL_UNSIGNED_BYTE,texb);
};
