#include "menuElements.h"
#include "Font.h"
#include <stdlib.h>
#include <string.h>

void Slider::Update(const V2D &mouse,bool mouseBtn,bool lastBtn)
{
	ul=bbox;
	br=bbox;
	text=bbox;
	float s;
	unsigned char i;
	if(vertical) i=1; else i=0;
	s=bbox.sup[1-i]-bbox.inf[1-i];
	text.inf[i]=ul.sup[i]=ul.inf[i]+s;
	text.sup[i]=br.inf[i]=br.sup[i]-s;
	if(mouseBtn)
	{
		if((!lastBtn)&&(bbox.IsInside(mouse)))
		{
			if(ul.IsInside(mouse)) state=CLICK_UL;
			if(br.IsInside(mouse)) state=CLICK_BR;
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
				if(state==CLICK_UL) if(ul.IsInside(mouse))  if(pos>minValue) pos-=stepValue;
				if(state==CLICK_BR) if(br.IsInside(mouse))  if(pos<maxValue) pos+=stepValue;

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
	} else state=NORMAL;
}

void Slider::Draw(Font *f,Arrow2D *a)
{
	char buff[20];
	itoa(pos,buff,10);
	if(withMax)
	{
		char b2[10];
		itoa(maxValue,b2,20);
		strcat(buff,"/");
		strcat(buff,b2);
	}
	f->ShowStringOnBox(buff,text.inf.x,text.inf.y,text.sup.x,text.sup.y,cBlack,Font::CENTER);
	if(vertical)
	{
		a->DrawUp(ul.inf.x+0,ul.inf.y+0,ul.sup.x-0,ul.sup.y-0,false,(state==CLICK_UL));
		a->DrawDown(br.inf.x+0,br.inf.y+0,br.sup.x-0,br.sup.y-0,false,(state==CLICK_BR));
	} else
	{
		a->DrawLeft(ul.inf.x+0,ul.inf.y+0,ul.sup.x-0,ul.sup.y-0,false,(state==CLICK_UL));
		a->DrawRight(br.inf.x+0,br.inf.y+0,br.sup.x-0,br.sup.y-0,false,(state==CLICK_BR));
	}
}
