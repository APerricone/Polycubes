#ifndef __MENUS_H__
#define __MENUS_H__

#include "primitives.h"
#include "menuElements.h"

//! a dialog box with a message and 1 or 2 buttons
class Dialog
{
public:
	enum TYPE
	{
		NOSET,
		OK,
		YESNO,
		EDIT
	};
protected:
	wchar_t *message[2];

	bool move;
	V2D start;

	TYPE t;

	Button button[2];
	Box2 allDlg,text[2];
public:

	Dialog() { t=NOSET; move=false; }
	~Dialog() { DeInit(); }

	void Init(wchar_t *msg1,wchar_t *msg2,TYPE _t,float w=0);
	unsigned char Update();
	void Draw();
	void DeInit();
};

//! a dialog box with text entry
class DialogEdit : private Dialog
{
private:
	EditBox edit;
	wchar_t *dest;
public:

	DialogEdit() : Dialog() { t=EDIT; }
	~DialogEdit() { DeInit(); }

	void Init(wchar_t *msg1,wchar_t *_dest,unsigned char len,float w);
	unsigned char Update();
	void Draw();
	void DeInit();
};

//! Elemento base dei menù
class MenuBase
{
public:
	virtual void Init() {};
	virtual bool Update() { return true; };
	virtual void Draw3D() {};
	virtual void Draw() {};
	virtual void Deinit() {};
};

//! the start screen
class MainMenu : public MenuBase
{
private:
	Dialog *exitConfirm;
	Button buttons[6];
	ComboBox games;
	Slider startLevel;

	float myNameW;
	float PolycubesSize;
public:

	MainMenu() { exitConfirm=0; }
	~MainMenu() { Deinit(); }
	virtual void Init();
	virtual bool Update();
	virtual void Draw3D() {};
	virtual void Draw();
	virtual void Deinit();
};

#include "hole.h"
#include "pieces.h"
#include "GameSet.h"

//! editor of game
class EditGame : public MenuBase
{
private:
	//! the pieces show
	Hole holes[N_PIECES];
	unsigned char currCalc; //!< actual piece in calculation
	void GetPieceBox(const unsigned char i,Box2 *toRet);
	unsigned char currPiece; //!< selected piece (with mouse)

	enum State
	{
		NONE,
		ADD_PIECE,
		DEL_PIECE,
		QUERY_NEW_GAME_NAME,
		QUERY_NEW_PSET_NAME,
		CONFIRM_DELETE_GAME,
		CONFIRM_DELETE_PSET,
		CONFIRM_OVERWRITE_GAME,
		CONFIRM_OVERWRITE_PSET,
		MESSAGE
	} state;

	ComboBox games;
	Button saveGame,delGame;
	Slider width,height,depth;
	ComboBox pieces;
	Button savePieces,delPieces;

	Dialog confirms;
	DialogEdit insertName;

	Button exit;

	Games *gg;
	GameSet currGame;
	PieceSet currPSet;
public:
	EditGame() { }
	~EditGame() { Deinit(); }
	virtual void Init();
	virtual bool Update();
	virtual void Draw3D();
	virtual void Draw();
	virtual void Deinit();
};

#include "locKit.h"

//! option screen
class Options : public MenuBase
{
private:
	Button doBenchMark;
	//! setting the sizo of radiosity texture:
	//! * low - 8
	//! * medium -16
	//! * high -32
	//! * very high -64
	Slider quality;
	SliderLine sfxVol;
	SliderLine musicVol;
	ComboBox lang;
	ComboBox sDevice;
	Button exit;

	wchar_t langNames[10][MAX_LEN];
	wchar_t deviceNames[10][MAX_LEN];

	unsigned char waitforMessage;
public:

	Options() { waitforMessage=0; }
	~Options() { Deinit(); }
	virtual void Init();
	virtual bool Update();
	virtual void Draw3D() {};
	virtual void Draw();
	virtual void Deinit();
};

//! interface for menù in game (esc key)
class inGame : public MenuBase
{
private:
	Box2 bbox;
	Button continueGame;
	Button exit;

	bool move;
	bool lastEsc;
	V2D start;
public:

	inGame() { move=false; lastEsc=true;}
	~inGame() { Deinit(); }
	virtual void Init();
	virtual bool Update();
	virtual void Draw3D() {};
	virtual void Draw();
	virtual void Deinit();
};

//! classification show
class Records : public MenuBase
{
private:
	ComboBox games;
	Games *gg;
	GameSet *currGame;
	Button exit;
#ifdef DEBUG
	Button reset;
#endif
public:
	Records() { }
	~Records() { Deinit(); }
	virtual void Init();
	virtual bool Update();
	virtual void Draw3D() {}
	virtual void Draw();
	virtual void Deinit() {};
};

//! hidden screen, need click on my name in main screen
class Gallery : public MenuBase
{
private:
	//preview
	Hole holeShow;
	unsigned char currPiece;
	Slider selPiece;
	Button exit;

	float alpha,beta;
	short exMx,exMy;

	void SetPiece();
public:
	Gallery() { }
	~Gallery() { Deinit(); }
	virtual void Init();
	virtual bool Update();
	virtual void Draw3D();
	virtual void Draw();
	virtual void Deinit();
};

#include "pieces.h"

class Image;
class Help : public MenuBase
{
private:
	Pieces model;
	Slider pageChange;
	Button exit;
	//! every think live 1 second, in this time the key flash 2 times
	//! 1 for move and 4 for wait in 60 frames
	//! right
	//! down
	//! left
	//! up
	//! space
	//! totals 5 anims
	unsigned char currKey;
	unsigned char animTime;
	Matrix mMove;

	Matrix m[6];
	float angle;

	void DrawPage1(Font *f,Rects2D *r,Arrow2D *a,LocKit *lk,float w2D,float center);
	void DrawPage2(Font *f,Rects2D *r,Arrow2D *a,LocKit *lk,float w2D,float center);
	void DrawPage3(Font *f,Rects2D *r,Arrow2D *a,LocKit *lk,float w2D,float center);

	Image *libs[2];
public:
	Help() { libs[0]=libs[1]=0; }
	~Help() { Deinit(); }
	virtual void Init();
	virtual bool Update();
	virtual void Draw3D();
	virtual void Draw();
	virtual void Deinit();
};

#endif //__MENUMANAGER_H__
