#include "menuElements.h"
#include "Font.h"
#include "systemInterface.h"
#include "game.h"

EditBox *EditBox::currEdit=0;

void EditBox::Init(unsigned char _len)
{
	if(len!=0)
	{
		SAFE_DELETE_ARRAY(text);
	}
	len=_len;
	text=new wchar_t[len+1];
	text[0]=0;
	cursorStart=0;
	cursorEnd=0;
	currentLen=0;
	howMove=0;
}

void EditBox::SetText(const wchar_t *_text)
{
	text[0]=0; Paste(_text);
	cursorStart=0;
	cursorEnd=currentLen;
}

void EditBox::Paste(const wchar_t *_text)
{
	Font *f=&Game::Get()->font;
	long toCopyLen=f->GetWStringLen(_text);
	if(toCopyLen==0) return;
	wchar_t *tPasteTmp=new wchar_t[toCopyLen+1];
	wchar_t *srcTmp=new wchar_t[currentLen+1];
	wcscpy(srcTmp,text);
	wchar_t *d=tPasteTmp;
	const wchar_t *s=_text;
	while(*s)
	{
		if(f->IsOnSetW(*s)) *d=*s;
		if(*s == 16) { d++; break; }
		if(*s == 10) { d++; break; }
		s++; d++;
	}
	*d=0;
	if(cursorStart>currentLen+1) cursorStart=currentLen+1;
	currentLen=toCopyLen+currentLen-cursorEnd+cursorStart;
	if(currentLen>len)
	{
		toCopyLen-=currentLen-len;
		currentLen=len;
	}
	long i;
	for(i=currentLen;i>=0;i--)
	{
		if(i<cursorStart) break; //prima del cursore non cambia
		if(i<cursorStart+toCopyLen)
		{
			text[i]=tPasteTmp[i-cursorStart];
			continue;
		}
		text[i]=srcTmp[i-toCopyLen+cursorEnd-cursorStart];
	}
	delete [] tPasteTmp;
	delete [] srcTmp;
	cursorStart+=toCopyLen;
	cursorEnd=cursorStart;
}

void EditBox::Update(const V2D &mouse,bool mouseBtn)
{
	if(state==DISABLE) return;
	if(state!=SELECTING && bbox.IsInside(mouse) && mouseBtn)
	{
		cursorStart=cursorEnd=GetChar(mouse.x);
		state=SELECTING;
		currEdit=this;
		return;
	}
	if(state==SELECTING)
	{
		currEdit=this;
		if(!mouseBtn) state=EDIT;
		if(keyboardInput[0]) state=EDIT;
		if(state!=EDIT)
		{
			unsigned char newLimit=GetChar(mouse.x);
			if(newLimit<cursorStart) cursorStart=newLimit;
			if(newLimit>cursorEnd) cursorEnd=newLimit;
			if((newLimit>cursorStart)&&(newLimit<cursorEnd))
			{
				if(newLimit<(cursorStart+cursorEnd)/2)
					cursorStart=newLimit; else
					cursorEnd=newLimit;
			}
		}
	}
	if(state==EDIT)
	{
		currEdit=this;
		Font *f=&Game::Get()->font;
		wchar_t toP[2]; toP[1]=0;
		if(!bbox.IsInside(mouse) && mouseBtn) { state=NORMAL; currEdit=0; return; }
		for(char i=0;i<KB_QUEUE_LEN;i++)
		{
			if(keyboardInput[i]==13) { state=NORMAL; currEdit=0; return; }
			if(keyboardInput[i]==10) { state=NORMAL; currEdit=0; return; }
			if(keyboardInput[i]==27) { state=NORMAL; currEdit=0; return; }
			// undo not implemented... (27=ESC)
			if( (f->IsOnSetW(keyboardInput[i])) && (!specialKey[i]) )
			{
				toP[0]=keyboardInput[i]; Paste(toP);
			}
			if(keyboardInput[i]==VK_LEFT && specialKey[i])
			{
				if(howMove==0) howMove=1; //start/home
				if(howMove==1)
				{
					if(cursorStart) cursorStart--;
					if(!IsShiftPress()) cursorEnd=cursorStart;
				} else
				{
					if(cursorEnd) cursorEnd--;
					if(cursorEnd<cursorStart) { cursorStart=cursorEnd; cursorEnd++; howMove=1; }
					if(!IsShiftPress()) cursorStart=cursorEnd;
				}
				if(!IsShiftPress()) howMove=0;
			}
			if(keyboardInput[i]==VK_RIGHT && specialKey[i])
			{
				if(howMove==0) howMove=2; //end
				if(howMove==1)
				{
					cursorStart++;
					if(cursorStart>currentLen) cursorStart--;
					if(cursorStart>cursorEnd) { cursorEnd=cursorStart; cursorStart--; howMove=2; }
					if(!IsShiftPress()) cursorEnd=cursorStart;
				} else
				{
					cursorEnd++;
					if(cursorEnd>currentLen) cursorEnd--;
					if(!IsShiftPress()) cursorStart=cursorEnd;
				}
				if(!IsShiftPress()) howMove=0;
			}
			if(keyboardInput[i]==VK_HOME && specialKey[i])
			{
				cursorStart=0;
				if(!IsShiftPress()) cursorEnd=cursorStart;
			}
			if(keyboardInput[i]==VK_END && specialKey[i])
			{
				cursorEnd=currentLen;
				if(!IsShiftPress()) cursorStart=cursorEnd;
			}
			if(cursorEnd!=cursorStart)
			{
				if((keyboardInput[i]==VK_DELETE && specialKey[i]) || (keyboardInput[i]==VK_BACK))
				{
					wcsncpy(&text[cursorStart],&text[cursorEnd],currentLen-cursorEnd+1);
					currentLen-=cursorEnd-cursorStart;
					cursorEnd=cursorStart;
				}
			}
			if(keyboardInput[i]==VK_DELETE && specialKey[i])
				if(cursorEnd==cursorStart)
				{
					if(cursorStart!=currentLen)
					{
						wcsncpy(&text[cursorStart],&text[cursorStart+1],currentLen-cursorStart);
						currentLen--;
					}
				}
			if(keyboardInput[i]==VK_BACK)
				if(cursorEnd==cursorStart)
				{
					if(cursorStart>0)
					{
						wcsncpy(&text[cursorStart-1],&text[cursorStart],currentLen-cursorStart+1);
						cursorEnd=--cursorStart;
						currentLen--;
					}
				}
		}
	}
}

unsigned char EditBox::GetChar(float px)
{
	if(px<bbox.inf.x) return 0;
	Font *f=&Game::Get()->font;
	float cx;
	for(unsigned i=1;i<currentLen;i++)
	{
		cx=f->GetPosWCharOnStringOnBox(text,bbox.inf.x,bbox.inf.y,bbox.sup.x,
			bbox.sup.y,Font::LEFT,i);
		if(cx>px) return i-1;
	}
	return currentLen;
}

void EditBox::Draw(Font *f,Rects2D *r)
{
	Color c=cBlack;
	if(state==DISABLE) c=cGray;
	f->ShowWStringOnBox(text,bbox.inf.x,bbox.inf.y,bbox.sup.x,bbox.sup.y,c,Font::LEFT);
	if(state==NORMAL) return;
	float cx=f->GetPosWCharOnStringOnBox(text,bbox.inf.x,bbox.inf.y,bbox.sup.x,
		bbox.sup.y,Font::LEFT,cursorStart);
	if(cursorEnd!=cursorStart)
	{
		float ex=f->GetPosWCharOnStringOnBox(text,bbox.inf.x,bbox.inf.y,bbox.sup.x,bbox.sup.y,
			Font::LEFT,cursorEnd);

		glDisable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0,0,0);
		glVertex3f(cx,bbox.inf.y,0);
		glVertex3f(cx,bbox.sup.y,0);
		glVertex3f(ex,bbox.inf.y,0);
		glVertex3f(ex,bbox.sup.y,0);
		glEnd();

		wchar_t exC=text[cursorEnd];
		text[cursorEnd]=0;
		f->ShowWStringOnBox(&text[cursorStart],cx,bbox.inf.y,ex,bbox.sup.y,cWhite,Font::LEFT);
		text[cursorEnd]=exC;
	} else
	{
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINES);
		glColor3f(0,0,0);
		glVertex3f(cx,bbox.inf.y,0);
		glVertex3f(cx,bbox.sup.y,0);
		glEnd();
	}
}
