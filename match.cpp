#include "match.h"
#include "systeminterface.h"
#include "game.h"

#include <stdio.h>

//Frames for move/rotate a piece
#define MOVE_FRAME 3
//the time between auto-falls is (9-level)*TIME_FOR_LEVEL.<br>
//It mean: 0 for level 10 and 45 for level 0
#define TIME_FOR_LEVEL 5
//frames for repeating last moving withou release the key
#define REPEAT_TIME 12
//delta-snake for every cube in piece
#define SHAKE_FACTOR 2
//cycle of snake
#define SHAKE_PERIOD 5
//wait frames before the piece place itself(you can move it in this time)
#define WAIT_PLACE 18
//time between disappear of plane if they are more of one
#define WAIT_TIME 18

void OpenWave(char *nameFile,unsigned int buffer);
extern ALCdevice *alDevice;

#include "efx.h"
#define AL_EFX_FUNC(func,type) type func = (type)alGetProcAddress(#func);

void Match::Init()
{
#ifndef _DEBUG
	hole.radMan.nDivision=previewHole.radMan.nDivision=1<<(3+Game::Get()->radQuality);
#endif
	current=Game::Get()->games.current;
	hole.Init(current->sideA,current->sideB,current->depth);
	nextPiece=current->currSet->GetRandomPiece();
	InitPiece();
	shake=0;
	totalShake=0;
	state=NORMAL;
	v.Reset();
	r.Reset();
	miniHoleAnglePos=0;
	timeToNextDown=0;
	level=Game::Get()->startLevel-1;
	nLineMade=0;
	score=0;
	if(sound)
	{
		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
		alGenSources(5,doneLinesSource);
		alGenSources(2,placePieceSource);
		alGenBuffers(5,doneLinesBuffer);
		alGenBuffers(1,&placePieceBuffer);
		OpenWave("sound\\154C.wav",doneLinesBuffer[0]);
		OpenWave("sound\\154D.wav",doneLinesBuffer[1]);
		OpenWave("sound\\154E.wav",doneLinesBuffer[2]);
		OpenWave("sound\\154F.wav",doneLinesBuffer[3]);
		OpenWave("sound\\154G.wav",doneLinesBuffer[4]);
		OpenWave("sound\\000.wav",placePieceBuffer);
		unsigned char i;
		float gain=Game::Get()->sfxVolume/100.f;
		effectSlot=effect=0;
		AL_EFX_FUNC(alGenAuxiliaryEffectSlots,LPALGENAUXILIARYEFFECTSLOTS);
		AL_EFX_FUNC(alGenEffects,LPALGENEFFECTS);
		AL_EFX_FUNC(alEffecti,LPALEFFECTI);
		AL_EFX_FUNC(alEffectf,LPALEFFECTF);
		AL_EFX_FUNC(alAuxiliaryEffectSloti,LPALAUXILIARYEFFECTSLOTI);
		alGenAuxiliaryEffectSlots(1, &effectSlot);
		alGenEffects(1, &effect);
		bool canEffect=(effect!=0) && (effectSlot!=0);
		if(canEffect)
		{
			alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
			alEffectf(effect, AL_REVERB_DECAY_TIME,
				(float)current->sideA*(float)current->sideB*(float)current->depth/1000.f);
			alAuxiliaryEffectSloti(effectSlot,AL_EFFECTSLOT_EFFECT, effect);
		}
		for(i=0;i<5;i++)
		{
		    alSourcei(doneLinesSource[i],AL_BUFFER,doneLinesBuffer[i]);
		    alSourcef(doneLinesSource[i],AL_GAIN,gain);
		    alSource3i(doneLinesSource[i],AL_AUXILIARY_SEND_FILTER,effectSlot,0,0);
		}
		for(i=0;i<2;i++)
		{
		    alSourcei(placePieceSource[i],AL_BUFFER,placePieceBuffer);
		    alSourcef(placePieceSource[i],AL_MIN_GAIN,0);
			alSourcef(placePieceSource[i],AL_MAX_GAIN,gain);
		    //alSourcef(placePieceSource[i],AL_GAIN,gain);
			alSourcef(placePieceSource[i],AL_REFERENCE_DISTANCE,1);
			alSourcef(placePieceSource[i],AL_MAX_DISTANCE,current->sideA/2);
			alSource3i(placePieceSource[i],AL_AUXILIARY_SEND_FILTER,effectSlot,0,0);
		}
		nextPlacePieceSourceToPlay=0;
		lastMusic=10;
		PlayMusic();
	}
}

void Match::PlayMusic()
{
	if(!sound) return;
	if(Game::Get()->musicVolume==0) return;
	if((music) && (!music->ended)) return;
	unsigned char newMusic;
	if(lastMusic>7) newMusic=rand()%7; else
	{
		newMusic=rand()%6;
		if(newMusic>=lastMusic) newMusic++;
	}
	//::message("Play Music","now Play %02i(last %02i)",newMusic,lastMusic);
	lastMusic=newMusic;
	char buff[20];
	sprintf(buff,"music\\polka%02i.ogg",newMusic);
	music=BeginOggVorbis(buff,music);
	if(music==0) return;
	float vol=Game::Get()->musicVolume/100.f;
	alSourcef(music->alSource,AL_GAIN,vol);
}

void Match::Deinit(bool total)
{
	state=INVALID;
	previewHole.Deinit();
	hole.Deinit();
	if(sound && doneLinesSource[0]!=0)
	{
		if(music)
		{
			EndOggVorbis(music);
			delete music;
		} music=0;
		alSourceStopv(5,doneLinesSource);
		alSourceStopv(2,placePieceSource);
		unsigned char i;
		for(i=0;i<5;i++) alSourcei(doneLinesSource[i],AL_BUFFER,0);
		for(i=0;i<2;i++) alSourcei(placePieceSource[i],AL_BUFFER,0);

		alDeleteSources(5,doneLinesSource);
		alDeleteSources(2,placePieceSource);
		alDeleteBuffers(5,doneLinesBuffer);
		alDeleteBuffers(1,&placePieceBuffer);
		doneLinesSource[0]=0;

		AL_EFX_FUNC(alDeleteEffects,LPALDELETEEFFECTS);
		AL_EFX_FUNC(alDeleteAuxiliaryEffectSlots,LPALDELETEAUXILIARYEFFECTSLOTS);
		if(effect) alDeleteEffects(1,&effect);
		if(effectSlot) 	alDeleteAuxiliaryEffectSlots(1, &effectSlot);
		effect=effectSlot=0;
	}

	if(!total)
	{
		Game::Get()->currMenu=new MainMenu();
		Game::Get()->currMenu->Init();
	}
}

void Match::InitPiece()
{
	currPiece=nextPiece;
	nextPiece=current->currSet->GetRandomPiece();
	pieces.SetupDraw(currPiece);
	nextPieceLines.SetupDraw(nextPiece);
	bm.r.Set(1,0,0);
	bm.u.Set(0,1,0);
	bm.a.Set(0,0,1);
	bm.p.x=hole.GetSideA()/2;
	bm.p.y=hole.GetSideB()/2;
	bm.p.z=realz=0;
	v.Reset();
	r.Reset();
	CheckSyncMatrix();
	if(!Check(bm))
	{
		End(true);
	}

	previewHole.radMan.nDivision=N_DIVISION_MAX; //max quality
	char buff[30];
	sprintf(buff,"EditCubes/prev%03i.boh",nextPiece);
	previewHole.Load(buff);
	fall=false;
}

bool Match::End(bool showGameOver)
{
	state=GAME_OVER;
	record=current->AddRecord(score);
	if(record)
	{
		LocKit *lk=&Game::Get()->locKit;
		insertName.Init(
			Game::Get()->locKit.string[LK_ENDGAME+record->pos][0],
			record->name,NAME_LEN,200);
		return true;
	} else
	{
		if(showGameOver)
		{
			gameOver.Init(
				Game::Get()->locKit.string[LK_ENDGAME+10][0],
				0,Dialog::OK);
		} else
			Deinit(false);
	}
	return false;
}

void Match::AddShake(char shakeFact)
{
	totalShake+=shakeFact;
	shake-=shakeFact;
	if(shake<0) shake=0;
}

void Match::PlacePiece()
{
	char i,j;
	BOCube *c[5];
	//bm.p.z=realz;
	c[0]=hole.GetBOCube(bm.p);
	AddShake(SHAKE_FACTOR*colletion[currPiece].nPiece);
	for(i=0;i<colletion[currPiece].nPiece-1;i++)
		c[i+1]=hole.GetBOCube(colletion[currPiece].dirs[i]*bm);
	B3D d;
	for(i=0;i<colletion[currPiece].nPiece;i++)
	{
		c[i]->on=true;
		c[i]->unionXpos=c[i]->unionXneg=
		c[i]->unionYpos=c[i]->unionYneg=
		c[i]->unionZpos=c[i]->unionZneg=false;
		for(j=0;j<colletion[currPiece].nPiece;j++)
			if(j!=i)
			{
				d=c[j]->p-c[i]->p;
				if((abs(d.x)+abs(d.y)+abs(d.z))==1)
				{
					if(d.x== 1) c[i]->unionXpos=true;
					if(d.x==-1) c[i]->unionXneg=true;
					if(d.y== 1) c[i]->unionYpos=true;
					if(d.y==-1) c[i]->unionYneg=true;
					if(d.z==-1) c[i]->unionZpos=true;
					if(d.z== 1) c[i]->unionZneg=true;
				}
			}
	}
	hole.UpdateAll();
	state=WAIT_NEXT_PIECE;
	nLines=0;
	InitPiece();
	timeToNextPiece=WAIT_TIME;
}

extern const Color colorArray[8];

bool Match::Check(const BMatrix &startMat,B3D *movePtr)
{
	BMatrix tmp=startMat;
	tmp.p.z+=realz-bm.p.z;
	B3D curr=tmp.p;
	if(movePtr) movePtr->Reset();
	bool changeX=false,changeY=false;
	char i=0;
	unsigned char w=hole.GetSideA();
	unsigned char h=hole.GetSideB();
	while(1)
	{
		if(curr.z==-1) { tmp.p.z++; curr=tmp.p; i=0; continue; }
		if(movePtr)
		{
			if((curr.x==-1)&&(movePtr->x==0))
				{ movePtr->x=1; tmp.p.x++; curr=tmp.p; i=0; continue; }
			if((curr.x==hole.GetSideA())&&(movePtr->x==0))
				{ movePtr->x=-1; tmp.p.x--; curr=tmp.p; i=0; continue; }
			if((curr.y==-1)&&(movePtr->y==0))
				{ movePtr->y=1; tmp.p.y++; curr=tmp.p; i=0; continue; }
			if((curr.y==hole.GetSideB())&&(movePtr->y==0))
				{ movePtr->y=-1; tmp.p.y--; curr=tmp.p; i=0; continue; }
		}
		if(!hole.Check(curr)) return false;
		curr=colletion[currPiece].dirs[i]*tmp;
		i++;
		if(i==colletion[currPiece].nPiece) return true;
	}
	return true;
}

void Match::CheckSyncMatrix()
{
	//return;
	//if(v.x!=0) return;
	//if(v.y!=0) return;
	//if(v.z!=0) return;
	if((r.x==0)&&(r.y==0)&&(r.z==0))
	{
		m.Right().Set(bm.r.x,bm.r.y,bm.r.z);
		m.Up().Set(   bm.u.x,bm.u.y,bm.u.z);
		m.At().Set(   bm.a.x,bm.a.y,bm.a.z);
	}
	V3D c;
	bm.p.z=realz;
	c=hole.Get3D(bm.p);
	if(v.x) c.x-=v.x/(float)MOVE_FRAME;
	if(v.y) c.y-=v.y/(float)MOVE_FRAME;
	if((v.z)&&(v.z<MOVE_FRAME))
		c.z+=v.z/(float)MOVE_FRAME;
	m.Pos()=c;
	V3D s,i;
	m.Transform(&s,pieces.box.sup);
	m.Transform(&i,pieces.box.inf);
	if(i.z>s.z) s.z=i.z;
	if(s.z>-1.f)
	{
		bm.p.z+=1.f+s.z;
		m.Pos().z-=1.f+s.z;
	}
}

void Match::Update()
{
	if(state==INVALID) return;
	PlayMusic();
#define MINITURNTIME 300
	unsigned char i,j;
	miniHoleAnglePos++;
	if(miniHoleAnglePos==MINITURNTIME)
		miniHoleAnglePos=0;
	if(totalShake!=0)
	{
		shake++;
		if(shake==totalShake)
			shake=totalShake=0;
	}

	if(Game::Get()->currMenu!=0)
	{
		myButtons[11]=true;
		return;
	}
	if((buttons[11])&&(!myButtons[11]))
	{
		Game::Get()->currMenu=new inGame;
		Game::Get()->currMenu->Init();
		return;
	}
	myButtons[11]=buttons[11];
	if(state==GAME_OVER)
	{
		unsigned char r;
		if(record)
		{
			r=insertName.Update();
		} else
		{
			r=gameOver.Update();
		}
		if(r==0) Deinit(false);
		return;
	}
	if(state==WAIT_NEXT_PIECE)
	{
		timeToNextPiece--;
		if(timeToNextPiece==0)
		{
			unsigned char deletePlane=hole.CheckForPlanes();
			if(deletePlane)
			{
				alSourcePlay(doneLinesSource[nLines]);
				AddShake(SHAKE_FACTOR*hole.GetSideA()*hole.GetSideB()*deletePlane);
				timeToNextPiece=WAIT_TIME;
				nLines++;
				score+=(level+1)*10*nLines;
				nLineMade++;
				if(nLineMade==10)
				{
					level++;
					if(level==10) level=9;
					nLineMade=0;
				}
			}
			else
			{
				//InitPiece();
				state=NORMAL;
			}
		}
		for(i=0;i<NUM_BUTTON;i++) myButtons[i]=false;
		return;
	}
	//key check
	BMatrix pr=bm;
	/*
	//This code is for testing drawing of next pieces
	static exB=true;
	if((buttons[ 0])&&(!exB)&&(nextPiece>0)) { nextPiece--; previewHole.SetPreview(nextPiece); }
	if((buttons[ 1])&&(!exB)&&(nextPiece<N_PIECES-1)) { nextPiece++; previewHole.SetPreview(nextPiece); }
	exB=(buttons[ 0] || buttons[ 1]);
	/*/
	if(!fall)
	{
		for(i=0;i<NUM_BUTTON;i++)
			myButtons[i]=buttons[i];
	} else
	{
		for(i=0;i<10;i++)
			if(buttons[i])
			{
				myButtons[i]=true;
				if(i%2)
					myButtons[i-1]=false;
				else
					myButtons[i+1]=false;
			}
		myButtons[10]=false;
	}
	//Moving
	bool canApplyKey;
	for(i=0;i<4;i++)
	{
		j=1-i/2;
		canApplyKey=false;
		if( (myButtons[i])&&
			(v_time[j]==0)&&
			(v[j]==0))
		{
			switch(i)
			{
				case 0: canApplyKey=Check(bm.GetTranslate(B3D(0, 1,0))); break;
				case 1: canApplyKey=Check(bm.GetTranslate(B3D(0,-1,0))); break;
				case 2: canApplyKey=Check(bm.GetTranslate(B3D(-1,0,0))); break;
				case 3: canApplyKey=Check(bm.GetTranslate(B3D( 1,0,0))); break;
			}
		}
		if(canApplyKey)
		{
			v[j]= MOVE_FRAME;
			if((i==0)||(i==3))
			{
				v[j]=MOVE_FRAME;
				bm.p[j]++;
			} else
			{
				v[j]=-MOVE_FRAME;
				bm.p[j]--;
			}
			v_time[j]=REPEAT_TIME;
		}
	}
	//Rotation
	B3D rotMove;
	for(i=4;i<10;i++)
	{
		j=(i-4)/2;
		if(	(myButtons[i])&&
			(r_time[j]==0)&&
			(r[j]==0))
		{
			switch(i)
			{
				case 4: canApplyKey=Check(bm*pr.SetXRotate( 1),&rotMove); break;
				case 5: canApplyKey=Check(bm*pr.SetXRotate(-1),&rotMove); break;
				case 6: canApplyKey=Check(bm*pr.SetYRotate( 1),&rotMove); break;
				case 7: canApplyKey=Check(bm*pr.SetYRotate(-1),&rotMove); break;
				case 8: canApplyKey=Check(bm*pr.SetZRotate( 1),&rotMove); break;
				case 9: canApplyKey=Check(bm*pr.SetZRotate(-1),&rotMove); break;
			}
			if((canApplyKey)&&(rotMove.x!=0))
			{
				if(v.x==0)
				{
					v.x=rotMove.x*MOVE_FRAME;
					bm.p.x+=rotMove.x;
				}
				else
					canApplyKey=false;
			}
			if((canApplyKey)&&(rotMove.y!=0))
			{
				if(v.y==0)
				{
					v.y=rotMove.y*MOVE_FRAME;
					bm.p.y+=rotMove.y;
				}
				else
					canApplyKey=false;
			}
			if(canApplyKey)
			{
				r[j]= MOVE_FRAME;
				if(i%2) r[j]=-MOVE_FRAME;
				r_time[j]=REPEAT_TIME;
				switch(i)
				{
					case 4: bm=bm*pr.SetXRotate( 1); break;
					case 5: bm=bm*pr.SetXRotate(-1); break;
					case 6: bm=bm*pr.SetYRotate( 1); break;
					case 7: bm=bm*pr.SetYRotate(-1); break;
					case 8: bm=bm*pr.SetZRotate( 1); break;
					case 9: bm=bm*pr.SetZRotate(-1); break;
				}

			}
		}
	}
	/*
	if((myButtons[ 4])&&(r_time.x==0)&&(r.x==0)&&(Check(bm*pr.SetXRotate( 1),&rotMove)))
	{
		canApplyKey=true;
		if(rotMove.x!=0)
		{
			if(v.x==0) v.x=rotMove.x*MOVE_FRAME; else canRotate=false;
		}
		if(canRotate)
		{
			r.x= MOVE_FRAME;
			r_time.x=REPEAT_TIME;
			bm=bm*pr.SetXRotate( 1);
		}
	}
	if((myButtons[ 5])&&(r_time.x==0)&&(Check(bm*pr.SetXRotate(-1),&rotMove)))
	{
		r.x=-MOVE_FRAME;
		r_time.x=REPEAT_TIME;
		bm=bm*pr.SetXRotate(-1);
	}
	if((myButtons[ 6])&&(r_time.y==0)&&(Check(bm*pr.SetYRotate( 1),&rotMove)))
	{
		r.y= MOVE_FRAME;
		r_time.y=REPEAT_TIME;
		bm=bm*pr.SetYRotate( 1);
	}
	if((myButtons[ 7])&&(r_time.y==0)&&(Check(bm*pr.SetYRotate(-1),&rotMove)))
	{
		r.y=-MOVE_FRAME;
		r_time.y=REPEAT_TIME;
		bm=bm*pr.SetYRotate(-1);
	}
	if((myButtons[ 8])&&(r_time.z==0)&&(Check(bm*pr.SetZRotate( 1),&rotMove)))
	{
		r.z= MOVE_FRAME;
		r_time.z=REPEAT_TIME;
		bm=bm*pr.SetZRotate( 1);
	}
	if((myButtons[ 9])&&(r_time.z==0)&&(Check(bm*pr.SetZRotate(-1),&rotMove)))
	{
		r.z=-MOVE_FRAME;
		r_time.z=REPEAT_TIME;
		bm=bm*pr.SetZRotate(-1);
	}*/
	if((!myButtons[0])&&(!myButtons[1])) { v_time.y=0; }
	if((!myButtons[2])&&(!myButtons[3])) { v_time.x=0; }
	if((!myButtons[4])&&(!myButtons[5])) { r_time.x=0; }
	if((!myButtons[6])&&(!myButtons[7])) { r_time.y=0; }
	if((!myButtons[8])&&(!myButtons[9])) { r_time.z=0; }
	if(myButtons[10]) { fall=true; timeToNextDown=0; }
	timeToNextDown++;
	//FALLIIIIIIIIIIING
	unsigned short toCheck=MOVE_FRAME*((9-level)*TIME_FOR_LEVEL);
	if(fall) toCheck=MOVE_FRAME;//+MOVE_FRAME/2;
	if((timeToNextDown>toCheck)&&(v.z==0))
	{
		timeToNextDown=0;
		v.z=MOVE_FRAME;
	}
	if((v.z==MOVE_FRAME))
	{
		if(Check(bm.GetTranslate(B3D(0,0,1))))
		{
			bm.p.z++;
			realz++;
			timeToPlacePiece=WAIT_PLACE;
			hasDown=true;
		}
		else
		{
			if((sound)&&(hasDown))
			{
				V3D p,c=(pieces.box.inf+pieces.box.sup)/2;
				m.Transform(&p,c);
				alSourcefv(
					placePieceSource[nextPlacePieceSourceToPlay],AL_POSITION,
					&(p.x));
				alSourcePlay(placePieceSource[nextPlacePieceSourceToPlay]);
				// 0 - 1 (1-0=0 1-1=1)
				nextPlacePieceSourceToPlay=1-nextPlacePieceSourceToPlay;
				hasDown=false;
			}
			timeToPlacePiece--;
			if((v.x!=0)||(v.y!=0)||(r.x!=0)||(r.y!=0)||(r.z!=0))
				timeToPlacePiece=WAIT_PLACE;
			if(timeToPlacePiece<=0)
			{
				PlacePiece();
			}
			else
				v.z++;
		}
	}
	//Apply move
	float s=1.f/MOVE_FRAME;
	if(v.y>0) { m.Translate(V3D(0, s,0)); v.y--; CheckSyncMatrix(); }
	if(v.y<0) { m.Translate(V3D(0,-s,0)); v.y++; CheckSyncMatrix(); }
	if(v.x<0) { m.Translate(V3D(-s,0,0)); v.x++; CheckSyncMatrix(); }
	if(v.x>0) { m.Translate(V3D( s,0,0)); v.x--; CheckSyncMatrix(); }
	if(v.z>0) { m.Translate(V3D(0,0,-s)); v.z--; CheckSyncMatrix(); }
	//repeat move
	if(v_time.y>0) { v_time.y--; if(v_time.y==0) myButtons[0]=myButtons[1]=false; }
	if(v_time.x>0) { v_time.x--; if(v_time.x==0) myButtons[2]=myButtons[3]=false; }
	//Apply rotation
	Matrix rot;
	V3D t;
	if(r.x>0) { t=m.Pos(); m=m*rot.SetXRotate(-1.57f/MOVE_FRAME); m.Pos()=t; r.x--; CheckSyncMatrix(); }
	if(r.x<0) { t=m.Pos(); m=m*rot.SetXRotate( 1.57f/MOVE_FRAME); m.Pos()=t; r.x++;	CheckSyncMatrix(); }
	if(r.y>0) { t=m.Pos(); m=m*rot.SetYRotate(-1.57f/MOVE_FRAME); m.Pos()=t; r.y--;	CheckSyncMatrix(); }
	if(r.y<0) { t=m.Pos(); m=m*rot.SetYRotate( 1.57f/MOVE_FRAME); m.Pos()=t; r.y++;	CheckSyncMatrix(); }
	if(r.z>0) { t=m.Pos(); m=m*rot.SetZRotate(-1.57f/MOVE_FRAME); m.Pos()=t; r.z--;	CheckSyncMatrix(); }
	if(r.z<0) { t=m.Pos(); m=m*rot.SetZRotate( 1.57f/MOVE_FRAME); m.Pos()=t; r.z++;	CheckSyncMatrix(); }
	//repeat rotation
	if(r_time.x>0) { r_time.x--; if(r_time.x==0) myButtons[4]=myButtons[5]=false; }
	if(r_time.y>0) { r_time.y--; if(r_time.y==0) myButtons[6]=myButtons[7]=false; }
	if(r_time.z>0) { r_time.z--; if(r_time.z==0) myButtons[8]=myButtons[9]=false; }
}

void Match::Draw2D()
{
	if(state==INVALID) return;
	float w2D=Font::Get2DScreenWidth();
	Font *f=&Game::Get()->font;
	float left=spaceForText*w2D;
	float right=w2D-left;
	char buff[20];

	LocKit *lk=&Game::Get()->locKit;
	f->ShowWStringOnBox(lk->string[LK_INGAME+0][0],right,10,w2D,30,cBlack,Font::CENTER);
	sprintf(buff,"%i",score);
	f->ShowStringOnBox(buff,right,30,w2D,60,cBlack,Font::CENTER);

	f->ShowWStringOnBox(lk->string[LK_INGAME+1][0],right,70,w2D,90,cBlack,Font::CENTER);
	unsigned short tp=Game::Get()->games.current->records[0].points;
	if(tp<score) tp=score;
	sprintf(buff,"%i",tp);
	f->ShowStringOnBox(buff,right,90,w2D,120,cBlack,Font::CENTER);

	f->ShowWStringOnBox(lk->string[LK_INGAME+2][0],0,10,left,30,cBlack,Font::CENTER);
	sprintf(buff,"%i",(level+1));
	f->ShowStringOnBox(buff,0,30,left,90,cBlack,Font::CENTER);
	f->Draw();
/*
	sprintf(buff,"%i",timeToPlacePiece);
	f->ShowStringOnBox(buff,0,470,w2D,480,cBlack,Font::LEFT);
*/

	if(state==GAME_OVER)
	{
		if(record)
			insertName.Draw();
		else
			gameOver.Draw();
	}
}

void Match::SetUpProjMatrix()
{
#ifndef NOGL
	//SETUP CAMERA
	unsigned char sA=hole.GetSideA();
	unsigned char sB=hole.GetSideB();
	unsigned char de=hole.GetDepth();
	float widthGameArea=(float)sA*1.6f;
	float heightGameArea=hole.GetSideB();
	float DepthGameArea=hole.GetDepth();
	Game *g=Game::Get();
	if( widthGameArea*g->RenderDimY>
		heightGameArea*g->RenderDimX)
	{
		heightGameArea=widthGameArea*g->RenderDimY/g->RenderDimX;
	} else
	{
		widthGameArea=heightGameArea*g->RenderDimX/g->RenderDimY;
	}
	moveBack=(sA>sB)? sA : sB;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(
		-widthGameArea*0.5f,widthGameArea*0.5f,
		-heightGameArea*0.5f,heightGameArea*0.5f,
		 moveBack-0.15f,moveBack+DepthGameArea+1);
	glTranslatef(0,0,-moveBack+1.1f);
	sideSize=(widthGameArea-sA)/2;
	spaceForText=sideSize/widthGameArea;
#endif
}

void Match::Draw()
{
	if(state==INVALID) return;
#ifndef NOGL
	bool dCubes=(Game::Get()->currMenu==0);
	SetUpProjMatrix();
	//next piece preview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Matrix nextPos;
	unsigned char sA=hole.GetSideA();
	unsigned char sB=hole.GetSideB();
	unsigned char de=hole.GetDepth();
	nextPos.Reset();
	float s=sideSize*0.3f;
	nextPos.m11=s;
	nextPos.m22=s;
	nextPos.m33=s*0.75f;
	nextPos.Pos().x=sA*0.5f+sideSize-s*0.35f;
	nextPos.Pos().y=0;
	nextPos.Pos().z=-1;
	glLoadMatrixf(nextPos[0]);
	if(state==NORMAL) previewHole.Draw(false,false,false,false,dCubes);
	//on left hole
	Matrix rot; rot.SetYRotate(miniHoleAnglePos*(3.1416f*2)/MINITURNTIME);
	Matrix rot2; rot2.SetXRotate(3.1416f/2.f);
	nextPos=rot2*rot;
	s=(sA>sB) ? sA : sB;
	s=sideSize*0.5f/s;
	nextPos.Right()*=s;
	nextPos.Up()*=s;
	nextPos.At()*=s;
	nextPos.Pos().x=-(sA+sideSize*1.5f)*0.5f;
	nextPos.Pos().y=-sB*0.5f+de*s;
	nextPos.Pos().z=-1-sideSize*0.5f;
	V3D t=nextPos.Pos(); t.z-=moveBack;
	t.y=0;
	glLoadMatrixf(nextPos[0]);
	hole.Draw(
		nextPos.Right()*t>0,nextPos.Up()*t>0,
		nextPos.Right()*t<0,nextPos.Up()*t<0,dCubes);
	//*/
	//the main hole
	glMatrixMode(GL_PROJECTION);
	if(totalShake!=0)
	{
		unsigned char iShake=totalShake-shake-1;
		glTranslatef(0,0,
			-cosf(shake*3.1416f/SHAKE_PERIOD)*iShake*0.01f);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	hole.Draw(true,true,true,true,dCubes);
	if((state==NORMAL)&&(dCubes))
	{
		glLoadMatrixf(m[0]);
		pieces.Draw();
	}

#endif
}
