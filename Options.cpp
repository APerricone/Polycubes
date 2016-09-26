#include "menus.h"
#include "systeminterface.h"
#include "game.h"

void Options::Init()
{
	float w2D=Font::Get2DScreenWidth();
	float c=w2D/2;
	exit.bbox.inf.x=doBenchMark.bbox.inf.x=c-240;
	exit.bbox.sup.x=doBenchMark.bbox.sup.x=c+240;
	musicVol.bbox.inf.x=sfxVol.bbox.inf.x=c+5;
	sDevice.bbox.inf.x=lang.bbox.inf.x=c;
	quality.bbox.inf.x=c+150;
	sDevice.bbox.sup.x=lang.bbox.sup.x=
		quality.bbox.sup.x=c+240;
	musicVol.bbox.sup.x=sfxVol.bbox.sup.x=c+235;
	doBenchMark.bbox.inf.y=	50;
	doBenchMark.bbox.sup.y=	80;
	quality.bbox.inf.y=		100+5;
	quality.bbox.sup.y=		130-5;
	sDevice.bbox.inf.y=		150;
	sDevice.bbox.sup.y=		180;
	musicVol.bbox.inf.y=	200;
	musicVol.bbox.sup.y=	230;
	sfxVol.bbox.inf.y=		250;
	sfxVol.bbox.sup.y=		280;
	lang.bbox.inf.y=		300;
	lang.bbox.sup.y=		330;
	exit.bbox.inf.y=		350;
	exit.bbox.sup.y=		380;

	LocKit *lk=&Game::Get()->locKit;
	doBenchMark.text=lk->string[LK_OPTIONS][0];
	quality.pos=Game::Get()->radQuality;
	musicVol.pos=Game::Get()->musicVolume;
	sfxVol.pos=Game::Get()->sfxVolume;
	quality.vertical=false;
	quality.minValue=0; 	quality.maxValue=3;		quality.stepValue=1;
	musicVol.size=100;		musicVol.steps=10;
	sfxVol.size=100;		sfxVol.steps=10;

	lang.bottomY=lang.bbox.sup.y+80;
	unsigned char i,n=lk->CountLangFile();
	for(i=0;i<n;i++)
	{
		lk->GetLangName(i,langNames[i],MAX_LEN);
		lang.AddElementToList(langNames[i],(void*)i);
	}
	lang.SelectRef((void*)Game::Get()->currLang);

	sDevice.bottomY=sDevice.bbox.sup.y+80;
	i=0;
	const char *devices=alcGetString(0,ALC_DEVICE_SPECIFIER);
	while(devices[0])
	{
		n=0;
		while(devices[n])
		{
			deviceNames[i][n]=devices[n];
			n++;
			if(n==MAX_LEN-1) break;
		}
		deviceNames[i][n]=0;
		sDevice.AddElementToList(deviceNames[i],(void*)i);
		devices=devices+n+1;
		i++;
		if(i==10) break;
	}
	sDevice.SelectRef((void*)Game::Get()->currDevice);
	if(Game::Get()->currDevice==-1)
	{
		sDevice.Select(L"Generic Software");
	}
	exit.text=lk->string[LK_OPTIONS+6][0];
}

bool Options::Update()
{
	if(waitforMessage!=0)
	{
		if(waitforMessage&1)
			return true;
		Hole *bck;
		DWORD time1,time2;
		unsigned char i;
		for(i=0;i<4;i++)
		{
			bck=new Hole;
			bck->radMan.nDivision=1<<(3+i);
			bck->Init(6,6,6);
			bck->GetBOCube(B3D(0,0,0))->on=1;
			bck->GetBOCube(B3D(0,0,5))->on=1;
			bck->GetBOCube(B3D(0,5,0))->on=1;
			bck->GetBOCube(B3D(0,5,5))->on=1;
			bck->GetBOCube(B3D(5,0,0))->on=1;
			bck->GetBOCube(B3D(5,0,5))->on=1;
			bck->GetBOCube(B3D(5,5,0))->on=1;
			bck->GetBOCube(B3D(5,5,5))->on=1;
			time1=GetTickCount();
			bck->UpdateAll();
			time2=GetTickCount()-time1;
			delete bck;
			//message("time","test %i time: %i",i,time2);
			if(time2>800)
			{
				break;
			}
		}
		if(i==0) quality.pos=0; else
		if((i==3) && (time2<=800)) quality.pos=3; else
			quality.pos=i-1;
		waitforMessage=0;
		return true;
	}
	V2D m;
	Font::ConvertMouse(&m.x,&m.y);
	unsigned char r;
	if(sDevice.Update(m,mouse.btn,mouse.last)!=0)
		return true;
	r=lang.Update(m,mouse.btn,mouse.last);
	if(r!=0)
	{
		if(r==2)
		{
			LocKit *lk=&Game::Get()->locKit;
			Game::Get()->currLang=(unsigned char)lang.GetSelElement();
			lk->Init(Game::Get()->currLang);
			doBenchMark.text=lk->string[LK_OPTIONS][0];
			exit.text=lk->string[LK_OPTIONS+6][0];
		}
		return true;
	}
	if(doBenchMark.Update(m,mouse.btn,mouse.last))
		waitforMessage=1;
	quality.Update(m,mouse.btn,mouse.last);
	musicVol.Update(m,mouse.btn,mouse.last);
	sfxVol.Update(m,mouse.btn,mouse.last);
	if(exit.Update(m,mouse.btn,mouse.last))
	{
		Game::Get()->radQuality=quality.pos;
		Game::Get()->musicVolume=musicVol.pos;
		Game::Get()->sfxVolume=sfxVol.pos;
		if(wcscmp(sDevice.GetSelElementName(),L"Generic Software")==0)
			Game::Get()->currDevice=-1;
		else
			Game::Get()->currDevice=(unsigned char)sDevice.GetSelElement();
		Game::Get()->currMenu=new MainMenu;
		Game::Get()->currMenu->Init();
		delete this;
		return true;
	}
	return true;
}

void Options::Draw()
{
	Font *f=&Game::Get()->font;
	Rects2D *r=&Game::Get()->rects2D;
	Arrow2D *a=&Game::Get()->arrows;
	LocKit *lk=&Game::Get()->locKit;

	if(waitforMessage!=0)
	{
		f->ShowWStringOnBox(lk->string[LK_OPTIONS+7][0],
			doBenchMark.bbox.inf.x,quality.bbox.inf.y,
			doBenchMark.bbox.sup.x,quality.bbox.sup.y,cBlack,Font::CENTER);
		f->Draw();
		waitforMessage++;
		return;
	}

	doBenchMark.Draw(f,r);
	f->ShowWStringOnBox(lk->string[LK_MAINMENU+2][0],
		doBenchMark.bbox.inf.x,doBenchMark.bbox.inf.y-40,
		doBenchMark.bbox.sup.x,doBenchMark.bbox.inf.y-20,cBlack,Font::CENTER);
	f->ShowWStringOnBox(lk->string[LK_OPTIONS+1][0],
		doBenchMark.bbox.inf.x+15,quality.bbox.inf.y,
		quality.bbox.inf.x,quality.bbox.sup.y,cBlack,Font::LEFT);
	f->ShowWStringOnBox(lk->string[LK_OPTIONS+2][0],
		doBenchMark.bbox.inf.x+15,sDevice.bbox.inf.y+5,
		sDevice.bbox.inf.x,sDevice.bbox.sup.y-5,cBlack,Font::LEFT);
	f->ShowWStringOnBox(lk->string[LK_OPTIONS+3][0],
		doBenchMark.bbox.inf.x+15,musicVol.bbox.inf.y+5,
		musicVol.bbox.inf.x,musicVol.bbox.sup.y-5,cBlack,Font::LEFT);
	f->ShowWStringOnBox(lk->string[LK_OPTIONS+4][0],
		doBenchMark.bbox.inf.x+15,sfxVol.bbox.inf.y+5,
		sfxVol.bbox.inf.x,sfxVol.bbox.sup.y-5,cBlack,Font::LEFT);
	f->ShowWStringOnBox(lk->string[LK_OPTIONS+5][0],
		doBenchMark.bbox.inf.x+15,lang.bbox.inf.y+5,
		lang.bbox.inf.x,lang.bbox.sup.y-5,cBlack,Font::LEFT);
	quality.Draw(f,a);
	musicVol.Draw(r);
	sfxVol.Draw(r);
	exit.Draw(f,r);
	f->Draw();
	sDevice.Draw(f,r,a);
	lang.Draw(f,r,a);
	f->Draw();
}

void Options::Deinit()
{
}
