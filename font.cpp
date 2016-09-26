#include "font.h"
#include "image.h"
#include "systeminterface.h"

#include "game.h"

#include <stdio.h>
#include <string.h>

Font::Font()
{
	sets=0;
	chars=0;
	spaceScale=0.25f;
	indices=0;
}

Font::~Font()
{
	Destroy();
}

void Font::Destroy()
{
	Set *s;
	while(sets)
	{
		s=sets->next;
		delete sets->image;
		delete sets;
		sets=s;
	}
	Character *c;
	while(chars)
	{
		c=chars->next;
		delete chars;
		chars=c;
	}
	SAFE_DELETE_ARRAY(indices);
}

Font::Set *Font::AddSet()
{
	Set *toAdd=new Set();
	toAdd->next=sets;
	sets=toAdd;
	return toAdd;
}

Font::Character *Font::AddCharacter(wchar_t cc)
{
	Character *ck=chars;
	while(ck)
	{
		if(ck->character==cc)
		{
			message("Font::AddCharacter","charattere %C (0x0%hx) ripetuto",cc,cc);
			return ck;
		}
		ck=ck->next;
	}
	Character *toAdd=new Character();
	toAdd->character=cc;
	toAdd->next=chars;
	chars=toAdd;
	return toAdd;
}

void Font::Load(char *fontDir,unsigned short maxChar)
{
	SAFE_DELETE_ARRAY(indices);
	maxCharSet=maxChar;
	unsigned short i;
	indices=new unsigned short[maxChar*6];
	for(i=0;i<maxChar;i++)
	{
		indices[i*6+0]=i*4+0;
		indices[i*6+1]=i*4+1;
		indices[i*6+2]=i*4+2;
		indices[i*6+3]=i*4+2;
		indices[i*6+4]=i*4+1;
		indices[i*6+5]=i*4+3;
	}

	int dirLen=strlen(fontDir)+21;
	char *buff=new char[dirLen];
	sprintf(buff,"%s\\font.txt",fontDir);
	FILE *f=fopen(buff,"rb");
	if(f==0)
	{
		message("Font::Load","file %s don't found",buff);
		delete [] buff;
		return;
	}
	unsigned short bom;
	fread(&bom,2,1,f);
	if(bom!=0x0FEFF)
	{
		message("Font::Load","file %s is not an unicode little-endian with Byte Order Mark",buff);
		fclose(f);
		delete [] buff;
		return;
	}
	wchar_t wbuff[256],*wb;
	do
	{
		fgetws(wbuff,256,f);
		if(feof(f)) break;
		if(wbuff[0]==0) continue;
		if(wbuff[0]==L'\n') continue;
		if(wbuff[0]==L'\r') continue;
		wb=&wbuff[wcslen(wbuff)-1];
		if((*wb)==L'\n') { (*wb)=0; wb--; } if((*wb)==L'\r') { (*wb)=0; wb--; }
		sprintf(buff,"%s\\%S",fontDir,wbuff);

		Set *newSet=AddSet();
		newSet->image=new Image();
		if(!newSet->image->LoadPNG(buff))
		{
			fclose(f);
			delete [] buff;
			return;
		}
		fgetws(wbuff,256,f);
		InitSet(newSet,wbuff);
	} while(1);
	fclose(f);
	delete [] buff;
};

void Font::InitSet(Set *toInit,wchar_t *charList)
{
	toInit->nChars=0;
	toInit->vertices=new TCV[maxCharSet*4];
	Color refColor=toInit->image->GetPixel(0,0);
	//trovo l'altezza dei caratteri
	unsigned short hei=1;
	while(toInit->image->GetPixel(0,hei)!=refColor)
	{
		hei++;
		if(hei>=toInit->image->dimY) return;
	}
	unsigned short x,y,exx;
	wchar_t *curChar=charList;
	x=0; y=0;
	while(*curChar)
	{
		if(*curChar==13) break;
		if(*curChar==10) break;
		exx=x++;
		while(toInit->image->GetPixel(x,y)!=refColor)
		{
			x++;
			if(x>=toInit->image->dimX) break;
		}
		Character *newchar=AddCharacter(*curChar);
		newchar->set=toInit;
		newchar->ul.x=(float)exx/(float)toInit->image->dimX;
		newchar->ul.y=(float)y/(float)toInit->image->dimX;
		newchar->br.x=(float)(x-1)/(float)toInit->image->dimX;
		newchar->br.y=(float)(y+hei-1)/(float)toInit->image->dimX;
		newchar->aspectRatio=(newchar->br.x-newchar->ul.x)/(newchar->br.y-newchar->ul.y);

		if(toInit->image->GetPixel(x+1,y)==refColor)
		{
			x=0; y+=hei;
			if(hei>=toInit->image->dimY) break;
		}
		curChar++;
	}
	Image *nI=new Image;
	nI->dimX=toInit->image->dimX;
	nI->dimY=toInit->image->dimY;
	nI->format=Image::GRAYALPHA;
	nI->Create();
	unsigned char alpha;
	Color c;
	for(y=0;y<nI->dimY;y++)
		for(x=0;x<nI->dimX;x++)
		{
			c=toInit->image->GetPixel(x,y);
			if(c==refColor) alpha=0; else alpha=255-((short)c.r+(short)c.g+(short)c.b)/3;
			c.Set(255,255,255,alpha);
			nI->SetPixel(x,y,c);
		}
	delete toInit->image;
	toInit->image=nI;
	toInit->image->CreateTxt();
}

#ifdef _WINDOWS
#undef GetCharWidth
#endif

float Font::GetCharWidth(char c)
{
	if(c==' ') return spaceScale;
	return GetWCharWidth((wchar_t)c);
}

float Font::GetWCharWidth(wchar_t c)
{
	if(c==L' ') return spaceScale;
	Character *uc=chars;
	while(uc)
	{
		if(uc->character==c)
		{
			float r=uc->aspectRatio;
			return r;
		}
		uc=uc->next;
	} // while(uc)
	return 0;
}

void Font::SetSpaceScale(float s) { spaceScale=s; }
void Font::SetSpaceScaleLikeChar(char c) { SetSpaceScaleLikeWChar((wchar_t)c); }
void Font::SetSpaceScaleLikeWChar(wchar_t c)
{
	if(c==L' ') return;
	Character *uc=chars;
	while(uc)
	{
		if(uc->character==c)
		{
			spaceScale=uc->aspectRatio;
			return;
		}
		uc=uc->next;
	} // while(uc)
	spaceScale=0.25f;
}

bool Font::IsOnSet(char c) { return IsOnSetW((wchar_t)c); }
bool Font::IsOnSetW(wchar_t c)
{
	if(c==L' ') return true;
	Character *uc=chars;
	while(uc)
	{
		if(uc->character==c)
			return true;
		uc=uc->next;
	} // while(uc)
	return false;
}

float Font::GetStringWidth(char *string)
{
	float r=0;
	char *u=string;
	while(*u)
	{
		r+=GetCharWidth(*u);
		u++;
	} // while(*u)
	return r;
}

float Font::GetWStringWidth(wchar_t *string)
{
	float r=0;
	wchar_t *u=string;
	while(*u)
	{
		r+=GetWCharWidth(*u);
		u++;
	} // while(*u)
	return r;
}

float Font::ShowStringOnBox(char *string,float x0, float y0,float x1, float y1,const Color &col,JUST just)
{
	float sx=x0;
	float sy=y0;
	float h=y1-y0;
	float is=0;
	float w=GetStringWidth(string)*h;
	if(w>x1-x0) { is=(w-x1+x0)/(strlen(string)-1); w=x1-x0; }
	switch(just)
	{
		case CENTER:
			sx=(x0+x1-w)*0.5f;
			break;
		case RIGHT:
			sx=x1-w;
			break;
		case JUSTIFIED:
			if(is==0)
				is=(w-x1+x0)/(strlen(string)-1);
			break;
	}
	char *cc=string;
	while(*cc)
	{
		sx+=AddCharToBuffer(*cc,sx,y1,h,col)-is;
		cc++;
	}
	return w;
}

float Font::ShowWStringOnBox(wchar_t *string,float x0, float y0,float x1, float y1,const Color &col,JUST just)
{
	float sx=x0;
	float sy=y0;
	float h=y1-y0;
	float is=0;
	float w=GetWStringWidth(string)*h;
	if(w>x1-x0) { is=(w-x1+x0)/(wcslen(string)-1); w=x1-x0; }
	switch(just)
	{
		case CENTER:
			sx=(x0+x1-w)*0.5f;
			break;
		case RIGHT:
			sx=x1-w;
			break;
		case JUSTIFIED:
			if(is==0)
				is=(w-x1+x0)/(wcslen(string)-1);
			break;
	}
	wchar_t *cc=string;
	while(*cc)
	{
		sx+=AddWCharToBuffer(*cc,sx,y1,h,col)-is;
		cc++;
	}
	return w;
}

float Font::GetPosCharOnStringOnBox(char *string,float x0, float y0,float x1, float y1,JUST just,unsigned char id)
{
	float sx=x0;
	float sy=y0;
	float h=y1-y0;
	float is=0;
	float w=GetStringWidth(string)*h;
	if(w>x1-x0) { is=(w-x1+x0)/(strlen(string)-1); w=x1-x0; }
	switch(just)
	{
		case CENTER:
			sx=(x0+x1-w)*0.5f;
			break;
		case RIGHT:
			sx=x1-w;
			break;
		case JUSTIFIED:
			if(is==0)
				is=(w-x1+x0)/(strlen(string)-1);
			break;
	}
	char *cc=string;
	unsigned char i=0;
	while(*cc)
	{
		if(i==id) return sx;
		sx+=GetCharWidth(*cc)*h-is;
		cc++; i++;
	}
	return sx;
}

float Font::GetPosWCharOnStringOnBox(wchar_t *string,float x0, float y0,float x1, float y1,JUST just,unsigned char id)
{
	float sx=x0;
	float sy=y0;
	float h=y1-y0;
	float is=0;
	float w=GetWStringWidth(string)*h;
	if(w>x1-x0) { is=(w-x1+x0)/(wcslen(string)-1); w=x1-x0; }
	switch(just)
	{
		case CENTER:
			sx=(x0+x1-w)*0.5f;
			break;
		case RIGHT:
			sx=x1-w;
			break;
		case JUSTIFIED:
			if(is==0)
				is=(w-x1+x0)/(wcslen(string)-1);
			break;
	}
	wchar_t *cc=string;
	unsigned char i=0;
	while(*cc)
	{
		if(i==id) return sx;
		sx+=GetWCharWidth(*cc)*h-is;
		cc++; i++;
	}
	return sx;
}

void Font::CleanDraw()
{
	Set *u=sets;
	while(u)
	{
		u->nChars=0;
		u=u->next;
	}; // while(u)
}

float Font::AddCharToBuffer(char c,float px,float py,float height,const Color &col)
{
	if(c==' ') return height*spaceScale;
	return AddWCharToBuffer((wchar_t)c,px,py,height,col);
}

float Font::AddWCharToBuffer(wchar_t c,float px,float py,float h,const Color &col)
{
	if(c==L' ') return h*spaceScale;
	Character *cc=chars;
	if(cc==0) return 0;
	while(cc->character!=c)
	{
		cc=cc->next;
		if(cc==0) return 0;
	} // while(cc->character!=c)
	float w=cc->aspectRatio*h;
	if(cc->set->nChars>=maxCharSet) return w;
	TCV *dest=&(cc->set->vertices[cc->set->nChars<<2]);
	dest[0].p.Set(px  ,py  ,0.5f);
	dest[1].p.Set(px+w,py  ,0.5f);
	dest[2].p.Set(px  ,py-h,0.5f);
	dest[3].p.Set(px+w,py-h,0.5f);
	dest[0].c=dest[1].c=dest[2].c=dest[3].c=col;
	dest[0].t.Set(cc->ul.x,cc->br.y);	dest[1].t.Set(cc->br.x,cc->br.y);
	dest[2].t.Set(cc->ul.x,cc->ul.y);	dest[3].t.Set(cc->br.x,cc->ul.y);
	cc->set->nChars++;
	return w;
}

float Font::Get2DScreenWidth()
{
	return 480.f*Game::Get()->RenderDimX/Game::Get()->RenderDimY;
}

void Font::SetupMatricesFor2D()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float dimX=Get2DScreenWidth();
	glOrtho(0,dimX,480,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Font::Draw(bool clean)
{
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	Set *u=sets;
	while(u)
	{
		if(u->nChars!=0)
		{
			u->image->SetTxt();
			glVertexPointer(3,GL_FLOAT,sizeof(TCV),&u->vertices->p);
			glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(TCV),&u->vertices->c);
			glTexCoordPointer(2,GL_FLOAT,sizeof(TCV),&u->vertices->t);
			glDrawElements(GL_TRIANGLES,u->nChars*6 ,GL_UNSIGNED_SHORT,indices);

			if(clean) u->nChars=0;
		}
		u=u->next;
	}; // while(u)
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void Font::ConvertMouse(float *x,float *y)
{
	*x=mouse.x*480.f/Game::Get()->RenderDimY;
	*y=mouse.y*480.f/Game::Get()->RenderDimY;
}

long Font::GetWStringLen(const wchar_t *text)
{
	long r=0;
	if(text==0) return 0;
	const wchar_t *ptr=text;
	while(*ptr)
	{
		if(IsOnSetW(*ptr)) r++;
		if(*ptr == 16) break;
		if(*ptr == 10) break;
		ptr++;
	}
	return r;
}
