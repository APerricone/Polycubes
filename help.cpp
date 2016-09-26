#include "menus.h"
#include "systeminterface.h"
#include "game.h"
#include "image.h"

#include <stdio.h>

#define LOGOX1 40
#define LOGOX2 200
#define LOGOY1 270
#define LOGOY2 350


void Help::Init()
{
	float w2D=Font::Get2DScreenWidth();
	float c=w2D/2;
	pageChange.bbox.Set(V2D(c-100,420),V2D(c+100,440));
	pageChange.vertical=false;
	pageChange.minValue=1;
	pageChange.maxValue=3;
	pageChange.stepValue=1;
	pageChange.pos=1;
	pageChange.SetWithMax(true);
	exit.bbox.Set(V2D(w2D/4,445),V2D(w2D*3/4,475));
	exit.text=Game::Get()->locKit.string[LK_EDIT+7][0];

	currKey=0;
	animTime=0;
	m[0].Reset();
	model.SetupDraw(5);
	mMove.Reset();
	mMove.Pos().x=3;
	mMove.Pos().z=-8;
	angle=0;
	libs[0]=new Image;
	libs[1]=new Image;
	libs[0]->LoadPNG("libs\\ogl.png");
	libs[1]->LoadPNG("libs\\oal.png");
	libs[0]->CreateTxt();
	libs[1]->CreateTxt();
}

bool Help::Update()
{
	if(pageChange.pos==1)
	{
		animTime++;
		if((animTime>=20)&&(animTime<30))
		{
			switch(currKey)
			{
				case 0: mMove.Pos().x+=0.1f; break;
				case 1: mMove.Pos().y-=0.1f; break;
				case 2: mMove.Pos().x-=0.1f; break;
				case 3: mMove.Pos().y+=0.1f; break;
			}
		}
		if((currKey==4)&&(animTime>=20))
		{
			mMove.Pos().z-=0.4f;
		}
		if(animTime==60)
		{
			animTime=0;
			currKey++;
			if(currKey==6)
			{
				currKey=0;
				mMove.Reset();
				mMove.Pos().x=3;
				mMove.Pos().z=-8;
			}
		}
	} else
	if(pageChange.pos==2)
	{
		angle+=3.1416f/36.f;
		if(angle>31.416f) angle-=31.416f;
		m[0].SetXRotate(angle);
		m[1].SetYRotate(angle);
		m[2].SetZRotate(angle);
		m[3].SetXRotate(-angle);
		m[4].SetYRotate(180-angle);
		m[5].SetZRotate(-angle);
		m[0].Pos().Set(-4.5f,3,-16);
		m[1].Pos().Set( 0.0f,3,-16);
		m[2].Pos().Set( 5.0f,3,-16);
		m[3].Pos().Set(-4.5f,-3,-16);
		m[4].Pos().Set( 0.0f,-3,-16);
		m[5].Pos().Set( 5.0f,-3,-16);
	}
	V2D m; Font::ConvertMouse(&m.x,&m.y);
	pageChange.Update(m,mouse.btn,mouse.last);
	if(exit.Update(m,mouse.btn,mouse.last))
	{
		Game::Get()->currMenu=new MainMenu;
		Game::Get()->currMenu->Init();
		SetHandCursor(false);
		delete this;
		return true;
	}
	SetHandCursor(false);
	if(pageChange.pos==3)
	{
		float w2D=Font::Get2DScreenWidth();
		float c=w2D/2;
		if(	(m.x>c-LOGOX2)&&(m.x<c-LOGOX1)&&
			(m.y>LOGOY1)&&(m.y<LOGOY2))
			{
				SetHandCursor(true);
				if(mouse.btn && !mouse.last)
					OpenLink("http://www.opengl.org");
			}
		if(	(m.x>c+LOGOX1)&&(m.x<c+LOGOX2)&&
			(m.y>LOGOY1)&&(m.y<LOGOY2))
			{
				SetHandCursor(true);
				if(mouse.btn && !mouse.last)
					OpenLink("http://www.openal.org");
			}
	}
	return true;
}

void Help::Draw3D()
{
	if(pageChange.pos==3) return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float w=12.f*Game::Get()->RenderDimX/Game::Get()->RenderDimY;
	float h=12.f/4; w/=4;
	glFrustum(-w,w,-h,h,1,16);
	glScalef(1,1,0.25f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(pageChange.pos==1)
	{
		glLoadMatrixf(&mMove.m11);
		model.Draw();
	} else
	{
		for(unsigned char i=0;i<6;i++)
		{
			glLoadMatrixf(&m[i].m11);
			model.Draw();
		}
	}
}

void Help::DrawPage1(Font *f,Rects2D *r,Arrow2D *a,LocKit *lk,float w2D,float c)
{
	f->ShowWStringOnBox(lk->string[LK_HELP+1][0],0,10,w2D,30,
		cBlack,Font::CENTER);
	f->ShowWStringOnBox(lk->string[LK_HELP+2][0],0,30,w2D,50,
		cBlack,Font::CENTER);
	bool black=((animTime>19)&&(animTime<25));

	if(black && currKey==3)
		r->DrawFill(c-165,170,c-135,200); else
		r->Draw(c-165,170,c-135,200);
	if(black && currKey==2)
		r->DrawFill(c-195,200,c-165,230); else
		r->Draw(c-195,200,c-165,230);
	if(black && currKey==1)
		r->DrawFill(c-165,200,c-135,230); else
		r->Draw(c-165,200,c-135,230);
	if(black && currKey==0)
		r->DrawFill(c-135,200,c-105,230); else
		r->Draw(c-135,200,c-105,230);

	a->DrawUp(c-160,180,c-150,190,(black && currKey==3));
	a->DrawLeft(c-190,210,c-180,220,(black && currKey==2));
	a->DrawDown(c-160,210,c-150,220,(black && currKey==1));
	a->DrawRight(c-130,210,c-120,220,(black && currKey==0));

	if(black && currKey==4)
	{
		r->DrawFill(c-240,250,c-60,280);
		f->ShowWStringOnBox(lk->string[LK_HELP+3][0],c-230,255,c-70,275,
			cWhite,Font::CENTER);
	} else
	{
		r->Draw(c-240,250,c-60,280);
		f->ShowWStringOnBox(lk->string[LK_HELP+3][0],c-230,255,c-70,275,
			cBlack,Font::CENTER);
	}
}

void Help::DrawPage2(Font *f,Rects2D *r,Arrow2D *a,LocKit *lk,float w2D,float c)
{
	f->ShowWStringOnBox(lk->string[LK_HELP+4][0],c-240,10,w2D,30,cBlack,Font::LEFT);
	f->ShowWStringOnBox(lk->string[LK_HELP+5][0],c-240,30,w2D,50,cBlack,Font::LEFT);
	f->ShowWStringOnBox(lk->string[LK_HELP+6][0],c-240,50,w2D,70,cBlack,Font::LEFT);
	f->ShowWStringOnBox(lk->string[LK_HELP+7][0],c-240,70,w2D,90,cBlack,Font::LEFT);
	f->ShowWStringOnBox(lk->string[LK_HELP+8][0],c-240,90,w2D,110,cBlack,Font::LEFT);
	r->Draw(c+85,220,c+115,250); 	r->Draw(c+85,350,c+115,380);
	r->Draw(c-15,220,c+15,250);		r->Draw(c-15,350,c+15,380);
	r->Draw(c-115,220,c-85,250); 	r->Draw(c-115,350,c-85,380);
	f->ShowStringOnBox("Q",c-110,225,c-90,245,cBlack,Font::CENTER);
	f->ShowStringOnBox("W",c-10,225,c+10,245,cBlack,Font::CENTER);
	f->ShowStringOnBox("E",c+90,225,c+110,245,cBlack,Font::CENTER);
	f->ShowStringOnBox("A",c-110,355,c-90,375,cBlack,Font::CENTER);
	f->ShowStringOnBox("S",c-10,355,c+10,375,cBlack,Font::CENTER);
	f->ShowStringOnBox("D",c+90,355,c+110,375,cBlack,Font::CENTER);
}

void Help::DrawPage3(Font *f,Rects2D *r,Arrow2D *a,LocKit *lk,float w2D,float c)
{
	f->ShowWStringOnBox(lk->string[LK_HELP+9][0],0,10,w2D,40,cBlack,Font::CENTER);
	f->ShowWStringOnBox(lk->string[LK_HELP+10][0],0,90,w2D,110,cBlack,Font::CENTER);
	f->ShowStringOnBox("Antonino Perricone",0,110,w2D,150,cBlack,Font::CENTER);
	f->ShowWStringOnBox(lk->string[LK_HELP+11][0],0,170,w2D,190,cBlack,Font::CENTER);
	f->ShowStringOnBox("Kevin MacLeod",0,190,w2D,230,cBlack,Font::CENTER);
	f->ShowWStringOnBox(lk->string[LK_HELP+12][0],0,250,w2D,270,cBlack,Font::CENTER);
	f->ShowStringOnBox("libPNG",0,LOGOY2,w2D,LOGOY2+20,cBlack,Font::CENTER);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(1,1,1,1);
	libs[0]->SetTxt();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,0); glVertex2f(c-LOGOX2,LOGOY1);
	glTexCoord2f(0,1); glVertex2f(c-LOGOX2,LOGOY2);
	glTexCoord2f(1,0); glVertex2f(c-LOGOX1,LOGOY1);
	glTexCoord2f(1,1); glVertex2f(c-LOGOX1,LOGOY2);
	glEnd();
	libs[1]->SetTxt();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,0); glVertex2f(c+LOGOX1,LOGOY1);
	glTexCoord2f(0,1); glVertex2f(c+LOGOX1,LOGOY2);
	glTexCoord2f(1,0); glVertex2f(c+LOGOX2,LOGOY1);
	glTexCoord2f(1,1); glVertex2f(c+LOGOX2,LOGOY2);
	glEnd();
}

void Help::Draw()
{
	Font *f=&Game::Get()->font;
	Rects2D *r=&Game::Get()->rects2D;
	Arrow2D *a=&Game::Get()->arrows;
	LocKit *lk=&Game::Get()->locKit;
	float w2D=Font::Get2DScreenWidth();
	float c=w2D/2;
	switch(pageChange.pos)
	{
		case 1: DrawPage1(f,r,a,lk,w2D,c); break;
		case 2: DrawPage2(f,r,a,lk,w2D,c); break;
		case 3: DrawPage3(f,r,a,lk,w2D,c); break;
	}

	pageChange.Draw(f,a);
	f->ShowWStringOnBox(lk->string[LK_HELP+0][0],
		pageChange.bbox.inf.x,pageChange.bbox.inf.y-20,
		pageChange.bbox.sup.x,pageChange.bbox.inf.y,cBlack,Font::CENTER);

	exit.Draw(f,r);
	f->Draw();
}

void Help::Deinit()
{
	delete libs[0];
	delete libs[1];
}
