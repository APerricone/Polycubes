#include "menus.h"
#include "systeminterface.h"
#include "game.h"

//De-commentare SAVE_CUBES per creare i file con le ombre.
//#define SAVE_CUBES

#include <stdio.h>

void EditGame::Init()
{
	currCalc=0;
	currPiece=N_PIECES;
	state=NONE;

	float w2D=Font::Get2DScreenWidth();
	float c=w2D/2;
	LocKit *lk=&Game::Get()->locKit;

	games.nameWithOutSel=pieces.nameWithOutSel=lk->string[LK_EDIT+4][0];

	games.bbox.Set(V2D(c-280,25),V2D(c+80,55));
	games.bottomY=175;
	saveGame.bbox.Set(V2D(c+80,25),V2D(c+180,55));
	delGame.bbox.Set(V2D(c+181,25),V2D(c+280,55));
	saveGame.text=lk->string[LK_EDIT+5][0];
	delGame.text=lk->string[LK_EDIT+6][0];
	gg=&Game::Get()->games;
	GameSet *u=gg->gameList;
	while(u)
	{
		games.AddElementToList(u->gameName,u);
		u=u->next;
	}
	games.Select(gg->current->gameName);
	width.pos=gg->current->sideA;
	height.pos=gg->current->sideB;
	depth.pos=gg->current->depth;
	width.minValue=3; width.maxValue=6; width.stepValue=1;
	height.minValue=3; height.maxValue=6; height.stepValue=1;
	depth.minValue=6; depth.maxValue=14; depth.stepValue=1;
	width.bbox.Set(V2D(c-150,90),V2D(c-70,110));
	height.bbox.Set(V2D(c-40,90),V2D(c+40,110));
	depth.bbox.Set(V2D(c+70,90),V2D(c+150,110));

	pieces.bbox.Set(V2D(c-280,145),V2D(c+80,175));
	pieces.bottomY=285;
	savePieces.bbox.Set(V2D(c+80,145),V2D(c+180,175));
	delPieces.bbox.Set(V2D(c+181,145),V2D(c+280,175));
	savePieces.text=lk->string[LK_EDIT+5][0];
	delPieces.text=lk->string[LK_EDIT+6][0];
	PieceSet *up=gg->pieceSetList;
	while(up)
	{
		pieces.AddElementToList(up->setName,up);
		up=up->next;
	}
	pieces.Select(gg->current->currSet->setName);

	currGame.sideA=gg->current->sideA;
	currGame.sideB=gg->current->sideB;
	currGame.depth=gg->current->depth;
	currGame.currSet=&currPSet;
	memcpy(currPSet.canAppear,gg->current->currSet->canAppear,N_PIECES);

	exit.bbox.Set(V2D(w2D/4,445),V2D(w2D*3/4,475));
	exit.text=lk->string[LK_EDIT+7][0];
}

bool EditGame::Update()
{
	if(currCalc!=N_PIECES)
	{
		holes[currCalc].radMan.nDivision=N_DIVISION_MAX; //massima qualità
		char buff[30];
		sprintf(buff,"EditCubes/prev%03i.boh",currCalc);

#ifdef SAVE_CUBES
		holes[currCalc].InitForPreview();
		holes[currCalc].SetPreview(currCalc);
		holes[currCalc].Save(buff);
#else
		holes[currCalc].Load(buff);
#endif
		currCalc++;
		return true;
	}

	float w2D=Font::Get2DScreenWidth();
	V2D m; Font::ConvertMouse(&m.x,&m.y);
	LocKit *lk=&Game::Get()->locKit;

	if((state==QUERY_NEW_GAME_NAME))
	{
		switch(insertName.Update())
		{
		case 0: //ok
			if(gg->GetGameFromName(currGame.gameName))
			{
				state=CONFIRM_OVERWRITE_GAME;
				confirms.Init(
					lk->string[LK_EDIT_MESSAGE+0][0],
					lk->string[LK_EDIT_MESSAGE+0][1],Dialog::YESNO);
			} else
			{
				GameSet *u=(GameSet*)games.GetSelElement();
				if(u)
				{
					wcscpy(u->gameName,currGame.gameName);
				} else
				{
					u=gg->AddGame(currGame);
					games.AddElementToList(u->gameName,u);
					games.SelectRef(u);
					gg->current=u;
				}
				state=NONE;
			}
			break;
		case 1: //cancel
			state=NONE;
			break;
		}
		return true;
	}
	if((state==QUERY_NEW_PSET_NAME))
	{
		switch(insertName.Update())
		{
		case 0: //ok
			if(gg->GetPieceSetFromName(currPSet.setName))
			{
				state=CONFIRM_OVERWRITE_PSET;
				confirms.Init(
					lk->string[LK_EDIT_MESSAGE+1][0],
					lk->string[LK_EDIT_MESSAGE+1][1],Dialog::YESNO);
				return true;
			} else
			{
				PieceSet *u=(PieceSet*)pieces.GetSelElement();
				if(u)
				{
					wcscpy(u->setName,currPSet.setName);
				} else
				{
					u=gg->AddPieceSet(currPSet);
					pieces.AddElementToList(u->setName,u);
					pieces.SelectRef(u);
				}
				state=NONE;
			}
			break;
		case 1: //cancel
			state=NONE;
			break;
		}
		return true;
	}
	if(state==CONFIRM_DELETE_GAME)
	{
		switch(confirms.Update())
		{
		case 0: //si
			games.DelSelect();
			gg->DelGame(currGame);
			state=NONE;
			break;
		case 1:
			state=NONE;
			break;
		}
		return true;
	}
	if(state==CONFIRM_DELETE_PSET)
	{
		switch(confirms.Update())
		{
		case 0: //si
			pieces.DelSelect();
			gg->DelPieceSet(currPSet);
			state=NONE;
			break;
		case 1:
			state=NONE;
			break;
		}
		return true;
	}
	if(state==CONFIRM_OVERWRITE_GAME)
	{
		switch(confirms.Update())
		{
		case 0: //si
			{
				GameSet *u=gg->GetGameFromName(currGame.gameName);
				games.DelRef(u);
				gg->DelGame(*u);
				u=(GameSet*)games.GetSelElement();
				if(u)
				{
					wcscpy(u->gameName,currGame.gameName);
				} else
				{
					u=gg->AddGame(currGame);
					games.AddElementToList(u->gameName,u);
					games.SelectRef(u);
					gg->current=u;
				}
			}
			state=NONE;
			break;
		case 1:
			state=NONE;
			break;
		}
		return true;
	}
	if(state==CONFIRM_OVERWRITE_PSET)
	{
		switch(confirms.Update())
		{
		case 0: //si
			{
				PieceSet *u=gg->GetPieceSetFromName(currGame.gameName);
				pieces.DelRef(u);
				gg->DelPieceSet(*u);
				u=(PieceSet*)pieces.GetSelElement();
				if(u)
				{
					wcscpy(u->setName,currPSet.setName);
				} else
				{
					u=gg->AddPieceSet(currPSet);
					pieces.AddElementToList(u->setName,u);
					pieces.SelectRef(u);
				}
			}
			state=NONE;
			break;
		case 1:
			state=NONE;
			break;
		}
		return true;
	}

	if(state==MESSAGE)
	{
		if(confirms.Update()!=0x0FF)
			state=NONE;
		return true;
	}
    unsigned char r;
	if(pieces.state==ComboBox::NORMAL)
		r=games.Update(m,mouse.btn,mouse.last);
	if(r==2)
	{
		if(games.GetSelElement())
		{
			gg->current=(GameSet*)games.GetSelElement();
			width.pos=currGame.sideA=gg->current->sideA;
			height.pos=currGame.sideB=gg->current->sideB;
			depth.pos=currGame.depth=gg->current->depth;
			currGame.currSet=&currPSet;
			memcpy(currPSet.canAppear,gg->current->currSet->canAppear,N_PIECES);
			pieces.Select(gg->current->currSet->setName);
		}
	}
	if(r!=0) return true;
	if(games.state==ComboBox::NORMAL)
		r=pieces.Update(m,mouse.btn,mouse.last);
	if(r==2)
	{
		if(pieces.GetSelElement())
		{
			PieceSet *u=(PieceSet *)pieces.GetSelElement();
			memcpy(currPSet.canAppear,u->canAppear,N_PIECES);
			games.SelectRef(gg->GetGame(currGame));
		}
	}
	if(r!=0) return true;
	if(saveGame.Update(m,mouse.btn,mouse.last))
	{
		if(pieces.GetSelElement())
		{
			state=QUERY_NEW_GAME_NAME;
			if(games.GetSelElement())
				wcscpy(currGame.gameName,gg->current->gameName);
			else
				currGame.gameName[0]=0;
			insertName.Init(lk->string[LK_EDIT_MESSAGE+2][0],currGame.gameName,NAME_LEN,200);
		} else
		{
			state=MESSAGE;
			confirms.Init(
				lk->string[LK_EDIT_MESSAGE+3][0],
				lk->string[LK_EDIT_MESSAGE+3][0],Dialog::OK);
		}
		return true;
	}
	if(delGame.Update(m,mouse.btn,mouse.last)&&(games.GetSelElement()))
	{
		state=CONFIRM_DELETE_GAME;
		confirms.Init(
			lk->string[LK_EDIT_MESSAGE+4][0],
			lk->string[LK_EDIT_MESSAGE+4][1],Dialog::YESNO);
		return true;
	}
	width.Update(m,mouse.btn,mouse.last);
	height.Update(m,mouse.btn,mouse.last);
	depth.Update(m,mouse.btn,mouse.last);
	if(savePieces.Update(m,mouse.btn,mouse.last))
	{
		state=QUERY_NEW_PSET_NAME;
		if(pieces.GetSelElement())
			wcscpy(currPSet.setName,gg->current->currSet->setName);
		else
			currPSet.setName[0]=0;
		insertName.Init(lk->string[LK_EDIT_MESSAGE+5][0],currPSet.setName,NAME_LEN,200);
		return true;
	}
	if(delPieces.Update(m,mouse.btn,mouse.last)&&(pieces.GetSelElement()))
	{
		if(gg->CanDeletePieceSet(currPSet))
		{
			state=CONFIRM_DELETE_PSET;
			confirms.Init(
				lk->string[LK_EDIT_MESSAGE+6][0],
				lk->string[LK_EDIT_MESSAGE+6][1],Dialog::YESNO);
		} else
		{
			state=MESSAGE;
			confirms.Init(
				lk->string[LK_EDIT_MESSAGE+7][0],
				lk->string[LK_EDIT_MESSAGE+7][1],Dialog::OK);
		}
		return true;
	}

	char sA,sB; sA=width.pos; sB=height.pos;
	if(sB<sA)
	{
		sA=sB;
		sB=width.pos;
	}
	if((sA!=currGame.sideA)||(sB!=currGame.sideB)||(depth.pos!=currGame.depth))
	{
		currGame.sideA=sA;
		currGame.sideB=sB;
		currGame.depth=depth.pos;
		games.SelectRef(gg->GetGame(currGame));
	}

	if(exit.Update(m,mouse.btn,mouse.last))
	{
		Game::Get()->currMenu=new MainMenu;
		Game::Get()->currMenu->Init();
		delete this;
		return true;
	}

	currPiece=N_PIECES;
	unsigned char i;
	Box2 b2;
	for(i=0;i<N_PIECES;i++)
	{
		GetPieceBox(i,&b2);
		b2.inf.x+=w2D/2;	b2.inf.y=240-b2.inf.y;
		b2.sup.x+=w2D/2;	b2.sup.y=240-b2.sup.y;
		if(b2.IsInside(m))
		{
			currPiece=i;
			if(mouse.btn)
			{
				if(state==NONE)
				{
					if(currPSet.canAppear[i]) state=DEL_PIECE; else state=ADD_PIECE;
				}
				if(state==DEL_PIECE)
				{
					currPSet.canAppear[i]=false;

				}
				if(state==ADD_PIECE)
				{
					currPSet.canAppear[i]=true;
				}
				pieces.SelectRef(gg->GetPieceSet(currPSet));
				games.SelectRef(gg->GetGame(currGame));
			}
		}
	}
	if((!mouse.btn) &&((state==ADD_PIECE)||(state==DEL_PIECE)))
		state=NONE;
	return true;
}

void EditGame::GetPieceBox(const unsigned char i,Box2 *toRet)
{
	unsigned char x,y;
	if(i< 2) { y= 1 ;		x=i; } else
	if(i< 4) { y=i-1;		x=2; } else
	if(i<12) { y=(i-4)/2;	x=(i-4)%2+3; } else
	{
		y=(i-12)/8;
		x=(i-12)%8+5;
	}
	//x va da 0 a 12 e pos.x va da -300 a 300 cioé pos.x=-270+x*45;
	//y va da 0 a  3 e pos.y va da   60 a -180 cioé pos.y=30-y*60;
	//siccome ho deciso di fare distanza 0 pos.x diventa: pos.x=(-6+x)*60*5/7
	// con cioé pos.x=-257.14286+x*42.8571
	toRet->inf.x=(-6+x)*60*5/7;
	toRet->inf.y=25-y*60;
	if(i<2) toRet->inf.y-=30.f;

	toRet->sup=toRet->inf;
	toRet->inf-=V2D(30*5/7.f,-30);
	toRet->sup+=V2D(30*5/7.f,-30);

}

void EditGame::Draw3D()
{
	if(currCalc!=N_PIECES) return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float w2D=Font::Get2DScreenWidth()/2;
#define DIST 500.f
#define FACT (DIST-10-3.f*50/7)/DIST
	glFrustum(
		-w2D*FACT,w2D*FACT,
		-240*FACT,240*FACT,
		 DIST-10,DIST+100);
	glTranslatef(0,0,-DIST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLineWidth(1);
	/*
	glDisable(GL_TEXTURE_2D);
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	glVertex3f(-w2D,-240,-3);
	glVertex3f( w2D, 240,-3);
	glVertex3f(-w2D, 240,-3);
	glVertex3f( w2D,-240,-3);
	glVertex3f(-w2D,-240,-3);
	glVertex3f(-w2D, 240,-3);
	glVertex3f( w2D,-240,-3);
	glVertex3f( w2D, 240,-3);
	glVertex3f(-w2D,-240,-3);
	glVertex3f( w2D,-240,-3);
	glVertex3f(-w2D, 240,-3);
	glVertex3f( w2D, 240,-3);
	glEnd();//*/
	Matrix mat;
	mat.Reset();
	mat.m11=50/7.f;
	mat.m22=50/7.f;
	mat.m33=50/7.f;
	unsigned char i;
	Box2 b2;
	for(i=0;i<N_PIECES;i++)
	{
		GetPieceBox(i,&b2);
		mat.Pos().x=(b2.sup.x+b2.inf.x)/2;
		mat.Pos().y=(b2.sup.y+b2.inf.y)/2;
		mat.Pos().z=0;
		glLoadMatrixf(mat[0]);
		holes[i].Draw(false,false,false,false);
	}
}

void EditGame::Draw()
{
	Font *f=&Game::Get()->font;
	Rects2D *r=&Game::Get()->rects2D;
	Arrow2D *a=&Game::Get()->arrows;
	float w2D=Font::Get2DScreenWidth();
	float c=w2D/2;
	LocKit *lk=&Game::Get()->locKit;
	if(currCalc!=N_PIECES)
	{
		f->ShowWStringOnBox(lk->string[LK_EDIT+0][0],0,70,w2D,110,cBlack,Font::CENTER);
		Circles *ci=&Game::Get()->circles;
		ci->Draw(c,240,120);
		ci->DrawFill(c,240,115,0,3.1416f*2*currCalc/N_PIECES);
		f->Draw();
		return;
	}

	float s=f->ShowWStringOnBox(lk->string[LK_EDIT+1][0],c-280,5,c-180,25,cBlack,Font::LEFT);
	saveGame.Draw(f,r); delGame.Draw(f,r);
	r->Draw(c-300,13,c+300,65,c-285,c-275+s);

	s=f->ShowWStringOnBox(lk->string[LK_EDIT+2][0],c-280,70,c-180,90,cBlack,Font::LEFT);
	width.Draw(f,a);
	height.Draw(f,a);
	depth.Draw(f,a);
	f->ShowStringOnBox("x",c-70,90,c-40,110,cBlack,Font::CENTER);
	f->ShowStringOnBox("x",c+40,90,c+70,110,cBlack,Font::CENTER);
	r->Draw(c-300,78,c+300,120,c-285,c-275+s);

	s=f->ShowWStringOnBox(lk->string[LK_EDIT+3][0],c-280,125,c-180,145,cBlack,Font::LEFT);
	savePieces.Draw(f,r); delPieces.Draw(f,r);
	r->Draw(c-300,133,c+300,435,c-285,c-275+s);

	exit.Draw(f,r);

	unsigned char i;
	Box2 b2;
	for(i=0;i<N_PIECES;i++)
	{
		GetPieceBox(i,&b2);
		b2.inf.x+=c;	b2.inf.y=240-b2.inf.y;
		b2.sup.x+=c;	b2.sup.y=240-b2.sup.y;
		if(currPSet.canAppear[i]) r->Draw(b2.inf.x,b2.inf.y,b2.sup.x,b2.sup.y);
		if(i==currPiece)
		{
			r->Draw(b2.inf.x+3,b2.inf.y+3,b2.sup.x-3,b2.sup.y-3);
		}

	}


	pieces.Draw(f,r,a);
	games.Draw(f,r,a);

	f->Draw();

	if((state==QUERY_NEW_GAME_NAME)||(state==QUERY_NEW_PSET_NAME))
		insertName.Draw();
	if(state>=CONFIRM_DELETE_GAME)
	{
		confirms.Draw();
	}
}

void EditGame::Deinit()
{
	for(unsigned char i=0;i<N_PIECES;i++)
		holes[i].Deinit();
}
