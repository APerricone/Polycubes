#include "radiosityMan.h"
#include "game.h"

RadiosityMan::BOFace *RadiosityMan::AddFace(
	const V3D &pos,const V3D &nor,const Color &materialColor,float scale)
{
	unsigned long nDivisionBounce=nDivision/PIXEL_STEP;
	BOFace *add=new BOFace;
	add->posChar=parent->Revert3D(pos);
	add->center=pos;
	add->normal=nor;
	Color col=materialColor;
	if(nor.z==0) col=materialColor*0.5f;
	if(nor*V3D(-0.5f,0.5f,1.f)<0) col.Set(0,0,0,255);
	V3D dirA,dirB;
	if(fabsf(fabsf(nor.x)-1)<1e-3)
	{
		dirA=V3D(0,-nor.x,0);
		dirB=V3D(0,0,-1);
	}
	if(fabsf(fabsf(nor.y)-1)<1e-3)
	{
		dirA=V3D(nor.y,0,0);
		dirB=V3D(0,0,-1);
	}
	if(fabsf(fabsf(nor.z)-1)<1e-3)
	{
		dirA=V3D(1,0,0);
		dirB=V3D(0,1,0);
	}
	char x,y;
	for(x=0;x<nDivision;x++)
		for(y=0;y<nDivision;y++)
		{
			add->positionsDirect[x][y]=pos+
				(((x-(nDivision/2.f)+0.5f)/(float)(nDivision))*scale)*dirA+
				(((y-(nDivision/2.f)+0.5f)/(float)(nDivision))*scale)*dirB;
			add->pixelDirect[x][y]=col;
		}
	memset(add->pixelBounce,0,sizeof(Color)*N_DIVISION_MAX*N_DIVISION_MAX);
	for(x=0;x<nDivisionBounce;x++)
		for(y=0;y<nDivisionBounce;y++)
			add->positionsBounce[x][y]=pos+
				(((x-(nDivisionBounce/2.f)+0.5f)/(float)(nDivisionBounce))*scale)*dirA+
				(((y-(nDivisionBounce/2.f)+0.5f)/(float)(nDivisionBounce))*scale)*dirB;
	add->material=materialColor;
	add->next=faces;
	faces=add;
	return add;
}

void RadiosityMan::DelFace(BOFace *toDel)
{
	if(faces==toDel)
	{
		faces=toDel->next;
	} else
	{
		BOFace *u=faces;
		while(u->next!=toDel) u=u->next;
		u->next=toDel->next;
	}
	delete toDel;
}

void RadiosityMan::DelAllFaces()
{
	BOFace *u;
	while(faces)
	{
		u=faces->next;
		delete faces;
		faces=u;
	}
}

DWORD WINAPI RadThread(LPVOID lpParameter)
{
	RadiosityMan *r=(RadiosityMan *)lpParameter;
	DWORD rr=0;
	do
	{
		rr=WaitForSingleObject(r->hMutex,100);
		if(rr==WAIT_TIMEOUT)
		{
			rr=rr;
		}
	} while(rr!=WAIT_OBJECT_0);
	unsigned char myID=r->idTh++;
	ReleaseMutex(r->hMutex);
	RadiosityMan::BOFace *u;
	bool iStopThisState=true;
	while(r->iRadiosity)
	{
		u=r->GetNextFace();
		if(u)
		{
			r->iCalc[myID]=true;
			if(r->radState==0)
				r->DoDirect(u);
			else
				r->DoBounce(u,myID);
		} else
		{
			r->iCalc[myID]=false;
		}
	}
	return 1;
}

extern unsigned long nRay;
void RadiosityMan::MakeRadiosity(bool bounce)
{
	nRay=0;
	BOFace *u=faces,*v;
	V3D d;
	while(u)
	{
		u->on=true;
		u=u->next;
	}
	u=faces;
	while(u)
	{
		v=u->next;
		while(v)
		{
			d=u->center-v->center;
			if(d*d<0.2f)
			{
				u->on=false;
				v->on=false;
			}
			v=v->next;
		}
		u=u->next;
	}

	currFace=faces;
	actual_bounce=0;
	radState=0;
	idTh=0;
	iRadiosity=true;
	iCalc[0]=iCalc[1]=iCalc[2]=iCalc[3]=false;
	unsigned char i,n=Game::Get()->nThread;
	SECURITY_ATTRIBUTES sa;
	sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor=0;
	sa.bInheritHandle=false;
	hMutex=0;
	hMutex=CreateMutex(&sa,false,"BORadMutex");
	if(hMutex==0)
	{
		ExitProcess((UINT)-1) ;
	}
	for(i=0;i<n;i++)
	{
		DWORD rr=0;
		do
		{
			rr=WaitForSingleObject(hMutex,100);
			if(rr==WAIT_TIMEOUT)
			{
				rr=rr;
			}
		} while(rr!=WAIT_OBJECT_0);
		iCalc[i]=true; CreateThread(0,0,RadThread,this,0,0);
		ReleaseMutex(hMutex);
	}
	do
	{
		if(!iCalc[0] && !iCalc[1] && !iCalc[2] && !iCalc[3])
		{
			radState++;
			if(!bounce && radState==1) break;
			if(radState==4) break;
			facesOn=0;
			u=faces;
			actual_bounce=1-actual_bounce;
			while(u)
			{
				if(	(u->on)&&
					(u->bounce[1-actual_bounce].r>4)&&
					(u->bounce[1-actual_bounce].g>4)&&
					(u->bounce[1-actual_bounce].b>4))
				{
					u->nextOn=facesOn;
					facesOn=u;
				}
				u=u->next;
			}
			currFace=faces;
		}
		Sleep(1);
	} while(1);
	CloseHandle(hMutex);
	iRadiosity=false;
/*/	do single-thread version...
	{
		u=GetNextFace();
		if(u)
		{
			if(radState==0)
				DoDirect(u);
			else
				DoBounce(u);
		} else
		{
			radState++;
			if(radState==4) break;
			facesOn=0;
			u=faces;
			actual_bounce=1-actual_bounce;
			while(u)
			{
				if(	(u->on)&&
					(u->bounce[1-actual_bounce].r>4)&&
					(u->bounce[1-actual_bounce].g>4)&&
					(u->bounce[1-actual_bounce].b>4))
				{
					u->nextOn=facesOn;
					facesOn=u;
				}
				u=u->next;
			}
			currFace=faces;
		}
	} while(1);//*/
}

RadiosityMan::BOFace *RadiosityMan::GetNextFace()
{
	DWORD r=0;
	do
	{
		r=WaitForSingleObject(hMutex,100);
		if(r==WAIT_FAILED)
			return 0;
		if(r==WAIT_TIMEOUT)
		{
			if(currFace==0)
				return 0;
		}
	} while(r!=WAIT_OBJECT_0);
	if(currFace==0)
	{
		ReleaseMutex(hMutex);
		return 0;
	}
	BOFace *n=0,*tr;
	tr=currFace;
	if(tr) n=tr->next;
	currFace=n;
	ReleaseMutex(hMutex);
	return tr;
}

void RadiosityMan::DoDirect(RadiosityMan::BOFace *u)
{
	unsigned char x,y;
	V3D b,a;
	float i;
	float vi;
	i=0;
	memset(u->pixelBounce,0,sizeof(Color)*N_DIVISION_MAX*N_DIVISION_MAX);
	for(x=0;x<nDivision;x++)
		for(y=0;y<nDivision;y++)
		{
			a=u->positionsDirect[x][y];
			b=a+V3D(-10,10,20);
			if((u->normal*V3D(-0.5f,0.5f,1.f)<0) ||
				parent->CheckCollision(a,u->posChar,b,u->posChar+B3D(-10,10,-20),true))
#define DEST u->pixelDirect
				DEST[x][y].Set(0,0,0,255);
			else
			{
				//DEST[x][y]=u->material*1.f;
				if(u->normal.z==0)
				{
					DEST[x][y]=u->material*0.5f;
					i+=0.5f;
				}
				else
				{
					DEST[x][y]=u->material;
					i+=1.f;
				}
			}
#undef DEST
		}
	vi=i/(float)(nDivision*nDivision);
	u->totalr=u->bounce[0].r=(unsigned char)(u->material.r*vi);
	u->totalg=u->bounce[0].g=(unsigned char)(u->material.g*vi);
	u->totalb=u->bounce[0].b=(unsigned char)(u->material.b*vi);
	u->bounce[1].Set(0,0,0,255);
	u=u->next;
}

void RadiosityMan::DoBounce(RadiosityMan::BOFace *u,unsigned char id)
{
	BOFace *l;
#define area 0.5f
	unsigned char x,y;
	V3D b,a;
	float ir,ig,ib;
	float br,bg,bb;
	float vi;
	float dist;
	Color *dest;
	unsigned long nDivisionBounce=nDivision/PIXEL_STEP;
	if((u->on)
		&&(u->totalr<255)
		&&(u->totalg<255)
		&&(u->totalb<255))
	{
		br=bg=bb=0;
		for(x=0;x<nDivisionBounce;x++)
			for(y=0;y<nDivisionBounce;y++)
			{
				b=u->positionsBounce[x][y];
				l=facesOn;
				ir=ig=ib=0;
				dest=&u->pixelBounce[x][y];
				if(	(dest->r!=255)&&
					(dest->g!=255)&&
					(dest->b!=255))
				while(l)
				{
					if((x==0)&&(y==0))
					{
						l->ok[id]=(l!=u);
						if(l->ok[id])
						{
							a=u->center-l->center;
							l->ok[id]=(a*l->normal)>0;
							if(l->ok[id]) l->ok[id]=(a*u->normal)<0;
						}
					}
					if(l->ok[id])
						if(!parent->CheckCollision(l->center,l->posChar,b,u->posChar,false))
						{
							a.x=b.x-l->center.x;
							a.y=b.y-l->center.y;
							a.z=b.z-l->center.z;
							dist=a.x*a.x+a.y*a.y+a.z*a.z;
							vi=-area*(
								(a.x*u->normal.x+a.y*u->normal.y+a.z*u->normal.z)*
								(a.x*l->normal.x+a.y*l->normal.y+a.z*l->normal.z)/(dist*(dist+area)));
							if(vi>0)
							{
								//vi/=5;
								ir+=l->bounce[1-actual_bounce].r*vi/255.f;
								ig+=l->bounce[1-actual_bounce].g*vi/255.f;
								ib+=l->bounce[1-actual_bounce].b*vi/255.f;
							}
						}
					l=l->nextOn;
				}
				if(u->material.r!=255)
				{
					ir*=(255+255+255)/(240+32+32);
					ig*=(255+255+255)/(240+32+32);
					ib*=(255+255+255)/(240+32+32);
				}
				if(ir>1) ir=1;
				if(ig>1) ig=1;
				if(ib>1) ib=1;
				vi=ir-dest->r/255.f; if(vi<0) vi=0;
				br+=vi;
				vi=ig-dest->g/255.f; if(vi<0) vi=0;
				bg+=vi;
				vi=ib-dest->b/255.f; if(vi<0) vi=0;
				bb+=vi;
				ir*=u->material.r;
				ig*=u->material.g;
				ib*=u->material.b;
				ir+=dest->r;
				ig+=dest->g;
				ib+=dest->b;
				if(ir>255) ir=255;
				if(ig>255) ig=255;
				if(ib>255) ib=255;
				dest->r=(unsigned char)ir;
				dest->g=(unsigned char)ig;
				dest->b=(unsigned char)ib;
			}
		br/=(float)(nDivisionBounce*nDivisionBounce);
		bg/=(float)(nDivisionBounce*nDivisionBounce);
		bb/=(float)(nDivisionBounce*nDivisionBounce);
		if(br>1) br=1;
		if(bg>1) bg=1;
		if(bb>1) bb=1;
		u->bounce[actual_bounce].r=(unsigned char)(br*u->material.r);
		u->bounce[actual_bounce].g=(unsigned char)(bg*u->material.g);
		u->bounce[actual_bounce].b=(unsigned char)(bb*u->material.b);
		u->totalr+=u->bounce[actual_bounce].r;
		u->totalg+=u->bounce[actual_bounce].g;
		u->totalb+=u->bounce[actual_bounce].b;
		if(u->totalr>255)
		{
			u->totalr=255;
			u->bounce[actual_bounce].r-=u->totalr-255;
		}
		if(u->totalg>255)
		{
			u->totalg=255;
			u->bounce[actual_bounce].g-=u->totalg-255;
		}
		if(u->totalb>255)
		{
			u->totalg=255;
			u->bounce[actual_bounce].b-=u->totalb-255;
		}
	}
}
