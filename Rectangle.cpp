#include "menuElements.h"
#include "Image.h"
#include "systeminterface.h"
#include "game.h"
#include <string.h>

#include "gl/glext.h"
#ifdef _WINDOWS
#define GET_PROC_ADDRESS(x) wglGetProcAddress(x)
#else
#define GET_PROC_ADDRESS(x) glXGetProcAddress(x)
#endif
#define INITEXT(tipo,nome) nome=(tipo)GET_PROC_ADDRESS(#nome);

//Vertex e fragment program
PFNGLBINDPROGRAMARBPROC glBindProgramARB = NULL;
PFNGLGENPROGRAMSARBPROC glGenProgramsARB = NULL;
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4fARB = NULL;
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB = NULL;
//multitexture
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;

void Rects2D::Init()
{
	const char *extension =(const char*)glGetString(GL_EXTENSIONS);
	fragProg=/*
		false;
		/*/
		(strstr(extension, "ARB_fragment_program")!=NULL);//*/
	if(fragProg)
	{
		INITEXT(PFNGLBINDPROGRAMARBPROC,glBindProgramARB);
		INITEXT(PFNGLGENPROGRAMSARBPROC,glGenProgramsARB);
		INITEXT(PFNGLPROGRAMSTRINGARBPROC,glProgramStringARB);
		INITEXT(PFNGLPROGRAMENVPARAMETER4FARBPROC,glProgramEnvParameter4fARB);
		INITEXT(PFNGLDELETEPROGRAMSARBPROC,glDeleteProgramsARB);
	}
	INITEXT(PFNGLACTIVETEXTUREARBPROC,glActiveTextureARB);
	INITEXT(PFNGLCLIENTACTIVETEXTUREARBPROC,glClientActiveTextureARB);

	txtFlat=new Image(); txtFill=new Image(); txtTube=new Image(); txtGlass=new Image();
		txtFlat->dimX=txtFlat->dimY=
		txtFill->dimX=txtFill->dimY=16;
	txtFlat->format=Image::GRAYALPHA;
	txtFill->format=Image::GRAYALPHA;

	txtTube->dimX=txtTube->dimY=32;
	txtTube->format=Image::RGBA;


	txtFlat->Create();
	txtFill->Create();
	txtTube->Create();

	txtGlass->dimX=txtGlass->dimY=32;
	txtGlass->format=Image::GRAYALPHA;
	txtGlass->Create();

	txtGlassShadow=new Image();
	txtGlassShadow->dimX=txtGlassShadow->dimY=32;
	txtGlassShadow->format=Image::GRAYALPHA;
	txtGlassShadow->Create();


	float dist;
	Color c=cBlack;
	int x,y;
	V2D center;
	V3D nor;
	for(y=0;y<16;y++)
		for(x=0;x<16;x++)
		{
			center.Set(x-7.5f,y-7.5f);
			dist=center.Normalize();

			if(dist>7.f) c.a=0; else
			if(dist>6.f) c.a=(6.f-dist)*255.f; else
			if(dist>2.f) c.a=255; else
			if(dist>1.f) c.a=(dist-2.f)*255; else
				c.a=0;
			txtFlat->SetPixel(x,y,c);

			c.Set(0,0,0,0);
			if(dist>7.f) c.a=0; else
			if(dist>6.f) c.a=(6.f-dist)*255.f; else
				c.a=255;
			txtFill->SetPixel(x,y,c);
		}
	for(y=0;y<32;y++)
		for(x=0;x<32;x++)
		{
			center.Set(x-15.5f,y-15.5f);
			dist=center.Normalize();
			center*=8.f;
			center+=V2D(15.5f,15.5f);

			nor.x=(center.x-x);
			nor.y=(center.y-y);
			nor.z=7*7-(dist-7)*(dist-7);
			if(nor.z<0) nor.z=0; else  nor.z=sqrtf(nor.z);
			if(dist>14.f) c.a=0; else
			if(dist>13.f) c.a=(13.f-dist)*255.f; else
			if(dist>2.f) c.a=255; else
			if(dist>1.f) c.a=(dist-1.f)*255; else c.a=0;
			nor.Normalize();
			c.r=128+127.f*nor.x;
			c.g=128+127.f*nor.y;
			c.b=128+127.f*nor.z;
			if(!fragProg)
			{
				c.r=c.g=c.b=0;
			}
			txtTube->SetPixel(x,y,c);

			c.r=c.g=c.b=0;
#define OPC 130
			if(fragProg)
			{
				if(dist<8.f)
				{
					if(dist>1.f)
						c.a=(dist-1.f)*(255-OPC)/7.f+OPC; 
					else 
						c.a=OPC;
				}
			} else
			{
				if(dist<2.f)
				{
					if(dist>1.f)
						c.a=(dist-1.f)*(255-OPC)+OPC; 
					else 
						c.a=OPC;
				}
			}
			if(c.a>(OPC-2)) c.a-=(OPC-2); else c.a=0;
			txtGlassShadow->SetPixel(x,y,c);
			c.r=c.g=c.b=rand()*255/RAND_MAX;
			c.a=rand()*255/RAND_MAX;
			txtGlass->SetPixel(x,y,c);
		}
	txtFlat->CreateTxt();
	txtFill->CreateTxt();
	txtTube->CreateTxt();
	txtGlass->CreateTxt();
	txtGlassShadow->CreateTxt();

#define ADDRECT(i,v1,v2,v3,v4) idx[i]=v1; idx[i+1]=v2; idx[i+2]=v3; idx[i+3]=v3; idx[i+4]=v2; idx[i+5]=v4;
	ADDRECT( 0, 0, 1, 6, 7);
	ADDRECT( 6, 1, 2, 7, 8);
	ADDRECT(12, 4, 5,10,11);
	ADDRECT(18, 6, 7,12,13);
	ADDRECT(24,10,11,14,15);
	ADDRECT(30,12,13,16,17);
	ADDRECT(36,13,14,17,18);
	ADDRECT(42,14,15,18,19);
	ADDRECT(48, 3, 4, 9,10);
#undef ADDRECT
#define ADDRECT(i,v1,v2,v3,v4) idxf[i]=v1; idxf[i+1]=v2; idxf[i+2]=v3; idxf[i+3]=v3; idxf[i+4]=v2; idxf[i+5]=v4;
	ADDRECT( 0, 0, 1, 6, 7);
	ADDRECT( 6, 1, 4, 7,10);
	ADDRECT(12, 4, 5,10,11);
	ADDRECT(18, 6, 7,12,13);
	ADDRECT(24, 7,10,13,14);
	ADDRECT(30,10,11,14,15);
	ADDRECT(36,12,13,16,17);
	ADDRECT(42,13,14,17,18);
	ADDRECT(48,14,15,18,19);
#undef ADDRECT
	v[ 2].t.Set(0.5f,0.0f); v[ 3].t.Set(0.5f,0.0f);
	v[ 8].t.Set(0.5f,0.5f); v[ 9].t.Set(0.5f,0.5f);
	v[ 0].t.Set(0.0f,0.0f); v[ 1].t.Set(0.5f,0.0f); v[ 4].t.Set(0.5f,0.0f); v[ 5].t.Set(1.0f,0.0f);
	v[ 6].t.Set(0.0f,0.5f); v[ 7].t.Set(0.5f,0.5f); v[10].t.Set(0.5f,0.5f); v[11].t.Set(1.0f,0.5f);
	v[12].t.Set(0.0f,0.5f); v[13].t.Set(0.5f,0.5f); v[14].t.Set(0.5f,0.5f); v[15].t.Set(1.0f,0.5f);
	v[16].t.Set(0.0f,1.0f); v[17].t.Set(0.5f,1.0f); v[18].t.Set(0.5f,1.0f); v[19].t.Set(1.0f,1.0f);

	if(fragProg)
	{
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &txtScreen);
		glBindTexture(GL_TEXTURE_2D, txtScreen);
		txtScreenDimX=256;
		if(Game::Get()->RenderDimX>=512) txtScreenDimX=512;
		if(Game::Get()->RenderDimX>=1024) txtScreenDimX=1024;
		txtScreenDimY=256;
		if(Game::Get()->RenderDimY>=512) txtScreenDimY=512;
		if(Game::Get()->RenderDimY>=1024) txtScreenDimY=1024;
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,txtScreenDimX,txtScreenDimY,	0,GL_RGB,GL_UNSIGNED_BYTE,0);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		char tubeProgam[]="!!ARBfp1.0\n"			//1
			"ATTRIB pos = fragment.position;\n"		//2
			"OUTPUT col = result.color;\n"			//3
			"PARAM mult  = program.env[0];\n"		//4
			"TEMP n; TEMP t; TEMP h; TEMP tl; \n"	//5
			"TEX n, fragment.texcoord[0], texture[0], 2D;\n"
			"MOV col.a,n.a;\n"						//7
			"MAD n.xyz,n,2,-1;\n"					//8
			"MUL t.xy,tl,mult;\n"					//9
			"SUB t.y, 1, t.y;\n"					//10
			"SUB t.x, t.x, 1;\n"					//11
			"MOV t.z,5;\n"							//12
			"ADD h,t,{1,1,2,0};\n"					//13
			"DP3 h.w,h,h;\n"						//14
			"RSQ h.w,h.w;\n"						//15
			"MUL h,h,h.w;\n"						//16
			"MOV tl,{1,0,1,30};\n"					//17
			"DP3 tl.y,h,n;\n"						//18
			"LIT tl,tl;\n"							//19
			"MOV col.rgb,tl.z;\n"					//20
			"END\n";								//21
		glGenProgramsARB(1, &tubeProg);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,tubeProg);
		glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(tubeProgam),tubeProgam);

		const char *error= (const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		if(strlen(error)!=0) message("Errore in compilazione shader Tubo","%s",error);

		char glassProgam[]="!!ARBfp1.0\n"			//1
			"ATTRIB pos = fragment.position;\n"		//2
			"OUTPUT col = result.color;\n"			//3
			"PARAM mult = program.env[0];\n"		//4
			"PARAM offset = program.env[1];\n"		//4
			"TEMP d;TEMP t;TEMP r;\n"				//5
			"TEX d, fragment.texcoord[0], texture[0], 2D;\n"
			"MOV d.y,d.w;\n"						//7
			"SUB d,d,0.5;\n"						//8
			"ADD r.xy,pos,offset;\n"				//9
			"MUL t.xy,r,mult;\n"					//10
			"MAD t.xy,d,0.02,t;\n"					//11
			"TEX r, t, texture[1], 2D;\n"			//12
			"MAD r,r,0.25,0.75;\n"					//13
			"MOV col.rgb,r;\n"						//14
			"MOV col.a,1;\n"						//15
			"END\n";								//16
		glGenProgramsARB(1, &glassProg);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,glassProg);
		glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(glassProgam),glassProgam);

		error= (const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		if(strlen(error)!=0) message("Errore in compilazione Vetro","%s",error);
	}
}

void Rects2D::Draw(float x0,float y0,float x3,float y3,float topHole1,float topHole2)
{
	float x1=x0+4,x2=x3-4;
	float y1=y0+4,y2=y3-4;
	if(x1>x2) { x1=x2=(x1+x2)/2; }
	if(y1>y2) { y1=y2=(y1+y2)/2; }
	if(topHole1<x1) topHole1=x1; 	 if(topHole1>x2) topHole1=x2;
	if(topHole2<x1) topHole2=x1; 	 if(topHole2>x2) topHole2=x2;
	if(topHole1>topHole2) topHole2=topHole1;
	v[2].p.Set(topHole1,y0,0); v[3].p.Set(topHole2,y0,0);
	v[8].p.Set(topHole1,y1,0); v[9].p.Set(topHole2,y1,0);
	v[ 0].p.Set(x0,y0,0); v[ 1].p.Set(x1,y0,0); v[ 4].p.Set(x2,y0,0);  v[ 5].p.Set(x3,y0,0);
	v[ 6].p.Set(x0,y1,0); v[ 7].p.Set(x1,y1,0); v[10].p.Set(x2,y1,0);  v[11].p.Set(x3,y1,0);
	v[12].p.Set(x0,y2,0); v[13].p.Set(x1,y2,0); v[14].p.Set(x2,y2,0);  v[15].p.Set(x3,y2,0);
	v[16].p.Set(x0,y3,0); v[17].p.Set(x1,y3,0); v[18].p.Set(x2,y3,0);  v[19].p.Set(x3,y3,0);

	txtFlat->SetTxt();
	glColor4f(1,1,1,1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3,GL_FLOAT,sizeof(TV),&v->p);
	glTexCoordPointer(2,GL_FLOAT,sizeof(TV),&v->t);
	glDrawElements(GL_TRIANGLES,54,GL_UNSIGNED_BYTE,idx);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Rects2D::DrawTube(float x0,float y0,float x3,float y3)
{
	float x1=x0+8,x2=x3-8;
	float y1=y0+8,y2=y3-8;
	if(x1>x2) { x1=x2=(x1+x2)/2; }
	if(y1>y2) { y1=y2=(y1+y2)/2; }

	v[2].p.Set(x1,y0,0); v[3].p.Set(x1,y0,0);
	v[8].p.Set(x1,y1,0); v[9].p.Set(x1,y1,0);
	v[ 0].p.Set(x0,y0,0); v[ 1].p.Set(x1,y0,0); v[ 4].p.Set(x2,y0,0);  v[ 5].p.Set(x3,y0,0);
	v[ 6].p.Set(x0,y1,0); v[ 7].p.Set(x1,y1,0); v[10].p.Set(x2,y1,0);  v[11].p.Set(x3,y1,0);
	v[12].p.Set(x0,y2,0); v[13].p.Set(x1,y2,0); v[14].p.Set(x2,y2,0);  v[15].p.Set(x3,y2,0);
	v[16].p.Set(x0,y3,0); v[17].p.Set(x1,y3,0); v[18].p.Set(x2,y3,0);  v[19].p.Set(x3,y3,0);

	txtTube->SetTxt();

	if(fragProg)
	{
		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,tubeProg);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,0,
			2.f/Game::Get()->RenderDimX,
			2.f/Game::Get()->RenderDimY,0,0);

	}
	glColor4f(1,1,1,1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3,GL_FLOAT,sizeof(TV),&v->p);
	glTexCoordPointer(2,GL_FLOAT,sizeof(TV),&v->t);
	glDrawElements(GL_TRIANGLES,54,GL_UNSIGNED_BYTE,idx);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
}

void Rects2D::DrawFill(float x0,float y0,float x3,float y3)
{
	float x1=x0+4,x2=x3-4;
	float y1=y0+4,y2=y3-4;
	if(x1>x2) { x1=x2=(x1+x2)/2; }
	if(y1>y2) { y1=y2=(y1+y2)/2; }
	v[ 0].p.Set(x0,y0,0); v[ 1].p.Set(x1,y0,0); v[ 4].p.Set(x2,y0,0);  v[ 5].p.Set(x3,y0,0);
	v[ 6].p.Set(x0,y1,0); v[ 7].p.Set(x1,y1,0); v[10].p.Set(x2,y1,0);  v[11].p.Set(x3,y1,0);
	v[12].p.Set(x0,y2,0); v[13].p.Set(x1,y2,0); v[14].p.Set(x2,y2,0);  v[15].p.Set(x3,y2,0);
	v[16].p.Set(x0,y3,0); v[17].p.Set(x1,y3,0); v[18].p.Set(x2,y3,0);  v[19].p.Set(x3,y3,0);

	txtFill->SetTxt();
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3,GL_FLOAT,sizeof(TV),&v->p);
	glTexCoordPointer(2,GL_FLOAT,sizeof(TV),&v->t);
	glDrawElements(GL_TRIANGLES,54,GL_UNSIGNED_BYTE,idxf);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Rects2D::DrawGlass(float x0,float y0,float x1,float y1)
{
	x0+=4; y0+=4;
	x1-=4; y1-=4;
	if(fragProg)
	{
		glFinish();
		glBindTexture(GL_TEXTURE_2D, txtScreen);
		float tx0=x0*Game::Get()->RenderDimY/480;
		float ty0=y0*Game::Get()->RenderDimY/480;
		float tx1=x1*Game::Get()->RenderDimY/480;
		float ty1=y1*Game::Get()->RenderDimY/480;
		float cx=(tx0+tx1)/2;
		float cy=(ty0+ty1)/2;
		tx0=cx-txtScreenDimX/2;
		ty0=cy-txtScreenDimY/2;
		tx1=tx0+txtScreenDimX;
		ty1=ty0+txtScreenDimY;
		if(tx0<0)
		{
			tx0=0;
			tx1=txtScreenDimX;
		} else
		if(tx1>Game::Get()->RenderDimX)
		{
			tx1=Game::Get()->RenderDimX;
			tx0=tx1-txtScreenDimX;
		}
		if(ty0<0)
		{
			ty0=0;
			ty1=txtScreenDimY;
		} else
		if(ty1>Game::Get()->RenderDimY)
		{
			ty1=Game::Get()->RenderDimY;
			ty0=ty1-txtScreenDimY;
		}
		cx=(tx0+tx1)/2; cy=(ty0+ty1)/2;
		glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,tx0,Game::Get()->RenderDimY-ty1,txtScreenDimX,txtScreenDimY);

		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,glassProg);
		float fx=1.f/txtScreenDimX;
		float fy=1.f/txtScreenDimY;
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,0,fx,fy,0,0);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,1,-tx0,ty0-Game::Get()->RenderDimY,0,0);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		txtGlass->SetTxt();
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, txtScreen);
		glActiveTextureARB(GL_TEXTURE0_ARB);

		glColor4f(1,1,1,1);
	} else
	{
		glDisable(GL_TEXTURE_2D);
		glColor4f(1,1,1,0.75f);
	}
	glBegin(GL_TRIANGLE_STRIP);
	float nx=(x1-x0)/16;
	float ny=(y1-y0)/16;
	glTexCoord2f( 0, 0); glVertex2f(x0,y0);
	glTexCoord2f(nx, 0); glVertex2f(x1,y0);
	glTexCoord2f( 0,ny); glVertex2f(x0,y1);
	glTexCoord2f(nx,ny); glVertex2f(x1,y1);
	glEnd();

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
}

void Rects2D::DrawGlassShadow(float x0,float y0,float x3,float y3)
{
	x0+=8;x3+=8;
	y0+=8;y3+=8;
	float x1=x0+8,x2=x3-8;
	float y1=y0+8,y2=y3-8;
	if(x1>x2) { x1=x2=(x1+x2)/2; }
	if(y1>y2) { y1=y2=(y1+y2)/2; }
	v[ 0].p.Set(x0,y0,0); v[ 1].p.Set(x1,y0,0); v[ 4].p.Set(x2,y0,0);  v[ 5].p.Set(x3,y0,0);
	v[ 6].p.Set(x0,y1,0); v[ 7].p.Set(x1,y1,0); v[10].p.Set(x2,y1,0);  v[11].p.Set(x3,y1,0);
	v[12].p.Set(x0,y2,0); v[13].p.Set(x1,y2,0); v[14].p.Set(x2,y2,0);  v[15].p.Set(x3,y2,0);
	v[16].p.Set(x0,y3,0); v[17].p.Set(x1,y3,0); v[18].p.Set(x2,y3,0);  v[19].p.Set(x3,y3,0);

	txtGlassShadow->SetTxt();
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3,GL_FLOAT,sizeof(TV),&v->p);
	glTexCoordPointer(2,GL_FLOAT,sizeof(TV),&v->t);
	glDrawElements(GL_TRIANGLES,54,GL_UNSIGNED_BYTE,idxf);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void Rects2D::DeInit()
{
	SAFE_DELETE(txtFlat);
	SAFE_DELETE(txtFill);
	SAFE_DELETE(txtTube);
	SAFE_DELETE(txtGlass);
	SAFE_DELETE(txtGlassShadow);
}
