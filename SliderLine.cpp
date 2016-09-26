#include "menuElements.h"
#include "game.h"

void SliderLine::Update(const V2D &mouse,bool mouseBtn,bool lastBtn)
{
	if(state==NORMAL)
	{
		if((mouseBtn)&&(!lastBtn)&&(bbox.IsInside(mouse)))
		{
			state=MOVING;
		}
	} else
	{
		if((!mouseBtn)&&(lastBtn))
		{
			state=NORMAL;
		}
	}
	Box2 tmp=bbox;
	tmp.inf.x-=20;
	tmp.sup.x+=20;
	if((state==MOVING)&&(tmp.IsInside(mouse)))
	{
		if(bbox.IsInside(mouse))
			pos=size*(mouse.x-bbox.inf.x)/(bbox.sup.x-bbox.inf.x);
		else
			if(mouse.x<bbox.inf.x)
				pos=0;
			else
				pos=size;
	}
}

void SliderLine::Draw(Rects2D *r)
{
	float c=(bbox.inf.y+bbox.sup.y)/2;
	r->DrawFill(bbox.inf.x-5,c-2,bbox.sup.x+5,c+2);
	float p=bbox.inf.x+(bbox.sup.x-bbox.inf.x)*(float)pos/(float)size;
	if(state==NORMAL)
		r->DrawFill(p-5,bbox.inf.y,p+5,bbox.sup.y);
	else
		r->Draw(p-5,bbox.inf.y,p+5,bbox.sup.y);
	glDisable(GL_TEXTURE_2D);
	glColor4f(0,0,0,1);
	glBegin(GL_LINES);
	for(long i=0;i<=size;i+=steps)
	{
		p=bbox.inf.x+(bbox.sup.x-bbox.inf.x)*(float)i/(float)size;
		glVertex2f(p,bbox.inf.y); glVertex2f(p,c-3);
	}
	glEnd();
}
