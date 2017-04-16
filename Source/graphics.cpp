#include "graphics.h"


Graphics::Graphics()
{
	direct3d = NULL;
	device3d = NULL;
	fullscreen = false;
	width = GAME_WIDTH;    
	height = GAME_HEIGHT;
}


Graphics::~Graphics()
{
	releaseAll();
}

void Graphics::setColor(int r, int g, int b)
{
	backColor = SETCOLOR_ARGB(255, r, g, b);
	
}

void Graphics::releaseAll()
{
	SAFE_RELEASE(sprite);
	SAFE_RELEASE(device3d);
	SAFE_RELEASE(direct3d);
}

void Graphics::initialize(HWND hw, int w, int h, bool full)
{
	hwnd = hw;
	width = w;
	height = h;
	fullscreen = full;

	direct3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (direct3d == NULL)
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Direct3D"));

	initD3Dpp();       

	if (fullscreen){
		if (isAdapterCompatible())
			d3dpp.FullScreen_RefreshRateInHz = pMode.RefreshRate;
		else
			throw(GameError(gameErrorNS::FATAL_ERROR, "The graphics device does not support the specified resolution or format."));
	}

	D3DCAPS9 caps;
	DWORD behavior;
	result = direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 || caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
		behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING; 
	else
		behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;  

	result = direct3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, behavior, &d3dpp, &device3d);

	if (FAILED(result))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D device"));

	result = D3DXCreateSprite(device3d, &sprite);
	if (FAILED(result))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D sprite"));

	device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}


void Graphics::initD3Dpp()
{
	try{
		ZeroMemory(&d3dpp, sizeof(d3dpp));             
		
		d3dpp.BackBufferWidth = width;
		d3dpp.BackBufferHeight = height;
		if (fullscreen)
			d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;  
		else
			d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;  
		d3dpp.BackBufferCount = 1;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = hwnd;
		d3dpp.Windowed = (!fullscreen);
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	catch (...)
	{
		throw(GameError(gameErrorNS::FATAL_ERROR,
			"Error initializing D3D presentation parameters"));

	}
}

bool Graphics::isAdapterCompatible()
{
	UINT modes = direct3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, d3dpp.BackBufferFormat);

	for (UINT i = 0; i < modes; i++)
	{
		result = direct3d->EnumAdapterModes(D3DADAPTER_DEFAULT, d3dpp.BackBufferFormat, i, &pMode);

		if (pMode.Height == d3dpp.BackBufferHeight && pMode.Width == d3dpp.BackBufferWidth && pMode.RefreshRate >= d3dpp.FullScreen_RefreshRateInHz)
		{
			return true;
		}

	}
	return false;
}

HRESULT Graphics::showBackbuffer()
{
	result = E_FAIL;
	result = device3d->Present(NULL, NULL, NULL, NULL);
	return result;
}

HRESULT Graphics::beginScene()
{
	result = E_FAIL;
	if (device3d == NULL)
		return result;
	device3d->Clear(0, NULL, D3DCLEAR_TARGET, backColor, 1.0F, 0);
	result = device3d->BeginScene();
	return result;
}

HRESULT Graphics::endScene()
{
	result = E_FAIL;
	if (device3d)
		result = device3d->EndScene();
	return result;
}

HRESULT Graphics::getDeviceState()
{
	result = E_FAIL;
	if (device3d == NULL)
		return result;
	result = device3d->TestCooperativeLevel();
	return result;
}

HRESULT Graphics::reset()
{
	result = E_FAIL;
	initD3Dpp();
	sprite->OnLostDevice();
	result = device3d->Reset(&d3dpp);
	device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	sprite->OnResetDevice();
		return result;
}

HRESULT Graphics::loadTexture(const char* filename, COLOR_ARGB transcolor, UINT &width, UINT &height, LP_TEXTURE &texture)
{
	D3DXIMAGE_INFO info;
	result = E_FAIL;
	try{
		if (filename == NULL)
		{
			texture = NULL;
			return D3DERR_INVALIDCALL;
		}

		result = D3DXGetImageInfoFromFile(filename, &info);

			if (result != D3D_OK)
				return result;
			width = info.Width;
			height = info.Height;
			
			result = D3DXCreateTextureFromFileEx(device3d, filename, info.Width, info.Height, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, transcolor, &info, NULL, &texture);

	}
	catch (...)
	{
		throw (GameError(gameErrorNS::FATAL_ERROR, "Error in Graphics::loadTexture"));
	}
	return result;
}

void Graphics::drawSprite(const SpriteData &spriteData, COLOR_ARGB color)
{
	if (spriteData.texture == NULL)
		return;

	D3DXVECTOR2 spriteCenter = D3DXVECTOR2((float)(spriteData.width / 2 * spriteData.scale), (float)(spriteData.height / 2 * spriteData.scale));
	D3DXVECTOR2 translate = D3DXVECTOR2((float)spriteData.x, (float)spriteData.y);
	D3DXVECTOR2 scaling(spriteData.scale, spriteData.scale);

	if (spriteData.flipHorizontal)
	{
		scaling.x *= -1;
		spriteCenter.x -= (float)(spriteData.width*spriteData.scale);
		translate.x += (float)(spriteData.width*spriteData.scale);
	}

	if (spriteData.flipVertical)
	{
		scaling.y *= -1;
		spriteCenter.y -= (float)(spriteData.height*spriteData.scale);
		translate.y += (float)(spriteData.height*spriteData.scale);
	}

	D3DXMATRIX matrix;
	D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, &scaling, &spriteCenter, (float)(spriteData.angle), &translate);
	sprite->SetTransform(&matrix);
	sprite->Draw(spriteData.texture, &spriteData.rect, NULL, NULL, color);
}

void Graphics::spriteBegin()
{
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
}

void Graphics::spriteEnd()
{
	sprite->End();
}

void Graphics::changeDisplayMode(graphicsNS::DISPLAY_MODE mode)
{
	try{
		switch (mode)
		{
		case graphicsNS::FULLSCREEN:
			if (fullscreen)      
				return;
			fullscreen = true; break;
		case graphicsNS::WINDOW:
			if (fullscreen == false) 
				return;
			fullscreen = false; break;
		default:        
			fullscreen = !fullscreen;
		}
		reset();
		if (fullscreen)  
		{
			SetWindowLong(hwnd, GWL_STYLE, WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
		}
		else           
		{
			SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP, 0, 0, GAME_WIDTH, GAME_HEIGHT, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

			RECT clientRect;
			GetClientRect(hwnd, &clientRect);   
			MoveWindow(hwnd, 0, 0, GAME_WIDTH + (GAME_WIDTH - clientRect.right), GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom), TRUE);                                       
		}
	}
	catch (...)
	{
		SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, GAME_WIDTH, GAME_HEIGHT, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

		RECT clientRect;
		GetClientRect(hwnd, &clientRect);   
		MoveWindow(hwnd, 0, 0, GAME_WIDTH + (GAME_WIDTH - clientRect.right), GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom), TRUE);                                       
	}
}

HRESULT Graphics::loadTextureSystemMem(const char *filename, COLOR_ARGB transcolor, UINT &width, UINT &height, LP_TEXTURE &texture)
{
	D3DXIMAGE_INFO info;
	result = E_FAIL;
	try{
		if (filename == NULL)
		{
			texture = NULL;
			return D3DERR_INVALIDCALL;
		}
		result = D3DXGetImageInfoFromFile(filename, &info);
		if (result != D3D_OK)
			return result;
		result = D3DXCreateTextureFromFileEx(device3d, filename, info.Width, info.Height, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, transcolor, &info, NULL, &texture);
	}
	catch (...)
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error in Graphics::loadTexture"));
	}
	return result;
}

bool Graphics::drawQuad(LP_VERTEXBUFFER vertexBuffer)
{
	HRESULT result = E_FAIL;
	if (vertexBuffer == NULL)
		return false;

	device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	device3d->SetStreamSource(0, vertexBuffer, 0, sizeof(VertexC));
	device3d->SetFVF(D3DFVF_VERTEX);
	result = device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	if (FAILED(result))
		return false;
	return true;
}

HRESULT Graphics::createVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER &vertexBuffer)
{
	HRESULT result = E_FAIL;
	result = device3d->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX, D3DPOOL_DEFAULT, &vertexBuffer, NULL);
	if (FAILED(result))
		return result;
	void *ptr;
	result = vertexBuffer->Lock(0, size, (void**)&ptr, 0);
	if (FAILED(result))
		return result;
	memcpy(ptr, verts, size);
	vertexBuffer->Unlock();
	return result;
}
