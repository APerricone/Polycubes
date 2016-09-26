#ifndef __GAME_H__
#define __GAME_H__

#ifdef _WINDOWS
#include <windows.h>
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glu32.lib")
#pragma comment (lib,"openal32.lib")
#endif

#include <gl/gl.h>
#include <gl/glu.h>
#include <al.h>
#include <alc.h>

#include "match.h"
#include "font.h"
#include "Menus.h"
#include "MenuElements.h"
#include "GameSet.h"
#include "locKit.h"

//!
class Game
{
private:
	void *systemVar;

	static Game *gameStc;
	Game()  { gameStc=this; }
	~Game() { gameStc=0; }

public:
	unsigned char nThread;
	unsigned char radQuality; // 2^(3+radQuality)
	unsigned char sfxVolume;
	unsigned char musicVolume;
	unsigned char currLang;
	unsigned char startLevel;
	char currDevice;

	int RenderDimX;
	int RenderDimY;
	Match match;
	Font font;
	Rects2D rects2D;
	Arrow2D arrows;
	Circles circles;
	Games games;
	MenuBase *currMenu;
	LocKit locKit;


	static Game *Get() { if(gameStc) return gameStc; else return new Game(); }
	void Init();
	//! to call every 1/60 seconds
	bool Update();
	void Draw();
	static void DeInit();
};

#define SAFE_DELETE_TEX(a) if(a) glDeleteTextures(1,&a); a=0;

#endif //__GAME_H__
