#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "primitives.h"

//! Class's description
class Image
{
private:
	//! OpenGL texture's ID
	unsigned int textureID;

public:
	//! width of image
	unsigned short dimX;
	//! height of image
	unsigned short dimY;
	//! an array of #dimX * #dimY * #format byte
	char *bits;
	//! possibles format
	enum FORMAT
	{
		INDEFINITE=0,	//!< 0 byte for pixel no information
		GRAYSCALE,		//!< 1 byte per pixel only white intensity
		GRAYALPHA,		//!< 2 byte per pixel white and alpha
		RGB,			//!< 3 byte per pixel Red,Green and blue
		RGBA,			//!< 4 byte per pixel Red,Green,blue and alpha
	} format; //!< image's format


	Image() { dimX=dimY=0; bits=0; format=INDEFINITE; textureID=0; }
	~Image() { Destroy(); }

	//! creates bits of image based of #dimX , #dimY and #format <br>
	//! does checks and set white opaque image
	void Create();
	//! Init #textureID with OpenGL
	void CreateTxt();
	//! Load an PNG imag
	//! \param file file name to load
	//! \note This functions uses libpng (http://www.libpng.org/pub/png/).
	bool LoadPNG(char *file);

	//! Set the texture for OpenGL use
	void SetTxt();

	//! Destroy the image and reset the class to original state
	void Destroy();

	//! Getter of color of one pixel
	Color GetPixel(unsigned short x,unsigned short y);
	//! Setter of color of one pixel
	void SetPixel(unsigned short x,unsigned short y,const Color &col);
};

#endif //__IMAGE_H__

