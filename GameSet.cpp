#include "gameset.h"
#include "Game.h"

#include "systemInterface.h"
// /-\ | |-- /-- |--    |-- |-- -+-
// |-/ | |-  |   |-     +-+ |-   |
// |   | |-- \-- |--    --| |--  |

void PieceSet::Load(FILE *f)
{
	fread(setName,sizeof(wchar_t),NAME_LEN,f);
	fread(canAppear,sizeof(bool),N_PIECES,f);
}

void PieceSet::Save(FILE *f)
{
	fwrite(setName,sizeof(wchar_t),NAME_LEN,f);
	fwrite(canAppear,sizeof(bool),N_PIECES,f);
}

unsigned char PieceSet::GetRandomPiece()
{
	unsigned char i,v,c;
	if(nPieces==0x0FF)
	{
		nPieces=0;
		for(i=0;i<N_PIECES;i++)
			if(canAppear[i]) nPieces++;
	}
	v=rand()%nPieces; c=0;
	for(i=0;i<N_PIECES;i++)
	{
		if(canAppear[i])
		{
			if(c==v) return i;
			c++;
		}
	}
	message("PieceSet::GetRandomPiece","errore scelto pezzo %i su %i pezzi, anche se pensavo di averne %i",
		v,c,nPieces);
	return 0;
}

// /-\ /-\ |  | |--    |-- |-- -+-
// |   |+| |\/| |-     +-+ |-   |
// \-+ | | |  | |--    --| |--  |

GameSet::GameSet()
{
	for(char i=0;i<10;i++)
	{
		records[i].pos=i;
		records[i].points=100-i*10;
		wcscpy(records[i].name,L"Perry");
	}
	currSet=0;
	next=0;
}

GameSet::Records *GameSet::AddRecord(unsigned long points)
{
	if(points<records[9].points) return 0;
	//es. classification is:100,90,80,70,60,50,40,30,20,10, and i want add 55..
	//....          		 0   1  2  3  4  5  6  7  8  9
	char i,j;
	for(i=9;i>=0;i--) if(points<records[i].points) break;
	i++; if(i==10) return 0;
	//in the example i=5, now will copy
	// the 8th in 9th, the 7th in 8th, the 6th in 7th, the 6th in 5th,
	for(j=9;j>i;j--)
	{
		wcscpy(records[j].name,records[j-1].name);
		records[j].points=records[j-1].points;
	}
	//i have:
	//es. classification is:100,90,80,70,60,50,50,40,30,20,
	//....          		 0   1  2  3  4  5  6  7  8  9
	records[i].name[0]=0;
	records[i].points=points;
	//change 5th and have:
	//es. classification is:100,90,80,70,60,55,50,40,30,20,
	//....          		 0   1  2  3  4  5  6  7  8  9
	return &records[i];
}

void GameSet::Load(FILE *f,Games *parent)
{
	fread(gameName,sizeof(wchar_t),NAME_LEN,f);
	fread(&sideA,1,1,f);
	fread(&sideB,1,1,f);
	fread(&depth,1,1,f);
	char i;
	for(i=0;i<10;i++)
	{
		fread(records[i].name,sizeof(wchar_t),NAME_LEN,f);
		fread(&records[i].points,4,1,f);
	}
	PieceSet tmp;
	tmp.Load(f);
	currSet=parent->GetPieceSet(tmp);
}

void GameSet::Save(FILE *f)
{
	fwrite(gameName,sizeof(wchar_t),NAME_LEN,f);
	fwrite(&sideA,1,1,f);
	fwrite(&sideB,1,1,f);
	fwrite(&depth,1,1,f);
	char i;
	for(i=0;i<10;i++)
	{
		fwrite(records[i].name,sizeof(wchar_t),NAME_LEN,f);
		fwrite(&records[i].points,4,1,f);
	}
	currSet->Save(f);
}

// /-\ /-\ |  | |-- |--
// |   |+| |\/| |-  +-+
// \-+ | | |  | |-- --|

Games::~Games()
{
	GameSet *ug;
	while(gameList)
	{
		ug=gameList->next;
		delete gameList;
		gameList=ug;
	}
	PieceSet *up;
	while(pieceSetList)
	{
		up=pieceSetList->next;
		delete pieceSetList;
		pieceSetList=up;
	}
}

void Games::Load()
{
	FILE *f=fopen("BlockOut.opt","rb");
	if(f==0)
	{
		PieceSet tmp;
		wcscpy(tmp.setName,L"Extended");
		memset(tmp.canAppear,1,N_PIECES);
		AddPieceSet(tmp);
		GameSet tmpg;
		wcscpy(tmpg.gameName,L"Crazy");
		tmpg.sideA=tmpg.sideB=tmpg.depth=6;
		tmpg.currSet=GetPieceSet(tmp);
		current=AddGame(tmpg);
		return;
	}
	char buff[18];
	fread(buff,1,18,f);
	if(memcmp(buff,"Block-Out by Perry",18)!=0)
	{
		fclose(f);
		return;
	}
	unsigned char ic;
	fread(&ic,1,1,f);
	if(ic>=1) fread(&Game::Get()->radQuality,1,1,f);
	if(ic>=2) fread(&Game::Get()->sfxVolume,1,1,f);
	if(ic>=3) fread(&Game::Get()->musicVolume,1,1,f);
	if(ic>=4) fread(&Game::Get()->currLang,1,1,f);
	if(ic>=5) fread(&Game::Get()->currDevice,1,1,f);
	if(ic>=6) fread(&Game::Get()->startLevel,1,1,f);
	unsigned long i,n;
	PieceSet tmp;
	fread(&n,4,1,f);
	for(i=0;i<n;i++)
	{
		tmp.Load(f);
		AddPieceSet(tmp);
	}
	GameSet tmpg;
	fread(&n,4,1,f);
	unsigned char c;
	for(i=0;i<n;i++)
	{
		fread(&c,1,1,f);
		tmpg.Load(f,this);
		AddGame(tmpg);
		if(c) current=GetGame(tmpg);
	}
	fclose(f);
}

void Games::Save()
{
	FILE *f=fopen("BlockOut.opt","wb");
	if(f==0) return;
	fwrite("Block-Out by Perry",1,18,f);

	unsigned char i;
	i=6; fwrite(&i,1,1,f);
	fwrite(&Game::Get()->radQuality,1,1,f);
	fwrite(&Game::Get()->sfxVolume,1,1,f);
	fwrite(&Game::Get()->musicVolume,1,1,f);
	fwrite(&Game::Get()->currLang,1,1,f);
	fwrite(&Game::Get()->currDevice,1,1,f);
	fwrite(&Game::Get()->startLevel,1,1,f);

	unsigned long n;
	//conto i set di pezzi;
	PieceSet *up=pieceSetList;  n=0;
	while(up) { n++; up=up->next; }
	fwrite(&n,4,1,f);
	up=pieceSetList;
	while(up) { up->Save(f); up=up->next; }
	//conto i set di gameSet
	GameSet *ug=gameList; n=0;
	while(ug) { n++; ug=ug->next; }
	fwrite(&n,4,1,f);
	ug=gameList;
	unsigned char c;
	while(ug)
	{
		c=0;
		if(ug==current) c=1;
		fwrite(&c,1,1,f);
		ug->Save(f);
		ug=ug->next;
	}
	fclose(f);

}

GameSet *Games::AddGame(const GameSet& tmp)
{
	GameSet *u=new GameSet;
	memcpy(u,&tmp,sizeof(GameSet));
	if(u->sideA>u->sideB)
	{
		u->sideA=tmp.sideB;
		u->sideB=tmp.sideA;
	}
	u->currSet=GetPieceSet(*tmp.currSet);
	u->next=gameList;
	gameList=u;
	return u;
}

PieceSet *Games::AddPieceSet(const PieceSet& tmp)
{
	PieceSet *u=new PieceSet;
	wcscpy(u->setName,tmp.setName);
	memcpy(u->canAppear,tmp.canAppear,N_PIECES);
	u->next=pieceSetList;
	pieceSetList=u;
	return u;
}

GameSet *Games::GetGame(const GameSet& tmp)
{
	GameSet *u=gameList;
	while(u)
	{
		if(*u==tmp) return u;
		u=u->next;
	}
	return 0;
}

PieceSet *Games::GetPieceSet(const PieceSet& tmp)
{
	PieceSet *u=pieceSetList;
	while(u)
	{
		if(*u==tmp) return u;
		u=u->next;
	}
	return 0;
}

void Games::DelGame(const GameSet& tmp)
{
	GameSet *u=gameList,*p=0;
	while(u)
	{
		if(*u==tmp)
		{
			if(p) p->next=u->next; else gameList=u->next;
			if(current==u) current=gameList;
			delete u;
			return;
		}
		p=u;
		u=u->next;
	}
}

void Games::DelPieceSet(const PieceSet& tmp)
{
	PieceSet *u=pieceSetList,*p=0;
	while(u)
	{
		if(*u==tmp)
		{
			if(p) p->next=u->next; else pieceSetList=u->next;
			delete u;
			return;
		}
		p=u;
		u=u->next;
	}
}

bool Games::CanDeletePieceSet(const PieceSet& tmp)
{
	PieceSet *ck=GetPieceSet(tmp);
	if(ck==0) return false;
	GameSet *u=gameList;
	while(u)
	{
		if(u->currSet==ck) return false;
		u=u->next;
	}
	return true;
}

GameSet *Games::GetGameFromName(const wchar_t* tmp)
{
	GameSet *u=gameList;
	while(u)
	{
		if(wcscmp(u->gameName,tmp)==0) return u;
		u=u->next;
	}
	return 0;
}

PieceSet *Games::GetPieceSetFromName(const wchar_t* tmp)
{
	PieceSet *u=pieceSetList;
	while(u)
	{
		if(wcscmp(u->setName,tmp)==0) return u;
		u=u->next;
	}
	return 0;
}
bool operator==(const PieceSet &a,const PieceSet &b)
{
	if(memcmp(a.canAppear,b.canAppear,N_PIECES)==0) return true;
	return false;
}

bool operator==(const GameSet &a,const GameSet &b)
{
	if(!(*a.currSet==*b.currSet)) return false;
	if(a.depth!=b.depth) return false;
	if( ((a.sideA==b.sideA)&&(a.sideB==b.sideB))||
		((a.sideA==b.sideB)&&(a.sideB==b.sideA))) return true;
	return false;
}

