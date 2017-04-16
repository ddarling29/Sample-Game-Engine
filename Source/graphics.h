#ifndef _GRAPHICS_H             
#define _GRAPHICS_H            
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif
#include <d3d9.h>
#include <d3dx9.h>
#include "constants.h"
#include "gameError.h"

#define LP_3DDEVICE LPDIRECT3DDEVICE9
#define LP_3D LPDIRECT3D9
#define LP_SPRITE LPD3DXSPRITE
#define LP_TEXTURE LPDIRECT3DTEXTURE9
#define VECTOR2 D3DXVECTOR2
#define LP_VERTEXBUFFER LPDIRECT3DVERTEXBUFFER9
#define LP_DXFONT   LPD3DXFONT

#define COLOR_ARGB DWORD
#define SETCOLOR_ARGB(a,r,g,b) \
    ((COLOR_ARGB)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

namespace graphicsNS
{
	const COLOR_ARGB ORANGE = D3DCOLOR_ARGB(255, 255, 165, 0);
	const COLOR_ARGB BROWN = D3DCOLOR_ARGB(255, 139, 69, 19);
	const COLOR_ARGB LTGRAY = D3DCOLOR_ARGB(255, 192, 192, 192);
	const COLOR_ARGB GRAY = D3DCOLOR_ARGB(255, 128, 128, 128);
	const COLOR_ARGB OLIVE = D3DCOLOR_ARGB(255, 128, 128, 0);
	const COLOR_ARGB PURPLE = D3DCOLOR_ARGB(255, 128, 0, 128);
	const COLOR_ARGB MAROON = D3DCOLOR_ARGB(255, 128, 0, 0);
	const COLOR_ARGB TEAL = D3DCOLOR_ARGB(255, 0, 128, 128);
	const COLOR_ARGB GREEN = D3DCOLOR_ARGB(255, 0, 128, 0);
	const COLOR_ARGB NAVY = D3DCOLOR_ARGB(255, 0, 0, 128);
	const COLOR_ARGB WHITE = D3DCOLOR_ARGB(255, 255, 255, 255);
	const COLOR_ARGB YELLOW = D3DCOLOR_ARGB(255, 255, 255, 0);
	const COLOR_ARGB MAGENTA = D3DCOLOR_ARGB(255, 255, 0, 255);
	const COLOR_ARGB RED = D3DCOLOR_ARGB(255, 255, 0, 0);
	const COLOR_ARGB CYAN = D3DCOLOR_ARGB(255, 0, 255, 255);
	const COLOR_ARGB LIME = D3DCOLOR_ARGB(255, 0, 255, 0);
	const COLOR_ARGB BLUE = D3DCOLOR_ARGB(255, 0, 0, 255);
	const COLOR_ARGB BLACK = D3DCOLOR_ARGB(255, 0, 0, 0);
	const COLOR_ARGB FILTER = D3DCOLOR_ARGB(0, 0, 0, 0);  
	const COLOR_ARGB ALPHA25 = D3DCOLOR_ARGB(64, 255, 255, 255);  
	const COLOR_ARGB ALPHA50 = D3DCOLOR_ARGB(128, 255, 255, 255);  
	const COLOR_ARGB BACK_COLOR = NAVY;                       

	enum DISPLAY_MODE{ TOGGLE, FULLSCREEN, WINDOW };
}

struct VertexC              
{
	float x, y, z;          
	float rhw;              
	unsigned long color;    
};

#define D3DFVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct SpriteData
{
	int width;
	int height;
	float x;
	float y;
	float scale;
	float angle;
	RECT rect;
	LP_TEXTURE texture;
	bool flipHorizontal;
	bool flipVertical;
};

class Graphics
{
private:
	LP_3D direct3d;
	LP_3DDEVICE device3d;
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE pMode;
	LP_SPRITE sprite;

	HRESULT result;         
	HWND hwnd;
	bool fullscreen;
	int width;
	int height;

	void initD3Dpp();
	COLOR_ARGB backColor;

public:

	Graphics();

	virtual ~Graphics();

	void releaseAll();

	bool isAdapterCompatible();
	
	void initialize(HWND hw, int width, int height, bool fullscreen);

	HRESULT getDeviceState();

	HRESULT reset();

	HRESULT beginScene();

	HRESULT endScene();

	HRESULT showBackbuffer();

	void setColor(int r, int g, int b);

	HRESULT loadTexture(const char* filename, COLOR_ARGB transcolor, UINT &width, UINT &height, LP_TEXTURE &texture);

	HRESULT loadTextureSystemMem(const char* filename, COLOR_ARGB transcolor, UINT &width, UINT &height, LP_TEXTURE &texture);

	void drawSprite(const SpriteData &spriteData, COLOR_ARGB color);

	void spriteBegin();

	void spriteEnd();

	void    changeDisplayMode(graphicsNS::DISPLAY_MODE mode = graphicsNS::TOGGLE);

	LP_3D   get3D()             { return direct3d; }

	LP_3DDEVICE get3Ddevice()   { return device3d; }

	LP_SPRITE   getSprite()     { return sprite; }

	HDC     getDC()             { return GetDC(hwnd); }

	bool    getFullscreen()     { return fullscreen; }

	static float Vector2Length(const VECTOR2 *v) { return D3DXVec2Length(v);  }

	static float Vector2Dot(const VECTOR2 *v1, const VECTOR2 *v2) { return D3DXVec2Dot(v1, v2); }

	static void Vector2Normalize(VECTOR2 *v) { D3DXVec2Normalize(v, v); }

	static VECTOR2* Vector2Transform(VECTOR2 *v, D3DXMATRIX *m) { return D3DXVec2TransformCoord(v, v, m); }

	bool drawQuad(LP_VERTEXBUFFER vertexBuffer);

	HRESULT createVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER &vertexBuffer);

};

#endif