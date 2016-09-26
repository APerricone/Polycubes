#ifndef __LOCKIT_H__
#define __LOCKIT_H__

#define LK__NAMELANG	0
#define LK_AUTHOR 		1
#define LK_MAINMENU 	3
#define LK_EXITQUERY 	10
#define LK_PAUSE 		11
#define LK_INGAME		14
#define LK_ENDGAME		17
#define LK_EDIT			28
#define LK_EDIT_MESSAGE	36
#define LK_OPTIONS		44
#define LK_DIALOGS		52
#define LK_HELP			56
#define LK_TOTAL		69
#define MAX_LEN 		100

class LocKit
{
public:
	wchar_t string[LK_TOTAL][2][MAX_LEN];


	unsigned char CountLangFile();
	void GetLangName(unsigned char i,wchar_t *dest,unsigned long len);

	void Init(unsigned char lang);
};

#endif //__LOCKIT_H__
