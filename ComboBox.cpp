#include "menuElements.h"
#include "Font.h"
#include "systemInterface.h"
#include "game.h"

ComboBox::ComboBox()
{
	list=0;
	actual=0;
	nList=0;
	state=NORMAL;
	nameWithOutSel=0;
}

ComboBox::~ComboBox()
{
	ResetList();
}

void ComboBox::ResetList()
{
	nList=0;
	while(list)
	{
		actual=list->next;
		delete list;
		list=actual;
	}
}

void ComboBox::AddElementToList(wchar_t *_name,void *_ref)
{
	ListElement *toAdd=new ListElement();
	toAdd->name=_name;
	toAdd->ref=_ref;
	toAdd->next=0;
	if(list)
	{
		ListElement *prec=0,*succ=list;
		//find before one
		while(succ)
		{
			if(wcscmp(_name,succ->name)<=0)
				break;
			prec=succ;
			succ=succ->next;
		}
		if(prec)
		{
			toAdd->next=prec->next;
			prec->next=toAdd;
		} else
		{
			toAdd->next=list;
			list=toAdd;
		}

	} else
	{
		list=toAdd;
	}
	nList++;
}

ComboBox::ListElement *ComboBox::Get(unsigned char n)
{
	unsigned char c=0;
	ListElement *toR=list;
	while(c!=n)
	{
		if(toR==0) return 0;
		c++;
		toR=toR->next;
	}
	return toR;
}

ComboBox::ListElement *ComboBox::Get(wchar_t *name)
{
	ListElement *toR=list;
	while(1)
	{
		if(toR==0) return 0;
		if(wcsicmp(name,toR->name)==0) return toR;
		toR=toR->next;
	}
	return 0;
}

ComboBox::ListElement *ComboBox::GetRef(void *ref)
{
	ListElement *toR=list;
	while(1)
	{
		if(toR==0) return 0;
		if(toR->ref==ref) return toR;
		toR=toR->next;
	}
	return 0;
}

unsigned char ComboBox::GetIdx(ListElement *ele)
{
	unsigned char c=0;
	ListElement *user=list;
	while(user!=ele)
	{
		if(user==0) return nList;
		c++;
		user=user->next;
	}
	return c;
}


unsigned char ComboBox::Update(const V2D &mouse,bool mouseBtn,bool lastBtn)
{
	if(state==DISABLE) return 0;
	if(state==NORMAL)
	{
		if((lastBtn) && (!mouseBtn) && (bbox.IsInside(mouse)))
		{
			state=OPEN;
			sel=actual;
			bboxOpen.inf.x=bbox.inf.x;
			bboxOpen.sup.x=bbox.sup.x-24;
			bboxOpen.inf.y=bbox.sup.y;
			bboxOpen.sup.y=bottomY;
			scrollBar.bbox.sup.x=bbox.sup.x-3;
			scrollBar.bbox.inf.x=bboxOpen.sup.x;
			scrollBar.bbox.inf.y=bboxOpen.inf.y-2;
			scrollBar.bbox.sup.y=bboxOpen.sup.y-4;
			scrollBar.total=nList;
			scrollBar.size=(scrollBar.bbox.sup.y-scrollBar.bbox.inf.y-10)/20.f;
			if(scrollBar.size>scrollBar.total)
				scrollBar.size=scrollBar.total;
			scrollBar.pos=GetIdx(actual)-scrollBar.size/2;
			scrollBar.vertical=true;
			if(scrollBar.pos<0) scrollBar.pos=0;
			if(scrollBar.pos>scrollBar.total-scrollBar.size)
				scrollBar.pos=scrollBar.total-scrollBar.size;
			scrollBar.Update(mouse,false,false);
			return 1;
		}
	}
	if(state==OPEN)
	{
		if(bboxOpen.IsInside(mouse))
		{
			sel=Get(scrollBar.pos+(mouse.y-bboxOpen.inf.y)/20);
			if(mouseBtn && sel) actual=sel;
		}
		if((lastBtn) && (!mouseBtn) && (scrollBar.state==ScrollBar::NORMAL))
		{
			//if(sel) actual=sel;
			state=NORMAL;
			return 2;
		}
		scrollBar.Update(mouse,mouseBtn,lastBtn);
		return 1;
	}
	return 0;
}

void ComboBox::Del(ListElement *ele)
{
	if(ele==0) return;
	if(ele==list)
	{
		list=ele->next;
	} else
	{
		ListElement *pr=list;
		while(pr->next!=ele) pr=pr->next;
		pr->next=ele->next;
	}
	delete ele;
	nList--;
}

void ComboBox::Draw(Font *f,Rects2D *r,Arrow2D *a)
{
	if(state==OPEN)
	{
		f->Draw();
		r->DrawGlassShadow(bbox.inf.x-3,bbox.inf.y-3,bbox.sup.x+3,bottomY+3);
		r->DrawGlass(bbox.inf.x-3,bbox.inf.y-3,bbox.sup.x+3,bottomY+3);
		r->DrawTube(bbox.inf.x-3,bbox.inf.y-3,bbox.sup.x+3,bottomY+3);
	}
	r->Draw(bbox.inf.x,bbox.inf.y,bbox.sup.x,bbox.sup.y);
	r->Draw(bbox.sup.x-30,bbox.inf.y,bbox.sup.x,bbox.sup.y);
	if(state==NORMAL) a->DrawDown(bbox.sup.x-25,bbox.inf.y+5,bbox.sup.x-5,bbox.sup.y-5);
	if(actual)
		f->ShowWStringOnBox(actual->name,bbox.inf.x+5,bbox.inf.y+5,bbox.sup.x-5,bbox.sup.y-5,cBlack,Font::LEFT);
	else
		if(nameWithOutSel)
			f->ShowWStringOnBox(nameWithOutSel,bbox.inf.x+5,bbox.inf.y+5,bbox.sup.x-5,bbox.sup.y-5,cBlack,Font::LEFT);
	if(state==OPEN)
	{
		scrollBar.Draw(r,a);
		a->DrawUp(bbox.sup.x-25,bbox.inf.y+5,bbox.sup.x-5,bbox.sup.y-5);
		ListElement *user=Get(scrollBar.pos);
		float currY=bbox.sup.y+5;
		for(unsigned char i=0;i<scrollBar.size;i++)
		{
			if(user==0) break;
			if(sel==user)
			{
				r->DrawFill(bboxOpen.inf.x+5,currY,bboxOpen.sup.x-5,currY+20);
				f->ShowWStringOnBox(user->name,bboxOpen.inf.x+5,currY,bboxOpen.sup.x-5,currY+20,cWhite,Font::LEFT);
			} else
			{
				f->ShowWStringOnBox(user->name,bboxOpen.inf.x+5,currY,bboxOpen.sup.x-5,currY+20,cBlack,Font::LEFT);
			}
			currY+=20;
			user=user->next;
		}
	}
}
