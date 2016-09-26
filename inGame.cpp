#include "menus.h"
#include "systeminterface.h"
#include "game.h"

void inGame::Init()
{
	float w2D=Font::Get2DScreenWidth();
	bbox.inf.x=w2D*3/8;
	bbox.sup.x=bbox.inf.x+w2D/4;
	bbox.inf.y=240-55;
	bbox.sup.y=240+55;
	continueGame.bbox.inf.y=225;
	continueGame.bbox.sup.y=255;
	exit.bbox.inf.y=256;
	exit.bbox.sup.y=285;

	exit.bbox.inf.x=continueGame.bbox.inf.x=bbox.inf.x+10;
	exit.bbox.sup.x=continueGame.bbox.sup.x=bbox.sup.x-10;

	LocKit *lk=&Game::Get()->locKit;
	continueGame.text=lk->string[LK_PAUSE+1][0];
	exit.text=lk->string[LK_PAUSE+2][0];
}

bool inGame::Update()
{
	V2D m;
	Font::ConvertMouse(&m.x,&m.y);

	if(move && mouse.btn)
	{
		start-=m;
		bbox.inf-=start;
		bbox.sup-=start;
		continueGame.bbox.inf-=start;
		continueGame.bbox.sup-=start;
		exit.bbox.inf-=start;
		exit.bbox.sup-=start;
		start=m;
		return true;
	}
	move=false;
	if(buttons[11])
	{
		move=false;
	}
	if(continueGame.Update(m,mouse.btn,mouse.last)||
		((buttons[11])&&(!lastEsc)))
	{
		Game::Get()->currMenu=0;
		delete this;
		return true;
	}
	lastEsc=buttons[11];
	if(exit.Update(m,mouse.btn,mouse.last))
	{
		if(Game::Get()->match.End(false))
		{
			Game::Get()->currMenu=0;
		}
		else
		{
			Game::Get()->currMenu=new MainMenu;
			Game::Get()->currMenu->Init();
		}
		delete this;
		return true;
	}

	if( (bbox.IsInside(m)) && (mouse.btn) && (!mouse.last)&&
		(!continueGame.bbox.IsInside(m)) &&
		(!exit.bbox.IsInside(m)))
	{
		move=true;
		start=m;
	}
	return true;
}

void inGame::Draw()
{
	Font *f=&Game::Get()->font;
	Rects2D *r=&Game::Get()->rects2D;
	Arrow2D *a=&Game::Get()->arrows;

	r->DrawGlassShadow(bbox.inf.x,bbox.inf.y,bbox.sup.x,bbox.sup.y);
	r->DrawGlass(bbox.inf.x,bbox.inf.y,bbox.sup.x,bbox.sup.y);
	r->DrawTube(bbox.inf.x,bbox.inf.y,bbox.sup.x,bbox.sup.y);

	f->ShowWStringOnBox(Game::Get()->locKit.string[LK_PAUSE][0],
		bbox.inf.x+15,bbox.inf.y+10,
		bbox.sup.x-15,bbox.inf.y+40,cBlack,Font::CENTER);

	continueGame.Draw(f,r);
	exit.Draw(f,r);
	f->Draw();
}

void inGame::Deinit()
{
}
