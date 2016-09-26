#include "image.h"
#include "game.h"
#include "systemInterface.h"

#include "png.h"
#pragma comment(lib,"libpng.lib")

void Image::Create()
{
	if((dimX==0)||(dimY==0)||(format==INDEFINITE))
	{
		message("Image::Create","image of %ix%ix%i can not be create",dimX,dimY,format);
		return;
	}
	if(bits!=0)  { message("Image::Create","Image already created"); return; }
	long tot=dimX*dimY*format;
	bits=new char[tot];
	memset(bits,255,tot);
}

void Image::CreateTxt()
{
	if(bits==0)  { message("Image::CreateTxt","Image unspecified"); return; }
	unsigned short pow2x=1,pow2y=1;
	while(pow2x<dimX) pow2x<<=1;
	while(pow2y<dimY) pow2y<<=1;
	if((pow2x!=dimX)||(pow2y!=dimY))
	{
		message("Image::CreateTxt","The image %ix%i needs a texture of %ix%i. The change is not implemented" ,dimX,dimY,pow2x,pow2y);
		return;
	}
	if(textureID!=0) {  message("Image::CreateTxt","Texture already created"); return;  }

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLint type,iFormat;
	switch(format)
	{
	case 1: type=GL_LUMINANCE; iFormat=GL_LUMINANCE8; break;
	case 2: type=GL_LUMINANCE_ALPHA; iFormat=GL_LUMINANCE8_ALPHA8; break;
	case 3: type=GL_RGB; iFormat=GL_RGB8; break;
	case 4: type=GL_RGBA; iFormat=GL_RGBA8; break;
	}

	glTexImage2D(GL_TEXTURE_2D,0,iFormat,dimX,dimY,0, type,GL_UNSIGNED_BYTE,bits);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void my_read(png_structp png_ptr,unsigned char *dest,unsigned int n)
{
	fread(dest,n,1,(FILE*)png_ptr->io_ptr);
}

bool Image::LoadPNG(char *fileName)
{
	FILE *fp = fopen(fileName, "rb");
	if (!fp) { message("Image::LoadPNG","file %s don't found",fileName); return false; }
#define NUM_HEADER 8
	unsigned char header[NUM_HEADER];
	fread(header, 1, NUM_HEADER, fp);
	bool is_png = !png_sig_cmp(header, 0, NUM_HEADER);
	if (!is_png) { message("Image::LoadPNG","il file %s is not a valid png",fileName); fclose(fp); return false; }
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	png_infop info_ptr=png_create_info_struct(png_ptr);
	png_infop end_ptr=png_create_info_struct(png_ptr);

	png_init_io(png_ptr, fp);
	png_set_read_fn(png_ptr, fp, my_read);
	png_set_sig_bytes(png_ptr, NUM_HEADER);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	dimX=info_ptr->width;
	dimY=info_ptr->height;
	format=(FORMAT)info_ptr->channels;
	if(info_ptr->bit_depth!=8) { message("Image::LoadPNG","bad format, %i bit for channel instead 8bit",info_ptr->bit_depth);
						png_destroy_read_struct(&png_ptr,&info_ptr,0); fclose(fp); return false; }
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	Create();
	for(unsigned short y=0;y<dimY;y++)
		memcpy(&bits[y*dimX*format],row_pointers[y],dimX*format);

	png_destroy_read_struct(&png_ptr,&info_ptr,&end_ptr);
	fclose(fp);
	return true;
}

void Image::SetTxt()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Image::Destroy()
{
	SAFE_DELETE_ARRAY(bits);
	SAFE_DELETE_TEX(textureID);
	dimX=dimY=0;
	format=INDEFINITE;
}

Color Image::GetPixel(unsigned short x,unsigned short y)
{
	if(format==INDEFINITE) return Color(0,0,0,0);
	if(bits==0) return Color(0,0,0,0);
	if(x>dimX) return Color(0,0,0,0);
	if(y>dimY) return Color(0,0,0,0);
	char *src=&bits[(y*dimX+x)*format];
	switch(format)
	{
	case GRAYSCALE: return Color(src[0],src[0],src[0], 255  );
	case GRAYALPHA: return Color(src[0],src[0],src[0],src[1]);
	case   RGB    : return Color(src[0],src[1],src[2],255);
	case   RGBA   : return Color(src[0],src[1],src[2],src[3]);
	}
	return Color(0,0,0,0);
}

void Image::SetPixel(unsigned short x,unsigned short y,const Color &col)
{
	if(format==INDEFINITE) return;
	if(bits==0) return;
	if(x>dimX) return;
	if(y>dimY) return;
	char *dst=&bits[(y*dimX+x)*format];
	switch(format)
	{
	case GRAYSCALE: dst[0]=((short)col.r+(short)col.g+(short)col.b)/3; break;
	case GRAYALPHA: dst[0]=((short)col.r+(short)col.g+(short)col.b)/3; dst[1]=col.a; break;
	case   RGB    : dst[0]=col.r; dst[1]=col.g; dst[2]=col.b; break;
	case   RGBA   : dst[0]=col.r; dst[1]=col.g; dst[2]=col.b; dst[3]=col.a; break;
	}
}
