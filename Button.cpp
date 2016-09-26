#include "menuElements.h"
#include "Font.h"

bool Button::Update(const V2D &mouse,bool mouseBtn,bool lastBtn)
{
	if(state==DISABLE) return false;
	state=NORMAL;
	if(bbox.IsInside(mouse))
	{
		state=SELECT;
		if(mouseBtn) state=PRESS;
		if(!mouseBtn && lastBtn) return true;
	}
	return false;
}

void Button::Draw(Font *f,Rects2D *r)
{
	Color c=cBlack; 
	if(state==SELECT) c=cWhite; 
	if(state==DISABLE) c=cGray;
	f->ShowWStringOnBox(text,
		bbox.inf.x+5,bbox.inf.y+5,
		bbox.sup.x-5,bbox.sup.y-5,c,Font::CENTER);
	if(state==SELECT) r->DrawFill(bbox.inf.x,bbox.inf.y,bbox.sup.x,bbox.sup.y);
	if(state==PRESS) r->Draw(bbox.inf.x,bbox.inf.y,bbox.sup.x,bbox.sup.y);
}
