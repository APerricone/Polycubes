#include "menuElements.h"
#include "Font.h"

void ScrollBar::Update(const V2D &mouse,bool mouseBtn,bool lastBtn)
{
	ul=bbox;
	br=bbox;
	bar=bbox;
	float s;
	unsigned char i;
	if(vertical) i=1; else i=0;
	s=bbox.sup[1-i]-bbox.inf[1-i];
	bar.inf[i]=ul.sup[i]=ul.inf[i]+s;
	bar.sup[i]=br.inf[i]=br.sup[i]-s;
	float barSize=bar.sup[i]-bar.inf[i];
	cursor=bar; 
	if(total!=0)
	{
		cursor.inf[i]=bar.inf[i]+barSize*pos/total;
		cursor.sup[i]=bar.inf[i]+barSize*(pos+size)/total;
	}
	cursor.inf[1-i]+=2;
	cursor.sup[1-i]-=2;
	if(mouseBtn)
	{
		if((!lastBtn)&&(bbox.IsInside(mouse)))
		{
			if(ul.IsInside(mouse)) state=CLICK_UL;
			if(br.IsInside(mouse)) state=CLICK_BR;
			if(cursor.IsInside(mouse)) { deltaPos=mouse[i]-cursor.inf[i]; state=MOVING; }
			if((state==NORMAL) && (bar.IsInside(mouse)))
			{
				if(mouse[i]<cursor.inf[i]) pos=0;
				if(mouse[i]>cursor.sup[i]) pos=total-size;
			}
			if((state==CLICK_UL) || (state==CLICK_BR))
			{
				time=29;
				currLimit=30;
				counterVel=60;
			}
		}
		if((state==CLICK_UL) || (state==CLICK_BR))
		{
			time++;
			if(time==currLimit)
			{
				if(state==CLICK_UL) if(ul.IsInside(mouse))  if(pos>0) pos--;
				if(state==CLICK_BR) if(br.IsInside(mouse))  if(pos<total-size) pos++;
				
				time=0;
				counterVel-=currLimit;
				if(counterVel<=0)
				{
					currLimit/=2;
					if(currLimit==0) currLimit=1;
					counterVel+=60;
				}
			}
		}
		if(state==MOVING)
		{
			cursor.inf[i]=mouse[i]-deltaPos;
			if(cursor.inf[i]<bar.inf[i]) cursor.inf[i]=bar.inf[i];
			if(total)
				cursor.sup[i]=cursor.inf[i]+barSize*size/total;
			else
				cursor.sup[i]=cursor.inf[i]+barSize;
			if(cursor.sup[i]>bar.sup[i])
			{
				cursor.sup[i]=bar.sup[i];
				if(total)
					cursor.inf[i]=cursor.sup[i]-barSize*size/total;
				else
					cursor.inf[i]=cursor.sup[i]-barSize;
			}
			pos=(cursor.inf[i]-bar.inf[i])*total/barSize;
			if(pos<0) pos=0;
			if(pos>total-size) pos=total-size;
		}
	} else state=NORMAL;
}

void ScrollBar::Draw(Rects2D *r,Arrow2D *a)
{
	r->Draw(bbox.inf.x,bbox.inf.y,bbox.sup.x,bbox.sup.y);
	if(state==CLICK_UL)
		r->DrawFill(ul.inf.x,ul.inf.y,ul.sup.x,ul.sup.y);
	else
		r->Draw(ul.inf.x,ul.inf.y,ul.sup.x,ul.sup.y);
	if(state==CLICK_BR)
		r->DrawFill(br.inf.x,br.inf.y,br.sup.x,br.sup.y);
	else
		r->Draw(br.inf.x,br.inf.y,br.sup.x,br.sup.y);
	if(state==MOVING)
		r->DrawFill(cursor.inf.x,cursor.inf.y,cursor.sup.x,cursor.sup.y);
	else
		r->Draw(cursor.inf.x,cursor.inf.y,cursor.sup.x,cursor.sup.y);

	if(vertical)
	{
		a->DrawUp(ul.inf.x+5,ul.inf.y+5,ul.sup.x-5,ul.sup.y-5,(state==CLICK_UL));
		a->DrawDown(br.inf.x+5,br.inf.y+5,br.sup.x-5,br.sup.y-5,(state==CLICK_BR));
	} else
	{
		a->DrawLeft(ul.inf.x+5,ul.inf.y+5,ul.sup.x-5,ul.sup.y-5,(state==CLICK_UL));
		a->DrawRight(br.inf.x+5,br.inf.y+5,br.sup.x-5,br.sup.y-5,(state==CLICK_BR));
	}
}
