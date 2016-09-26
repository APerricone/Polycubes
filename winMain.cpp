#include <windows.h>
#include <stdio.h>

#define WINMAIN_CALL
#include "systemInterface.h"
#undef WINMAIN_CALL

#include "game.h"

wchar_t keyboardInput[KB_QUEUE_LEN];
bool specialKey[KB_QUEUE_LEN];

HWND mainWindow;
LRESULT CALLBACK MainProc( HWND, UINT, WPARAM, LPARAM);

#define CLASSNAME "BO"
#define WINDOWSNAME "Policubes"

ALCdevice *alDevice;

HCURSOR arrowCursor,handCursor;
int APIENTRY WinMain(HINSTANCE	hInstance,HINSTANCE,LPSTR,int)
{
	srand(GetTickCount());
	//************
	//Init Windows
	//************
	arrowCursor=LoadCursor(NULL,IDC_ARROW);
	handCursor=LoadCursor(NULL,IDC_HAND);
	WNDCLASS MiaClasse={CS_DBLCLKS,MainProc,0,0,hInstance,NULL,
		arrowCursor,(HBRUSH)GetStockObject(BLACK_BRUSH),NULL,CLASSNAME};
	RegisterClass( &MiaClasse);
#ifdef _DEBUG
#define WINDOWSTILE WS_SYSMENU|WS_BORDER|WS_CAPTION
#else
#define WINDOWSTILE WS_POPUP
#endif
	RECT r; int cx,cy;
	r.right=GetSystemMetrics(SM_CXSCREEN);
	r.bottom=GetSystemMetrics(SM_CYSCREEN);
	cx=r.left=r.right/2;
	cy=r.top=r.bottom/2;
#ifdef _DEBUG
	if(r.bottom<=480)	Game::Get()->RenderDimY=300; else
	if(r.bottom<=600)	Game::Get()->RenderDimY=480; else
	if(r.bottom<=768)	Game::Get()->RenderDimY=600; else
						Game::Get()->RenderDimY=768;
	//Game::Get()->RenderDimY=300;
	Game::Get()->RenderDimX=Game::Get()->RenderDimY*16/10;
	Game::Get()->RenderDimX=Game::Get()->RenderDimY*4/3;
#else
	Game::Get()->RenderDimX=GetSystemMetrics(SM_CXSCREEN);
	Game::Get()->RenderDimY=GetSystemMetrics(SM_CYSCREEN);
#endif
	r.right=Game::Get()->RenderDimX;
	r.bottom=Game::Get()->RenderDimY;
	r.left-=r.right/2;
	r.right+=r.left;
	r.top-=r.bottom/2;
	r.bottom+=r.top;
	AdjustWindowRect(&r,WINDOWSTILE,FALSE);
	mainWindow=
		CreateWindow (CLASSNAME,WINDOWSNAME,WINDOWSTILE,
		r.left,r.top,r.right-r.left,r.bottom-r.top,NULL,NULL,hInstance,NULL);
#ifdef _DEBUG
	ShowWindow(mainWindow,SW_SHOW);
#else
	SetWindowPos(mainWindow,HWND_TOPMOST,0,0,0,0,
		SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
#endif
	UpdateWindow (mainWindow);
	//***********
	//Init OpenGL
	//***********
#ifndef NOGL
	HDC hDC=GetDC(mainWindow);
	PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)) ;
	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1 ;
	pfd.dwFlags    = PFD_DOUBLEBUFFER |
					PFD_SUPPORT_OPENGL |
					PFD_DRAW_TO_WINDOW ;
	pfd.cColorBits = 32;
	pfd.iPixelType = PFD_TYPE_RGBA ;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE ;
	int nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(nPixelFormat == 0)
	{
		ReleaseDC(mainWindow,hDC);
		message("ERROR","can't init OpenGL.(1)");
		return -1;
	}
	BOOL bResult = SetPixelFormat(hDC, nPixelFormat, &pfd);
	HGLRC m_hrc = wglCreateContext(hDC);
	if (!m_hrc)
	{
		ReleaseDC(mainWindow,hDC);
		message("ERROR","can't init OpenGL.(2)");
		return false;
	}
	wglMakeCurrent (hDC, m_hrc);
	//***************
	//* Init OpenAL *
	//***************
	Game::Get()->currDevice=-1;
	Game::Get()->radQuality=0;
	Game::Get()->musicVolume=85;
	Game::Get()->sfxVolume=85;
	Game::Get()->currLang=0;

	Game::Get()->games.Load();
	const char *devices= alcGetString(0,ALC_DEVICE_SPECIFIER);
	if(devices!=0)
	{
		if(Game::Get()->currDevice==-1) devices="Generic Software";  else
		for(unsigned char i=0;1<Game::Get()->currDevice;i++)
		{
			if(devices[0]==0) break;
			devices=devices+strlen(devices)+1;
		}
	}
	ALCcontext *context=0;
	alDevice=alcOpenDevice(devices); // open default device
	if(!alDevice) alDevice = alcOpenDevice(0);
	if(alDevice) context=alcCreateContext(alDevice,NULL); // create context
	if (context) alcMakeContextCurrent(context); // set active context*/
	sound=((alDevice!=0)&&(context!=0));
#endif
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	Game::Get()->nThread=si.dwNumberOfProcessors;
	if(Game::Get()->nThread>4)
		Game::Get()->nThread=4;
	Game::Get()->Init();
#ifdef NOGL
	return 1;
#endif
	//* Cycle
	char buff[100];
	for(buff[0]=0;buff[0]<NUM_BUTTON;buff[0]++)
		buttons[buff[0]]=false;
	DWORD att,prec=GetTickCount(),nF=0;
	float v=0,deltaTime,rest=0;
	MSG Messaggio;
	BOOL cont=TRUE;
	while (cont)
	{
		if(PeekMessage(&Messaggio,NULL,0,0,PM_NOREMOVE))
		{
			cont=GetMessage (&Messaggio,NULL,0,0);
			TranslateMessage (&Messaggio);
			DispatchMessage (&Messaggio);
		} else
		{
			att=GetTickCount();
			deltaTime=(att-prec)/1000.f;
			prec=att;
			if(GetFocus()!=NULL)
			{
				rest+=deltaTime;
				if(rest>0.05f) rest=1/60.f;
				while(rest>1/60.f)
				{
					cont=Game::Get()->Update();
					memset(keyboardInput,0,sizeof(wchar_t)*KB_QUEUE_LEN);
					if(!cont) break;
					rest-=1/60.f;
				}
				Game::Get()->Draw();
#ifndef NOGL
				SwapBuffers(hDC);
#endif
#ifdef _DEBUG
				nF++;
				v+=deltaTime;
				if(v>=1.f)
				{
					sprintf(buff,"Policubes dev - %5.2f fps", nF/v);
					v=0.f;
					nF=0;
					SetWindowText(mainWindow,buff);
				}
#endif
			} else Sleep(100);
		}
	}
	Game::DeInit();
	if(sound)
	{
		alcMakeContextCurrent(0); // set active context
		alcDestroyContext(context); // create context
		alcCloseDevice(alDevice);
	}
	return Messaggio.wParam;
}

bool hand=false;
void SetHandCursor(bool w)
{
	if(w)
		SetClassLong(mainWindow,GCL_HCURSOR,(LONG) handCursor); else
		SetClassLong(mainWindow,GCL_HCURSOR,(LONG) arrowCursor);
}

LRESULT CALLBACK MainProc ( HWND hWnd, UINT MSG, WPARAM wParam, LPARAM lParam)
{
	switch (MSG)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		{
			//POINT p;
			mouse.x=LOWORD(lParam);
			mouse.y=HIWORD(lParam);
			mouse.btn=(wParam & MK_LBUTTON)!=0;
			mouse.rbtn=(wParam & MK_RBUTTON)!=0;
		}
		break;
	case WM_CHAR:
		{
			if(GetAsyncKeyState(VK_CONTROL)&0x08000) break;
			for(char i=0;i<KB_QUEUE_LEN;i++)
				if(keyboardInput[i]==0)
				{
					keyboardInput[i]=(wchar_t)wParam;
					specialKey[i]=false;
					break;
				}
		}
		break;
	case WM_KEYDOWN:
		{
			if(EditBox::GetCurrentEdit())
			if(GetAsyncKeyState(VK_CONTROL)&0x08000)
			{
				if(wParam=='V')
				{
					if(OpenClipboard(NULL))
					{
						HANDLE hMem=GetClipboardData(CF_UNICODETEXT);
						if(hMem)
						{
							EditBox::GetCurrentEdit()->Paste((wchar_t*)GlobalLock(hMem));
							GlobalUnlock(hMem);
						}
						CloseClipboard();
					}
				}
			}
			if((lParam&0x01000000)!=0)
			{
				for(char i=0;i<KB_QUEUE_LEN;i++)
					if(keyboardInput[i]==0)
					{
						keyboardInput[i]=(wchar_t)wParam;
						specialKey[i]=true;
						break;
					}
			}
		}
	case WM_KEYUP:
		{
			char i;
			for(i=0;i<NUM_BUTTON;i++)
				if(wParam==keys[i])
				{
					buttons[i]=(MSG==WM_KEYDOWN);
					break;
				}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage (0);
		return 0;
	case WM_KILLFOCUS:
		ShowWindow(hWnd,SW_MINIMIZE);
		if(Game::Get()->match.music)
			Game::Get()->match.music->play=false;
		break;
	case WM_SETFOCUS:
		ShowWindow(hWnd,SW_RESTORE);
		if(Game::Get()->match.music)
			Game::Get()->match.music->play=true;
		break;
	}
	return DefWindowProc (hWnd,MSG,wParam,lParam);

}

bool IsShiftPress()
{
	return ((GetAsyncKeyState(VK_LSHIFT)&0x08000)||
			(GetAsyncKeyState(VK_RSHIFT)&0x08000));
}

void message(char* title,char* text, ... )
{
	char buff[1023];
	va_list marker;
	va_start(marker,text);
	vsprintf(buff,text,marker);
	va_end(marker);
	MessageBox(mainWindow,buff,title,MB_OK);
}

void OpenLink(char *url)
{
	mouse.last=true;
	ShellExecute(0,0,url,0,0,SW_SHOWNORMAL);
}
