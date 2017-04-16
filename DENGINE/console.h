#ifndef _CONSOLE_H              
#define _CONSOLE_H              
#define WIN32_LEAN_AND_MEAN

class Console;

#include <string>
#include <deque>
#include "constants.h"
#include "textDX.h"
#include "graphics.h"
#include "input.h"

namespace consoleNS
{
	const UINT WIDTH = 500;           
	const UINT HEIGHT = 400;          
	const UINT X = 5;                   
	const UINT Y = 5;
	const UINT MARGIN = 4;             
	const char FONT[] = "Courier New"; 
	const int FONT_HEIGHT = 14;         
	const COLOR_ARGB FONT_COLOR = graphicsNS::WHITE;
	const COLOR_ARGB BACK_COLOR = SETCOLOR_ARGB(192, 128, 128, 128); 
	const int MAX_LINES = 256;        
}


class Console
{
private:
	Graphics    *graphics;             
	Input       *input;               
	TextDX      dxFont;                
	float       x, y;                  
	int         rows;                   
	std::string commandStr;          
	std::string inputStr;            
	std::deque<std::string> text;    
	RECT        textRect;           
	COLOR_ARGB  fontColor;         
	COLOR_ARGB  backColor;          
	VertexC vtx[4];               
	LP_VERTEXBUFFER vertexBuffer; 
	int         scrollAmount;     
	bool        initialized;
	bool        visible;     

public:
	
	Console();

	virtual ~Console();

	bool initialize(Graphics *g, Input *in);

	const void draw();

	void showHide();

	bool getVisible() { return visible; }

	void show() { visible = true; }

	void hide() { visible = false; }

	void print(const std::string &str);

	std::string getCommand();

	std::string getInput() { return inputStr; }

	void clearInput()   { inputStr = ""; }

	void onLostDevice();

	void onResetDevice();
};

#endif