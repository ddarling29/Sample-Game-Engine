#include "console.h"

Console::Console()
{
	initialized = false;                
	graphics = NULL;
	visible = false;                 
	fontColor = consoleNS::FONT_COLOR;
	backColor = consoleNS::BACK_COLOR;

	x = consoleNS::X;                
	y = consoleNS::Y;

	textRect.bottom = consoleNS::Y + consoleNS::HEIGHT - consoleNS::MARGIN;
	textRect.left = consoleNS::X + consoleNS::MARGIN;
	textRect.right = consoleNS::X + consoleNS::WIDTH - consoleNS::MARGIN;
	textRect.top = consoleNS::Y + consoleNS::MARGIN;

	vertexBuffer = NULL;

	rows = 0;
	scrollAmount = 0;
}

Console::~Console()
{
	onLostDevice();
}

bool Console::initialize(Graphics *g, Input *in)
{
	try {
		graphics = g;                    
		input = in;

		vtx[0].x = x;
		vtx[0].y = y;
		vtx[0].z = 0.0f;
		vtx[0].rhw = 1.0f;
		vtx[0].color = backColor;

		vtx[1].x = x + consoleNS::WIDTH;
		vtx[1].y = y;
		vtx[1].z = 0.0f;
		vtx[1].rhw = 1.0f;
		vtx[1].color = backColor;

		vtx[2].x = x + consoleNS::WIDTH;
		vtx[2].y = y + consoleNS::HEIGHT;
		vtx[2].z = 0.0f;
		vtx[2].rhw = 1.0f;
		vtx[2].color = backColor;

		vtx[3].x = x;
		vtx[3].y = y + consoleNS::HEIGHT;
		vtx[3].z = 0.0f;
		vtx[3].rhw = 1.0f;
		vtx[3].color = backColor;

		graphics->createVertexBuffer(vtx, sizeof vtx, vertexBuffer);

		if (dxFont.initialize(graphics, consoleNS::FONT_HEIGHT, false,
			false, consoleNS::FONT) == false)
			return false;   
		dxFont.setFontColor(fontColor);

	}
	catch (...) {
		return false;
	}

	initialized = true;
	return true;
}

const void Console::draw()
{
	if (!visible || graphics == NULL || !initialized)
		return;
	graphics->drawQuad(vertexBuffer);
	if (text.size() == 0)
		return;
}

void Console::onLostDevice() { /*TODO*/ }