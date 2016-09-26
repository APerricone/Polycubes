#ifndef _SI_H__
#define _SI_H__
//Keyboard Input
#define NUM_BUTTON 12
#ifdef WINMAIN_CALL
const short keys[NUM_BUTTON] = {
	VK_UP,VK_DOWN,VK_LEFT,VK_RIGHT,
	'A','Q','S','W','D','E',
	VK_SPACE,VK_ESCAPE  };
bool buttons[NUM_BUTTON];
bool sound;
#else
extern bool buttons[NUM_BUTTON];
extern bool sound;
#endif


#include <wchar.h>

#define KB_QUEUE_LEN 5
extern wchar_t keyboardInput[KB_QUEUE_LEN];
extern bool specialKey[KB_QUEUE_LEN];
bool IsShiftPress();

struct Mouse
{
	short x,y;
	bool btn,rbtn;
	bool last;
};
#ifdef WINMAIN_CALL
Mouse mouse;
#else
extern Mouse mouse;
#endif

//Dialogs
void message(char* title,char* text, ... );

#define SAFE_DELETE(a) if(a) delete a; a=0;
#define SAFE_DELETE_ARRAY(a) if(a) delete [] a; a=0;

//Special Credits
void OpenLink(char *url);
void SetHandCursor(bool w);

#endif //_SI_H__
