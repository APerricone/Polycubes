#include "menus.h"
#include "systeminterface.h"
#include "game.h"

void MainMenu::Init()
{
	float w2D=Font::Get2DScreenWidth();
	float p1=w2D/4,p2=p1*3;
	for(char i=0;i<6;i++)
	{
		buttons[i].bbox.inf.Set(p1,275+i*31.f);
		buttons[i].bbox.sup.Set(p2,305+i*31.f);
	}

	games.bbox.Set(V2D(w2D*3/8,150),V2D(w2D*5/8,180));
	games.bottomY=340;
	Games *gg=&Game::Get()->games;
	GameSet *u=gg->gameList;
	while(u)
	{
		games.AddElementToList(u->gameName,u);
		u=u->next;
	}
	games.Select(gg->current->gameName);
	startLevel.bbox.Set(V2D(p2-100,235),V2D(p2,255));
	startLevel.vertical=false;
	startLevel.minValue=1;
	startLevel.maxValue=10;
	startLevel.stepValue=1;
	startLevel.pos=Game::Get()->startLevel;

	LocKit *lk=&Game::Get()->locKit;
	buttons[0].text=lk->string[LK_MAINMENU+0][0];
	buttons[1].text=lk->string[LK_MAINMENU+1][0];
	buttons[2].text=lk->string[LK_MAINMENU+2][0];
	buttons[3].text=lk->string[LK_MAINMENU+3][0];
	buttons[4].text=lk->string[LK_MAINMENU+4][0];
	buttons[5].text=lk->string[LK_MAINMENU+5][0];
	exitConfirm=0;

	myNameW=Game::Get()->font.GetWStringWidth(lk->string[LK_AUTHOR][0])*7.5f;
	PolycubesSize=Game::Get()->font.GetWStringWidth(L"Polycubes")*50.f;
}

bool MainMenu::Update()
{
	if(exitConfirm)
	{
		unsigned char r=exitConfirm->Update();
		if(r==0) return false;
		if(r==1)
		{
			delete exitConfirm;
			exitConfirm=0;
		}
		return true;
	}
	float w2D=Font::Get2DScreenWidth();
	float c=w2D/2;
	V2D m;
	Font::ConvertMouse(&m.x,&m.y);

	if( (mouse.last)&&(!mouse.btn)&&
		(m.y>110)&&(m.y<125)&&(m.x>c-myNameW)&&(m.x<c+myNameW))
		{
			Game::Get()->currMenu=new Gallery;
			Game::Get()->currMenu->Init();
			delete this;
			return true;
		}
	if( (mouse.last)&&(!mouse.btn)&&
		(m.y>10)&&(m.y<110)&&(m.x>c-PolycubesSize)&&(m.x<c+PolycubesSize))
		{
			OpenLink("http://sourceforge.net/projects/polycubes");
		}

	unsigned char r=games.Update(m,mouse.btn,mouse.last);
	if(r==2) Game::Get()->games.current=(GameSet*)games.GetSelElement();
    if(r!=0) return true;
	startLevel.Update(m,mouse.btn,mouse.last);
	Game::Get()->startLevel=startLevel.pos;

	for(char i=0;i<6;i++)
		if(buttons[i].Update(m,mouse.btn,mouse.last))
		{
			switch(i)
			{
			case 0: Game::Get()->match.Init(); Game::Get()->currMenu=0; delete this; return true;
			case 1: Game::Get()->currMenu=new EditGame; Game::Get()->currMenu->Init(); delete this; return true;
			case 2: Game::Get()->currMenu=new Options; Game::Get()->currMenu->Init(); delete this; return true;
			case 3: Game::Get()->currMenu=new Records; Game::Get()->currMenu->Init(); delete this; return true;
			case 4: Game::Get()->currMenu=new Help; Game::Get()->currMenu->Init(); delete this; return true;
			case 5: exitConfirm=new Dialog();
					exitConfirm->Init(
						Game::Get()->locKit.string[LK_EXITQUERY][0],
						Game::Get()->locKit.string[LK_EXITQUERY][1],Dialog::YESNO);
					break;
			}
		}
	return true;
}

void MainMenu::Draw()
{
	Font *f=&Game::Get()->font;
	float w2D=Font::Get2DScreenWidth();
	Rects2D *r=&Game::Get()->rects2D;
	LocKit *lk=&Game::Get()->locKit;
	f->ShowWStringOnBox(L"Polycubes",0,10,w2D,110,cBlack,Font::CENTER);
	f->ShowWStringOnBox(L"v.1.1",w2D/2+PolycubesSize,95,w2D,110,cBlack,Font::LEFT);
	f->ShowWStringOnBox(lk->string[LK_AUTHOR][0],0,110,w2D,125,cBlack,Font::CENTER);
	f->ShowWStringOnBox(lk->string[LK_AUTHOR+1][0],0,130,w2D,145,cBlack,Font::CENTER);

	GameSet *current=Game::Get()->games.current;
	float p1=buttons[0].bbox.inf.x;
	float p2=buttons[0].bbox.sup.x;
	float p3=games.bbox.inf.x;
	float p4=games.bbox.sup.x;
	Game::Get()->rects2D.Draw(p1-10,168,p2+10,220,p3,p4);
	char buff[20]; sprintf(buff,"%ix%ix%i",current->sideA,current->sideB,current->depth);
	f->ShowStringOnBox(buff,p1,180,p2,195,cBlack,Font::CENTER);
	f->ShowWStringOnBox(current->currSet->setName,p1,195,p2,210,cBlack,Font::CENTER);

	f->ShowWStringOnBox(lk->string[LK_MAINMENU+6][0],
		p1,startLevel.bbox.inf.y,
		p2-100,startLevel.bbox.sup.y,cBlack,Font::CENTER);
	startLevel.Draw(f,&Game::Get()->arrows);

	for(char i=0;i<6;i++) buttons[i].Draw(f,r);
	games.Draw(f,r,&Game::Get()->arrows);
	f->Draw();
	if(exitConfirm)
		exitConfirm->Draw();
}

void MainMenu::Deinit()
{
	SAFE_DELETE(exitConfirm);
}
