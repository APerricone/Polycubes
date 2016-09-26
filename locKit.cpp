#include "locKit.h"
#include "systemInterface.h"
#include <stdio.h>
#include <string.h>

unsigned char LocKit::CountLangFile()
{
	char buff[11];
	strcpy(buff,"lang1.loc");
	FILE *f;  unsigned char i;
	for(i=0;i<10;i++)
	{
		buff[4]='0'+i;
		f=fopen(buff,"rb");
		if(f==0) break;
		fclose(f);
	}
	return i;
}

void LocKit::GetLangName(unsigned char i,wchar_t *dest,unsigned long len)
{
	char buff[11];
	if(i>10)
	{
		message("LocKit::GetLangName","The maxium number of language is 10");
		i=0;
	}
	strcpy(buff,"lang1.loc");
	buff[4]='0'+i;
	FILE *f=fopen(buff,"rb");
	if(f==0)
	{
		message("LocKit::GetLangName","file %s don't found",buff);
		return;
	}
	unsigned short bom;
	fread(&bom,2,1,f);
	if(bom!=0x0FEFF)
	{
		message("LocKit::GetLangName","file %s is not unicode little-endian",buff);
		fclose(f);
		return;
	}
	while(1)
	{
		fgetws(dest,len,f);
		if(feof(f)) break;
		if(dest[0]==13) continue;
		if(dest[0]==10) continue;
		if(dest[0]==L'/') continue;
		break;
	}
	fclose(f);
}


void LocKit::Init(unsigned char lang)
{
	unsigned char nSet=0,i;
	for(i=0;i<LK_TOTAL;i++)
		string[i][0][0]=string[i][1][0]=0;
	char buff[11];
	if(lang>10)
	{
		message("LocKit::Init","The maxium number of language is 10");
		i=0;
	}
	strcpy(buff,"lang1.loc");
	buff[4]='0'+lang;
	FILE *f=fopen(buff,"rb");
	if(f==0)
	{
		message("LocKit::Init","file %s don't found",buff);
		return;
	}
	unsigned short bom;
	fread(&bom,2,1,f);
	if(bom!=0x0FEFF)
	{
		message("LocKit::Init","file %s is not unicode little-endian",buff);
		fclose(f);
		return;
	}
	wchar_t *u;
	while(1)
	{
		fgetws(string[nSet][0],MAX_LEN,f);
		if(feof(f)) break;
		if(string[nSet][0][0]==13) continue;
		if(string[nSet][0][0]==10) continue;
		if(string[nSet][0][0]==L'/') continue;
		u=wcschr(string[nSet][0],L'\\');
		if(u)
		{
			wcscpy(string[nSet][1],(u+1));
			*u=0;
		}
		nSet++;
		if(nSet==LK_TOTAL) break;
	}
	//hard codes name! don't change!!!!
	wcscat(string[LK_AUTHOR][0],L"Antonino Perricone");
	wcscat(string[LK_AUTHOR+1][0],L"Kevin MacLeod");

	fclose(f);
}
