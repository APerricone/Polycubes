#include "game.h"
#include "wave.h"

#include <Vorbis\vorbisfile.h>
void OpenWave(char *nameFile,unsigned int buffer)
{
	FILE *f=fopen(nameFile,"rb");
	if(f==0)
	{
		message("OpenWave","I can not open %s",nameFile);
		return;
	}
	char buff[10];
	fread(buff,1,4,f);
	if(memcmp(buff,"RIFF",4)!=0)
	{
		fclose(f);
		message("OpenWave","file %s is not RIFF",nameFile);
		return;
	}
	DWORD totalFileLen; fread(&totalFileLen,4,1,f);
	fread(buff,1,4,f);
	if(memcmp(buff,"WAVE",4)!=0)
	{
		fclose(f);
		message("OpenWave","file %s is not WAVE",nameFile);
		return;
	}
	DWORD lenThis,lenData;
	bool knowd;
	WAVEFORMATEX wfx;
	unsigned char *data;
	while(1)
	{
		fread(buff,1,4,f);
		if(feof(f)) break;
		fread(&lenThis,4,1,f);
		knowd=false;
		if(memcmp(buff,"fmt ",4)==0)
		{
			knowd=true;
			if(lenThis>sizeof(WAVEFORMATEX))
			{
				fread(&wfx,1,sizeof(WAVEFORMATEX),f);
				fseek(f,lenThis-sizeof(WAVEFORMATEX),SEEK_CUR);
			} else
				fread(&wfx,1,lenThis,f);
		}
		if (memcmp(buff,"data",4)==0)
		{
			knowd=true;
			lenData=lenThis;
			data=new unsigned char [lenData];
			fread(data,1,lenData,f);
		}
		if(!knowd)
			fseek(f,lenThis,SEEK_CUR);
	};
	fclose(f);
	ALenum format;
	if(wfx.nChannels==1)
	{
		if(wfx.wBitsPerSample==8)
			format=AL_FORMAT_MONO8;
		else
			format=AL_FORMAT_MONO16;
	} else
	{
		if(wfx.wBitsPerSample==8)
			format=AL_FORMAT_STEREO8;
		else
			format=AL_FORMAT_STEREO16;
	}
	alBufferData(buffer,format,data,lenData,wfx.nSamplesPerSec);
}

size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return fread(ptr, size, nmemb, (FILE*)datasource);
}
int ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	return fseek((FILE*)datasource, (long)offset, whence);
}
int ov_close_func(void *datasource) { return fclose((FILE*)datasource); }
long ov_tell_func(void *datasource) { return ftell((FILE*)datasource); }
typedef int (*LPOVCLEAR)(OggVorbis_File *vf);
typedef long (*LPOVREAD)(OggVorbis_File *vf,char *buffer,int length,int bigendianp,int word,int sgned,int *bitstream);
typedef ogg_int64_t (*LPOVPCMTOTAL)(OggVorbis_File *vf,int i);
typedef vorbis_info * (*LPOVINFO)(OggVorbis_File *vf,int link);
typedef vorbis_comment * (*LPOVCOMMENT)(OggVorbis_File *vf,int link);
typedef int (*LPOVOPENCALLBACKS)(void *datasource, OggVorbis_File *vf,char *initial, long ibytes, ov_callbacks callbacks);

bool canOggVorbis=false;
LPOVCLEAR			fn_ov_clear = 0;
LPOVREAD			fn_ov_read = 0;
LPOVPCMTOTAL		fn_ov_pcm_total = 0;
LPOVINFO			fn_ov_info = 0;
LPOVCOMMENT			fn_ov_comment = 0;
LPOVOPENCALLBACKS	fn_ov_open_callbacks = 0;
HINSTANCE			g_hVorbisFileDLL = 0;
void InitOggVorbis()
{
	if(canOggVorbis) return;
	canOggVorbis=false;
	HINSTANCE g_hVorbisFileDLL = LoadLibrary("vorbisfile.dll");
	if(g_hVorbisFileDLL==0) return;
	fn_ov_clear = (LPOVCLEAR)GetProcAddress(g_hVorbisFileDLL, "ov_clear");
	fn_ov_read = (LPOVREAD)GetProcAddress(g_hVorbisFileDLL, "ov_read");
	fn_ov_pcm_total = (LPOVPCMTOTAL)GetProcAddress(g_hVorbisFileDLL, "ov_pcm_total");
	fn_ov_info = (LPOVINFO)GetProcAddress(g_hVorbisFileDLL, "ov_info");
	fn_ov_comment = (LPOVCOMMENT)GetProcAddress(g_hVorbisFileDLL, "ov_comment");
	fn_ov_open_callbacks = (LPOVOPENCALLBACKS)GetProcAddress(g_hVorbisFileDLL, "ov_open_callbacks");
	if (fn_ov_clear && fn_ov_read && fn_ov_pcm_total && fn_ov_info &&
		fn_ov_comment && fn_ov_open_callbacks)
			canOggVorbis = true;
}

void DeinitOggVorbis()
{
	if(!canOggVorbis) return;
	FreeLibrary(g_hVorbisFileDLL); g_hVorbisFileDLL=0;

	canOggVorbis=false;
}


unsigned long DecodeOggVorbis(
	OggVorbis_File *psOggVorbisFile,
	char *pDecodeBuffer, unsigned long ulBufferSize)
{
	int current_section;
	long lDecodeSize;

	unsigned long ulBytesDone = 0;
	while (1)
	{
		lDecodeSize = fn_ov_read(psOggVorbisFile, pDecodeBuffer + ulBytesDone, ulBufferSize - ulBytesDone, 0, 2, 1, &current_section);
		if (lDecodeSize > 0)
		{
			ulBytesDone += lDecodeSize;

			if (ulBytesDone >= ulBufferSize)
				break;
		}
		else
		{
			break;
		}
	}
	return ulBytesDone;
}

DWORD WINAPI PlayOggVorbis(LPVOID lpParameter)
{
	MyOggVorbisData *pr=(MyOggVorbisData*)lpParameter;
	unsigned char toWrite=0;
	unsigned long nSet;
	for(toWrite=0;toWrite<N_BUFFER;toWrite++)
	{
		nSet=DecodeOggVorbis(&pr->ovf,pr->data,pr->bSize);
		alBufferData(pr->alBuffer[toWrite],pr->format,pr->data,nSet,pr->freq);
		alSourceQueueBuffers(pr->alSource,1,&pr->alBuffer[toWrite]);
	}
	alSourcePlay(pr->alSource);
	pr->play=true;
	toWrite=0;
	int nProc;
	int iState;
	unsigned int buff;
	while(1)
	{
		nProc=0;
		if(!pr->play)
		{
			if(pr->exPlay)
				alSourcePause(pr->alSource);
			Sleep(100);
			pr->exPlay=pr->play;
			continue;
		}
		if(!pr->exPlay) alSourcePlay(pr->alSource);
		pr->exPlay=pr->play;
        if(pr->ended) break;
 		alGetSourcei(pr->alSource, AL_BUFFERS_PROCESSED, &nProc);
        while(nProc)
        {
            // Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
            buff = 0;
            alSourceUnqueueBuffers(pr->alSource, 1, &buff);
            // Read more audio data (if there is any)
			nSet=DecodeOggVorbis(&pr->ovf,pr->data,pr->bSize);
			if(nSet)
			{
                alBufferData(buff,pr->format,pr->data,nSet,pr->freq);
                alSourceQueueBuffers(pr->alSource,1,&buff);
            }
            nProc--;
        }
		alGetSourcei(pr->alSource, AL_SOURCE_STATE, &iState);
		if (iState != AL_PLAYING)
		{
            alGetSourcei(pr->alSource, AL_BUFFERS_QUEUED, &iState);
            if (iState)
            {
                alSourcePlay(pr->alSource);
            }
            else
            {
                pr->ended=true;
                break;
            }
		}
	    Sleep(100);
	};
	return 0;
}

MyOggVorbisData *BeginOggVorbis(char *nameFile,MyOggVorbisData *toRet)
{
	if(!canOggVorbis) return 0;
	FILE *f=fopen(nameFile,"rb");
	if(f==0) return 0;
	if(toRet==0)
	{
		toRet=new MyOggVorbisData;
	}
	ov_callbacks	sCallbacks;
	sCallbacks.read_func = ov_read_func;
	sCallbacks.seek_func = ov_seek_func;
	sCallbacks.close_func = ov_close_func;
	sCallbacks.tell_func = ov_tell_func;
	if(fn_ov_open_callbacks(f, &toRet->ovf, NULL, 0, sCallbacks)!=0)
	{
		fclose(f);
		delete toRet;
		return 0;
	}
	vorbis_info	*psVorbisInfo = fn_ov_info(&toRet->ovf, -1);
	if(psVorbisInfo==0)
	{
		fclose(f);
		delete toRet;
		return 0;
	}
	ALenum format=0;
	switch(psVorbisInfo->channels)
	{
	case 1: format=AL_FORMAT_MONO16; break;
	case 2: format=AL_FORMAT_STEREO16; break;
	case 4: format=alGetEnumValue("AL_FORMAT_QUAD16"); break;
	case 6: format = alGetEnumValue("AL_FORMAT_51CHN16"); break;
	}
	if(format==0)
	{
		fclose(f);
		delete toRet;
		return 0;
	}
	//buffer of 0.5 sec (too much?)
	toRet->bSize = psVorbisInfo->rate * psVorbisInfo->channels / 2;
	if(toRet->data) delete [] toRet->data;
	toRet->data=new char[toRet->bSize];
	if(toRet->alSource==0)
	{
		alGenBuffers(N_BUFFER,toRet->alBuffer);
		alGenSources(1,&toRet->alSource);
	}
	toRet->ended=false;
	toRet->format=format;
	toRet->freq=psVorbisInfo->rate;
	toRet->play=false;
	CreateThread(0,0,PlayOggVorbis,toRet,0,0);
	return toRet;
}

void EndOggVorbis(MyOggVorbisData *dest)
{
	dest->ended=true;
	Sleep(200);
	alSourceStop(dest->alSource);
	alSourcei(dest->alSource, AL_BUFFER, 0);
 	alDeleteBuffers(N_BUFFER,dest->alBuffer);
	alDeleteSources(1,&dest->alSource);
	delete [] dest->data;
}
