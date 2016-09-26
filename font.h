#ifndef __FONT_H__
#define __FONT_H__

#include "primitives.h"
#include <wchar.h>

class Image;
//! font managing
class Font
{
private:
	//! max number of characters on a set
	unsigned short maxCharSet;
	//! the indices for set drawing
	unsigned short *indices;
	struct Set
	{
		Image *image;			//!< image with charactes
		unsigned short nChars;	//!< numbers of charactes in this set
		TCV *vertices;			//!< the vertices
		Set *next;				//!< next in list
	};
	//! characters container
	struct Character
	{
		wchar_t character; //!< unicode value of character
		Set* set; //!< set of this character
		V2D ul; //!< upper left corner in texture
		V2D br; //!< bottom right corner in texture

		float aspectRatio; //!< width with unity height

		Character *next; //!< next in list
	};
	//! all sets
	Set *sets;
	//! all characters
	Character *chars;
	//! space size
	//! \note default is 0.25f
	float spaceScale;

	//! Internal use of lists
	Set *AddSet();
	//! Internal use of lists
	Character *AddCharacter(wchar_t charToAdd);
	//! Init a set
	void InitSet(Set *toInit,wchar_t *charList);

	//! Add a character UTF8 for drawing
	//! \param c character to add and draw
	//! \param px horizontal position of character (left border)
	//! \param py vertical position of character (bottom border)
	//! \param height height of character
	//! \param col color of drawing
	//! \return how many put forward
	//! \note return same value of #GetCharWidth
	//! \sa AddWCharToBuffer, GetCharWidth
	float AddCharToBuffer(char c,float px,float py,float height,const Color &col);
	//! Add a character UNICODE for drawing
	//! \param c character to add and draw
	//! \param px horizontal position of character (left border)
	//! \param py vertical position of character (bottom border)
	//! \param height height of character
	//! \param col color of drawing
	//! \return how many put forward
	//! \note return same value of #GetWCharWidth
	//! \sa AddCharToBuffer, GetWCharWidth
	float AddWCharToBuffer(wchar_t c,float px,float py,float height,const Color &col);
	//! Ask for width by UT8 code
	//! \param c character which want information
	//! \return width of character with height = 1
	//! \sa AddCharToBuffer,GetWCharWidth
	float GetCharWidth(char c);
	//! Ask for width by UNICODE code
	//! \param c character which want information
	//! \return width of character with height = 1
	//! \param c carattere di cui avere l'informazione
	//! \sa AddWCharToBuffer,GetCharWidth
	float GetWCharWidth(wchar_t c);
public:
	Font();
	~Font();

	//! Load a font
	//! \param fontDir directory with font files
	//! \param maxChar max number of character in a set (or image)
	void Load(char *fontDir,unsigned short maxChar=200);
	//! Destro the font
	void Destroy();

	//! many types for justification
	enum JUST
	{
		LEFT,		//!< left align
		CENTER,		//!< center align
		RIGHT,		//!< right align
		JUSTIFIED	//!< justification for character of use all witdh
	};

	//! Set of space size
	//! \param spaceScale new space value
	//! \note default value is 0.25f
	//! \sa spaceScale,SetSpaceScaleLikeChar,SetSpaceScaleLikeWChar
	void SetSpaceScale(float spaceScale);
	//! Set of space size like the witdh of UTF8 character #c
	//! \param c character from coping size
	//! \sa spaceScale,SetSpaceScale,SetSpaceScaleLikeWChar
	void SetSpaceScaleLikeChar(char c);
	//! Set of space size like the witdh of UNICODE character #c
	//! \param c character from coping size
	//! \sa spaceScale,SetSpaceScale,SetSpaceScaleLikeChar
	void SetSpaceScaleLikeWChar(wchar_t c);

	//! Get of witdh of a UT8 string with height of 1
	//! \param string input string
	//! \sa GetWStringWidth
	float GetStringWidth(char *string);
	//! Get of witdh of a UNICODE string with height of 1
	//! \param string input string
	//! \sa GetWStringWidth
	float GetWStringWidth(wchar_t *string);

	//! rander and in case empty the buffers
	//! \param clean if true empty the buffers (false for screenShot,not implemented)
	//! \sa #CleanDraw
	void Draw(bool clean=true);

	//! empty the buffers
	void CleanDraw();

	//! Write a UTF8 string inside a box
	//! \param string string to write
	//! \param x0 left border of box
	//! \param y0 upper border of box
	//! \param x1 right border of box
	//! \param y1 legt  border of box
	//! \param col color of string
	//! \param just justification
	//! \sa GetStringWidth, ShowWStringOnBox
	float ShowStringOnBox(char *string,float x0, float y0,float x1, float y1,const Color &col,JUST just);
	//! Write a UNICODE string inside a box
	//! \param string string to write
	//! \param x0 left border of box
	//! \param y0 upper border of box
	//! \param x1 right border of box
	//! \param y1 legt  border of box
	//! \param col color of string
	//! \param just justification
	//! \sa GetWStringWidth, ShowStringOnBox
	float ShowWStringOnBox(wchar_t *string,float x0, float y0,float x1, float y1,const Color &col,JUST just);

	//! Getter of position of a character inside a UTF8 string in a box
	//! \param string string to write
	//! \param x0 left border of box
	//! \param y0 upper border of box
	//! \param x1 right border of box
	//! \param y1 legt  border of box
	//! \param col color of string
	//! \param just justification
	//! \param id indices of character
	//! \sa GetStringWidth, ShowWStringOnBox
	float GetPosCharOnStringOnBox(char *string,float x0, float y0,float x1, float y1,JUST just,unsigned char id);
	//! Getter of position of a character inside a UNICODE string in a box
	//! \param string string to write
	//! \param x0 left border of box
	//! \param y0 upper border of box
	//! \param x1 right border of box
	//! \param y1 legt  border of box
	//! \param col color of string
	//! \param just justification
	//! \param id indices of character
	float GetPosWCharOnStringOnBox(wchar_t *string,float x0, float y0,float x1, float y1,JUST just,unsigned char id);

	//! Ask for a UTF8 character is in font
	//! \sa IsOnSetW
	bool IsOnSet(char c);
	//! Ask for a UNICODE character is in font
	//! \sa IsOnSet
	bool IsOnSetW(wchar_t c);

	//! Width of screen if height is 480
	static float Get2DScreenWidth();
	//! Setting 2D rendering matrices (height =480)
	static void SetupMatricesFor2D();
	//! Translate the mouse position in 2D screen(height =480)
	static void ConvertMouse(float *x,float *y);

	//! return the numver of valid character in UNICODE string
	//! \sa IsOnSetW
	long GetWStringLen(const wchar_t *text);
};

#endif //__FONT_H__
