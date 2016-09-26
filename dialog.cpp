#include "menus.h"
#include "systeminterface.h"
#include "game.h"

void Dialog::Init(wchar_t *msg1,wchar_t*msg2,Dialog::TYPE _t,float w)
{
	t=_t;
	message[0]=msg1;
	message[1]=msg2;
	Font *f=&Game::Get()->font;
	float w2D = Font::Get2DScreenWidth();
	float dim[2] = {0,0};
	if(message[0][0]) dim[0]=f->GetWStringWidth(message[0])*20;
	if(message[1] && message[1][0]) dim[1]=f->GetWStringWidth(message[1])*20;
	if(dim[1]>dim[0]) dim[0]=dim[1];
	if(w>dim[0]) dim[0]=w;
	dim[0]+=40;
	allDlg.inf.x=(w2D-dim[0])/2;
	allDlg.sup.x=(w2D+dim[0])/2;
	float h=35;
	if(message[1] && message[1][0]) h+=10;
	allDlg.inf.y=240-h;
	allDlg.sup.y=240+h;

	text[0].inf.x=allDlg.inf.x+20;
	text[0].sup.x=allDlg.sup.x-20;
	text[0].inf.y=allDlg.inf.y+10; text[0].sup.y=text[0].inf.y+20;
	if(message[1] && message[1][0])
	{
		text[1].inf.x=text[0].inf.x;
		text[1].sup.x=text[0].sup.x;
		text[1].inf.y=text[0].sup.y;
		text[1].sup.y=text[1].inf.y+20;
	}
	button[0].bbox.inf.x=allDlg.inf.x+15;
	button[0].bbox.sup.x=allDlg.sup.x-15;
	button[0].bbox.sup.y=button[1].bbox.sup.y=allDlg.sup.y-10;
	button[0].bbox.inf.y=button[1].bbox.inf.y=button[0].bbox.sup.y-25;
	LocKit *lk=&Game::Get()->locKit;
	if(t==OK) button[0].text=lk->string[LK_DIALOGS+2][0];
	if((t==YESNO)||(t==EDIT))
	{
		if(t==YESNO)
		{
			button[0].text=lk->string[LK_DIALOGS+0][0];;
			button[1].text=lk->string[LK_DIALOGS+1][0];;
		} else
		{
			button[0].text=lk->string[LK_DIALOGS+2][0];;
			button[1].text=lk->string[LK_DIALOGS+3][0];;
		}
		button[1].bbox.sup.x=button[0].bbox.sup.x;
		button[0].bbox.sup.x=w2D/2-5;
		button[1].bbox.inf.x=w2D/2+5;
	}
	move=false;
}

unsigned char Dialog::Update()
{
	unsigned char i,n;
	n=2; if(t==OK) n=1;
	V2D m;
	Font::ConvertMouse(&m.x,&m.y);
	if(move && mouse.btn)
	{
		start-=m;
		allDlg.inf-=start;
		allDlg.sup-=start;
		for(i=0;i<n;i++)
		{
			button[i].bbox.inf-=start;
			button[i].bbox.sup-=start;
		}
		text[0].inf-=start;
		text[0].sup-=start;
		text[1].inf-=start;
		text[1].sup-=start;
		start=m;
		return 0x0FF;
	}
	move=false;
	bool canMove=true;;
	for(i=0;i<n;i++)
	{
		if(button[i].Update(m,mouse.btn,mouse.last))
			return i;
		if(button[i].state!=Button::NORMAL)
			canMove=false;
	}
	if(canMove && !mouse.last && mouse.btn && allDlg.IsInside(m))
	{
		move=true;
		start=m;
	}
	mouse.last=mouse.btn;
	return 0x0FF;
}

void Dialog::Draw()
{
	Font *f=&Game::Get()->font;
	Rects2D *r=&Game::Get()->rects2D;

	r->DrawGlassShadow(allDlg.inf.x,allDlg.inf.y,allDlg.sup.x,allDlg.sup.y);
	r->DrawGlass(allDlg.inf.x,allDlg.inf.y,allDlg.sup.x,allDlg.sup.y);
	r->DrawTube(allDlg.inf.x,allDlg.inf.y,allDlg.sup.x,allDlg.sup.y);

	f->ShowWStringOnBox(message[0],text[0].inf.x,text[0].inf.y,text[0].sup.x,text[0].sup.y,cBlack,Font::CENTER);
	if(message[1] && message[1][0])
		f->ShowWStringOnBox(message[1],text[1].inf.x,text[1].inf.y,text[1].sup.x,text[1].sup.y,cBlack,Font::CENTER);

	button[0].Draw(f,r);
	if(t!=OK)
		button[1].Draw(f,r);

	f->Draw();
}

void Dialog::DeInit()
{

}

void DialogEdit::Init(wchar_t *msg1,wchar_t *_dest,unsigned char len,float w)
{
	Dialog::Init(msg1,L"p",Dialog::EDIT,w);
	dest=_dest;
	message[1]=0;
	edit.bbox=text[1];
	edit.Init(len);
	edit.SetText(dest);
	edit.state=EditBox::EDIT;
}

unsigned char DialogEdit::Update()
{
	V2D m;
	Font::ConvertMouse(&m.x,&m.y);
	edit.bbox=text[1];
	edit.Update(m,mouse.btn);
	unsigned char r=0x0FF;
	if(edit.state!=EditBox::SELECTING)
		r=Dialog::Update();
	for(char i=0;i<KB_QUEUE_LEN;i++)
	{
		if(keyboardInput[i]==13)
			r=0;
		if(keyboardInput[i]==10)
			r=0;
		if(keyboardInput[i]==27)
			r=1;
	}
	if(r==0)
		wcscpy(dest,edit.GetText());
	return r;
}

void DialogEdit::Draw()
{
	Font *f=&Game::Get()->font;
	Rects2D *r=&Game::Get()->rects2D;
	Dialog::Draw();
	edit.Draw(f,r);
	f->Draw();
}

void DialogEdit::DeInit() {}
