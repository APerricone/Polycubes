#include "menus.h"
#include "systeminterface.h"
#include "game.h"

void Records::Init()
{
	float w2D=Font::Get2DScreenWidth();
	float c=w2D/2;
	games.bbox.Set(V2D(c-100,10),V2D(c+100,40));
	games.bottomY=180;
	gg=&Game::Get()->games;
	GameSet *u=gg->gameList;
	currGame=gg->current;
	while(u)
	{
		games.AddElementToList(u->gameName,u);
		u=u->next;
	}
	games.Select(currGame->gameName);

	exit.bbox.Set(V2D(c-100,440),V2D(c+100,470));
	exit.text=Game::Get()->locKit.string[LK_EDIT+7][0];
#ifdef DEBUG
	reset.bbox.Set(V2D(10,10),V2D(100,50));
	reset.text=L"Reset";
#endif
}

bool Records::Update()
{
	V2D m; Font::ConvertMouse(&m.x,&m.y);
    if(games.Update(m,mouse.btn,mouse.last)!=0)
        return true;
    currGame=(GameSet*)games.GetSelElement();
    if(exit.Update(m,mouse.btn,mouse.last))
    {
			Game::Get()->currMenu=new MainMenu;
			Game::Get()->currMenu->Init();
			delete this;
			return true;
    }
#ifdef DEBUG
    if(reset.Update(m,mouse.btn,mouse.last))
    {
		for(char i=0;i<10;i++)
		{
			wcscpy(currGame->records[i].name,L"Perry");
			currGame->records[i].points=100-i*10;
		}
    }
#endif
    return true;
}

void Records::Draw()
{
	Font *f=&Game::Get()->font;
	Rects2D *r=&Game::Get()->rects2D;
	Arrow2D *a=&Game::Get()->arrows;
	float w2D=Font::Get2DScreenWidth();
	float c=w2D/2;
	unsigned char i;
	char number[4],point[10];
	number[1]='.';
	number[2]=0;

    number[0]='1';
    itoa(currGame->records[0].points,point,10);
    f->ShowStringOnBox(number,c-240,80,c-160,120,cBlack,Font::LEFT);
    f->ShowWStringOnBox(currGame->records[0].name,c-160,80,c+240,120,cBlack,Font::LEFT);
    f->ShowStringOnBox(point,c-240,130,c+240,170,cBlack,Font::RIGHT);

	for(i=1;i<10;i++)
	{
	    number[0]='1'+i;
	    if(i==9) { memcpy(number,"10.",4); }
	    itoa(currGame->records[i].points,point,10);
	    f->ShowStringOnBox(number,c-240,180+i*25,c-200,200+i*25,cBlack,Font::LEFT);
	    f->ShowWStringOnBox(currGame->records[i].name,c-200,180+i*25,c+140,200+i*25,cBlack,Font::LEFT);
	    f->ShowStringOnBox(point,c+140,180+i*25,c+240,200+i*25,cBlack,Font::RIGHT);
	}
	exit.Draw(f,r);
    games.Draw(f,r,a);
#ifdef DEBUG
	reset.Draw(f,r);
#endif
    f->Draw();
}
