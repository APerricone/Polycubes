#include "game.h"
#include "systemInterface.h"
#include "wave.h"

Game *Game::gameStc;

void Game::Init()
{
#ifndef NOGL
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
	glEnable(GL_COLOR_MATERIAL);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(1,1,1,1);
#endif
	font.Load("font");
	rects2D.Init();
	arrows.Init();
	circles.Init();

	locKit.Init(currLang);

	currMenu=new MainMenu();
	currMenu->Init();
	InitOggVorbis();
};


bool Game::Update()
{
	//flash on start
	if(Font::Get2DScreenWidth()<100)
		return true;
	bool r=true;
	if(currMenu)
		r=currMenu->Update();
	match.Update();

	mouse.last=mouse.btn;
	return r;
};

//#define GETTIME
void Game::Draw()
{
#ifndef NOGL
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
#endif
	match.Draw();
	if(currMenu) currMenu->Draw3D();

	Font::SetupMatricesFor2D();
	glDisable(GL_DEPTH_TEST);
	match.Draw2D();
	if(currMenu) currMenu->Draw();
	glEnable(GL_DEPTH_TEST);
};

void Game::DeInit()
{
	gameStc->games.Save();
	if(gameStc->currMenu)
		delete gameStc->currMenu;
	delete gameStc;
}
