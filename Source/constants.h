#ifndef _CONSTANTS_H           
#define _CONSTANTS_H           
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#define SAFE_DELETE(ptr)       { if (ptr) { delete (ptr); (ptr)=NULL; } }

#define SAFE_RELEASE(ptr)      { if(ptr) { (ptr)->Release(); (ptr)=NULL; } }

#define SAFE_DELETE_ARRAY(ptr) { if(ptr) { delete (ptr); (ptr)=NULL; } }

#define SAFE_ON_RESET_DEVICE(ptr)   { if(ptr) { ptr->onResetDevice(); } }

#define SAFE_ON_LOST_DEVICE(ptr)    { if(ptr) { ptr->onLostDevice(); } }

#define TRANSCOLOR  SETCOLOR_ARGB(0,255,0,255)

const char NEBULA_IMAGE[] = "Pictures\\orion.jpg";
const char TEXTURES_IMAGE[] = "Pictures\\textures.png";

const char WAVE_BANK[] = "Audio\\Win\\Wave Bank.xwb";
const char SOUND_BANK[] = "Audio\\Win\\Sound Bank.xsb";
const char BEEP1[] = "beep1";
const char BEEP2[] = "beep2";
const char BEEP3[] = "beep3";
const char BEEP4[] = "beep4";
const char HIT[] = "hit";

const char CLASS_NAME[] = "WinMain";
const char GAME_TITLE[] = "DENGINE";
const bool FULLSCREEN = false;               
const UINT  GAME_WIDTH = 640;              
const UINT  GAME_HEIGHT = 480;               

const double PI = 3.14159265;
const float FRAME_RATE = 200.0f;               
const float MIN_FRAME_RATE = 10.0f;             
const float MIN_FRAME_TIME = 1.0f / FRAME_RATE;  
const float MAX_FRAME_TIME = 1.0f / MIN_FRAME_RATE; 
 
const UCHAR ESC_KEY = VK_ESCAPE;   
const UCHAR ALT_KEY = VK_MENU;    
const UCHAR ENTER_KEY = VK_RETURN;
const UCHAR CONSOLE_KEY = VK_OEM_3;

enum weapon {SHIP, PLANET, TORPEDO};
#endif