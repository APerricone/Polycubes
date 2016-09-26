#ifndef __MENUELEMENTS_H__
#define __MENUELEMENTS_H__

class Image;
#include "primitives.h"
#include "Font.h"

//! Drawing of 2D rectangles of varius type
class Rects2D
{
private:
	//! Image of rectangle only border
	Image *txtFlat;
	//! Image of rectangle filled
	Image *txtFill;
	//! vertices for draw
	TV v[20];
	//! indices for draw reclangle empty
	unsigned char idx[54];
	//! indices for draw reclangle filled
	unsigned char idxf[54];
	//! Image of rectagle with thick border
	Image *txtTube;
	//! Image for glass effect
	Image *txtGlass;
	//! Image for glass shadow
	Image *txtGlassShadow;

	//! fragment program for draw thick border
	unsigned int tubeProg;
	//! fragment program for draw glass effect
	unsigned int glassProg;
	//! texture of screen(glass effect)
	unsigned int txtScreen;
	//! width of texture of screen
	unsigned int txtScreenDimX;
	//! height of texture of screen
	unsigned int txtScreenDimY;
public:
	//! flag of fragment capacity
	bool fragProg;

	Rects2D() { txtFlat=txtFill=txtTube=txtGlass=0; }
	~Rects2D() { DeInit(); }

	//! inizialize the class
	void Init();
	//! drawing an empty rectange
	void Draw(float x0,float y0,float x1,float y1,float topHole0=-1,float topHole1=-1);
	//! drawing a rectangle with thick border
	void DrawTube(float x0,float y0,float x1,float y1);
	//! draw a filled rectangle
	void DrawFill(float x0,float y0,float x1,float y1);
	//! draw a rectangle with glass effect
	void DrawGlass(float x0,float y0,float x1,float y1);
	//! draw the shadow of glass
	void DrawGlassShadow(float x0,float y0,float x1,float y1);
	//! destroy all
	void DeInit();
};

//! drawing of a triangle
class Arrow2D
{
private:
public:
	Arrow2D() {};
	~Arrow2D() { DeInit(); };

	void Init() {}
	void DrawUp(float x0,float y0,float x1,float y1,bool white=false,bool hole=false);
	void DrawRight(float x0,float y0,float x1,float y1,bool white=false,bool hole=false);
	void DrawDown(float x0,float y0,float x1,float y1,bool white=false,bool hole=false);
	void DrawLeft(float x0,float y0,float x1,float y1,bool white=false,bool hole=false);
	void DeInit() {}
};

//! draw of a circles
class Circles
{
private:
public:
	Circles() {};
	~Circles() { DeInit(); };

	void Init() {}
	void Draw(float cx,float cy,float r);
	void DrawFill(float cx,float cy,float r,float angle1,float angle2);
	void DeInit() {}
};

//! base interface glass button
class Button
{
private:
public:
	Box2 bbox;
	wchar_t *text;
	enum State
	{
		NORMAL,
		SELECT,
		PRESS,
		DISABLE
	} state;

	Button() { state=NORMAL; text=L""; }
	bool Update(const V2D &mouse,bool mouseBtn,bool lastBtn);
	void Draw(Font *f,Rects2D *r);
};

//! base interface edit box
class EditBox
{
private:
	unsigned char len;
	unsigned char currentLen;
	wchar_t *text;

	char howMove;
	unsigned char cursorStart;
	unsigned char cursorEnd;
	unsigned char cursorTime;

	static EditBox *currEdit;
public:

	static EditBox *GetCurrentEdit() { return currEdit; }
	Box2 bbox;

	enum State
	{
		NORMAL,
		EDIT,
		SELECTING,
		DISABLE
	} state;

	EditBox() { state=NORMAL; text=0; len=0; }
	void Init(unsigned char len);
	void SetText(const wchar_t *text);
	void Paste(const wchar_t *text);
	const wchar_t *GetText() const { return text; }
	void Update(const V2D &mouse,bool mouseBtn);
	void Draw(Font *f,Rects2D *r);
private:
	unsigned char GetChar(float px);
};

//! base interface scroll bar
class ScrollBar
{
private:
	Box2 ul,br,bar,cursor;
	float deltaPos;
	char time,currLimit,counterVel;
public:
	Box2 bbox;
	bool vertical;
	enum State
	{
		NORMAL,
		CLICK_UL,
		CLICK_BR,
		MOVING
	} state;
	//! totale della barra
	long total;
	//! dimenzione del cursore
	long size;
	//! posizione del cursore
	//! \note va da 0 a total-size
	long pos;

	ScrollBar() { state=NORMAL; total=1; size=1; pos=0; }
	void Update(const V2D &mouse,bool mouseBtn,bool lastBtn);
	void Draw(Rects2D *r,Arrow2D *a);
};

//! base interface Slider (a number with 2 triangle on side)
class Slider
{
private:
	Box2 ul,br,text;
	float deltaPos;
	char time,currLimit,counterVel;
	bool withMax;
public:
	Box2 bbox;
	bool vertical;
	enum State
	{
		NORMAL,
		CLICK_UL,
		CLICK_BR,
		MOVING
	} state;
	//! valore Minimo
	long minValue;
	//! valore Massimo
	long maxValue;
	//! passi
	long stepValue;
	//! valore attuale
	long pos;

	Slider() { state=NORMAL; pos=0; withMax=false; }
	void Update(const V2D &mouse,bool mouseBtn,bool lastBtn);
	void Draw(Font *f,Arrow2D *a);

	bool IsInside(const V2D &p) { return (ul.IsInside(p)||(br.IsInside(p))); }
	void SetWithMax(const bool v) { withMax=v; }
};

//! base interface graphic Slider (volume seletor)
class SliderLine
{
private:
	Box2 ul,br;
public:
	Box2 bbox;
	enum State
	{
		NORMAL,
		MOVING
	} state;
	//! valore Massimo
	long size;
	//! valore attuale
	long pos;
	//! passi
	long steps;

	SliderLine() { state=NORMAL; pos=0; steps=1; }
	void Update(const V2D &mouse,bool mouseBtn,bool lastBtn);
	void Draw(Rects2D *r);

	bool IsInside(const V2D &p) { return (ul.IsInside(p)||(br.IsInside(p))); }
};

//! combo box
class ComboBox
{
private:
	ScrollBar scrollBar;
	struct ListElement
	{
		wchar_t *name;
		void *ref;
		ListElement *next;
	} *list;
	ListElement *actual;
	ListElement *sel;
	unsigned char nList;

	ListElement *Get(unsigned char i);
	ListElement *Get(wchar_t *name);
	ListElement *GetRef(void *ref);
	unsigned char GetIdx(ListElement *);
	void Del(ListElement *);
	Box2 bboxOpen;
public:
	wchar_t *nameWithOutSel;
	Box2 bbox;
	float bottomY;

	enum State
	{
		NORMAL,
		OPEN,
		DISABLE
	} state;

	ComboBox();
	~ComboBox();
	void ResetList();
	void AddElementToList(wchar_t *newElement,void *ref);
	unsigned char GetListLen() const { return nList; }
	void *GetElement(unsigned char i) { ListElement *e=Get(i); if(e) return e->ref; return 0; }
	void *GetElement(wchar_t *name) { ListElement *e=Get(name); if(e) return e->ref; return 0; }
	wchar_t *GetElementName(unsigned char i) { ListElement *e=Get(i); if(e) return e->name; return 0; }
	void Select(unsigned char i) { actual=Get(i); }
	void Select(wchar_t *name) { actual=Get(name); }
	void SelectRef(void *ref) { actual=GetRef(ref); }
	void *GetSelElement() { if(actual) return actual->ref; return 0; }
	wchar_t *GetSelElementName() { if(actual) return actual->name; return 0; }


	void DelElement(unsigned char i) { Del(Get(i)); }
	void DelElement(wchar_t *name) { Del(Get(name)); }
	void DelRef(void *ref) { Del(GetRef(ref)); }
	void DelSelect() { Del(actual); actual=0;; }

	// torna 0 quando non ha il controllo
	// torna 1 quando è aperto
	// torna 2 quando si è appena chiuso
    unsigned char Update(const V2D &mouse,bool mouseBtn,bool lastBtn);
	void Draw(Font *f,Rects2D *r,Arrow2D *a);
};

#endif //__MENUELEMENTS_H__
