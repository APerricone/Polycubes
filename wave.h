#ifndef __WAVE_H__
#define __WAVE_H__

void OpenWave(char *nameFile,unsigned int buffer);

//#include "game.h"
#include <al.h>
#include <Vorbis\vorbisfile.h>

#define N_BUFFER 12
struct MyOggVorbisData
{
	OggVorbis_File ovf;
	unsigned int alSource;
	unsigned int alBuffer[N_BUFFER];
	ALenum format;
	unsigned long freq;
	unsigned long bSize;
	char *data;
	bool ended;
	bool play;
	bool exPlay; //only for thread
	MyOggVorbisData() { alSource=0; data=0; }
};

void InitOggVorbis();
MyOggVorbisData *BeginOggVorbis(char *nameFile,MyOggVorbisData *toRet);
void EndOggVorbis(MyOggVorbisData *dest);

#endif //__WAVE_H__
