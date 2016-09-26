#include "menus.h"
#include "systeminterface.h"
#include "game.h"

//#define SAVE_CUBES

#include <stdio.h>

void Gallery::Init()
{
	float w2D=Font::Get2DScreenWidth();
	float c=w2D/2;
	selPiece.bbox.Set(V2D(c-100,10),V2D(c+100,40));
	selPiece.vertical=false;
	selPiece.minValue=1;
	selPiece.maxValue=N_PIECES;
	selPiece.stepValue=1;
	currPiece=0;
	selPiece.pos=1;
	exit.bbox.Set(V2D(w2D/4,445),V2D(w2D*3/4,475));
	exit.text=Game::Get()->locKit.string[LK_EDIT+7][0];
	SetPiece();
	alpha=beta=0;
}

void Gallery::SetPiece()
{
	holeShow.radMan.nDivision=N_DIVISION_MAX; //max quality
	char buff[30];
	sprintf(buff,"EditCubes/prev%03i.boh",selPiece.pos-1);
	holeShow.Load(buff);
	currPiece=selPiece.pos;
}

bool Gallery::Update()
{
	if(mouse.btn)
	{
		alpha+=(mouse.x-exMx)/100.f;
		beta+=(mouse.y-exMy)/100.f;
		if(beta>1.56f) beta=1.56f;
		if(beta<0) beta=0;
	}
	exMx=mouse.x;
	exMy=mouse.y;

	V2D m; Font::ConvertMouse(&m.x,&m.y);
	selPiece.Update(m,mouse.btn,mouse.last);
	if(selPiece.pos!=currPiece)
	{
		SetPiece();
	}
	if(exit.Update(m,mouse.btn,mouse.last))
	{
		Game::Get()->currMenu=new MainMenu;
		Game::Get()->currMenu->Init();
		delete this;
		return true;
	}
	return true;
}

void Gallery::Draw3D()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float w=3.5f*Game::Get()->RenderDimX/Game::Get()->RenderDimY;
	float h=3.5f/4; w/=4;
	glFrustum(-w,w,-h,h,1,20);
	V3D position,up,lookAt;
	lookAt.Set(0,0,-2.5f);
	up.x=-sinf(beta)*sinf(alpha);
	up.y=-sinf(beta)*cosf(alpha);
	up.z= cosf(beta);
	position.x=lookAt.x+4*cosf(beta)*sinf(alpha);
	position.y=lookAt.y+4*cosf(beta)*cosf(alpha);
	position.z=lookAt.z+4*sinf(beta);
	gluLookAt(position.x,position.y,position.z,lookAt.x,lookAt.y,lookAt.z,up.x,up.y,up.z);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLineWidth(1);
	Matrix m;

	holeShow.Draw(false,false,false,false);
}

void Gallery::Draw()
{
	Font *f=&Game::Get()->font;
	Rects2D *r=&Game::Get()->rects2D;
	Arrow2D *a=&Game::Get()->arrows;
	selPiece.Draw(f,a);
	exit.Draw(f,r);
	f->Draw();
}

void Gallery::Deinit()
{
	holeShow.Deinit();
}
