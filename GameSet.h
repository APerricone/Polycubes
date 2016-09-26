#ifndef __GAMESET_H__
#define __GAMESET_H__

#include "Pieces.h"
#include <stdio.h>
#define NAME_LEN 50
class Games;
class GameSet;
class PieceSet;

//! A set of pieces
class PieceSet
{
private:
	unsigned char nPieces;
public:
	wchar_t setName[NAME_LEN];
	bool canAppear[N_PIECES];
	PieceSet *next;

	PieceSet() { next=0; nPieces=0x0FF; }
	void Load(FILE *);
	void Save(FILE *);

	friend bool operator==(const PieceSet &a,const PieceSet &b);

	unsigned char GetRandomPiece();

};

//! A game mode
class GameSet
{
public:
	wchar_t gameName[NAME_LEN];
	char sideA,sideB,depth;
	struct Records
	{
		char pos;
		wchar_t name[NAME_LEN];
		unsigned long points;
	} records[10];
	PieceSet *currSet;
	GameSet *next;

	GameSet();
	//! try to add a record in classification, return the pointer of #Record
	//! NULL if point is too low.
	Records *AddRecord(unsigned long points);

	void Load(FILE *,Games *parent);
	void Save(FILE *);

	friend bool operator==(const GameSet &a,const GameSet &b);
};

//! All games mode container
class Games
{
public:
	GameSet *gameList,*current;
	PieceSet *pieceSetList;

	Games() { gameList=current=0; pieceSetList=0; }
	~Games();

	void Load();
	void Save();

	//! Add or replace a Game mode, check the name
	GameSet *AddGame(const GameSet& tmp);
	//! Add or replace a pieces set, check the name
	PieceSet *AddPieceSet(const PieceSet& tmp);
	//! Search a game mod same of tmp (all config)
	GameSet *GetGame(const GameSet& tmp);
	//! Search a piece set same of tmp (all config)
	PieceSet *GetPieceSet(const PieceSet& tmp);


	//! Del the game from list
	void DelGame(const GameSet& tmp);
	//! Del the piece set from list
	void DelPieceSet(const PieceSet& tmp);

	//! return true if the piece set is not used by a game mode
	bool CanDeletePieceSet(const PieceSet& tmp);

	//! Search a game mode by name
	GameSet *GetGameFromName(const wchar_t* tmp);
	//! Search a piece set by name
	PieceSet *GetPieceSetFromName(const wchar_t* tmp);
};

#endif //__GAMESET_H__
