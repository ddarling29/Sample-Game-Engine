#include "game.h"

Game::Game()
{
	input = new Input();       
	paused = false;           
	graphics = NULL;
	initialized = false;
	fpsOn = true;
	fps = 100;
}

Game::~Game()
{
	deleteAll();                
	ShowCursor(true);           
}

LRESULT Game::messageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (initialized)     
	{
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);        
			return 0;
		case WM_KEYDOWN: case WM_SYSKEYDOWN:   
			input->keyDown(wParam);
			return 0;
		case WM_KEYUP: case WM_SYSKEYUP:        
			input->keyUp(wParam);
			return 0;
		case WM_CHAR:                           
			input->keyIn(wParam);
			return 0;
		case WM_MOUSEMOVE:                     
			input->mouseIn(lParam);
			return 0;
		case WM_INPUT:                         
			input->mouseRawIn(lParam);
			return 0;
		case WM_LBUTTONDOWN:                    
			input->setMouseLButton(true);
			input->mouseIn(lParam);            
			return 0;
		case WM_LBUTTONUP:                      
			input->setMouseLButton(false);
			input->mouseIn(lParam);            
			return 0;
		case WM_MBUTTONDOWN:                    
			input->setMouseMButton(true);
			input->mouseIn(lParam);            
			return 0;
		case WM_MBUTTONUP:                      
			input->setMouseMButton(false);
			input->mouseIn(lParam);             
			return 0;
		case WM_RBUTTONDOWN:                    
			input->setMouseRButton(true);
			input->mouseIn(lParam);           
			return 0;
		case WM_RBUTTONUP:                      
			input->setMouseRButton(false);
			input->mouseIn(lParam);             
			return 0;
		case WM_XBUTTONDOWN: case WM_XBUTTONUP: 
			input->setMouseXButton(wParam);
			input->mouseIn(lParam);             
			return 0;
		case WM_DEVICECHANGE:                  
			input->checkControllers();
			return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);    
}


void Game::initialize(HWND hw)
{
	hwnd = hw;                                  

	graphics = new Graphics();

	graphics->initialize(hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

	input->initialize(hwnd, false);

	audio = new Audio();
	if (*WAVE_BANK != '\0' && SOUND_BANK != '\0')
	{
		if (FAILED(hr = audio->initialize()))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
				throw (GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system, media file not found."));
			else
				throw(GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system"));
		}
	}

	if (dxFont.initialize(graphics, gameNS::POINT_SIZE, false, false, gameNS::FONT) == false)
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing DirectX font."));
	dxFont.setFontColor(gameNS::FONT_COLOR);

	if (QueryPerformanceFrequency(&timerFreq) == false)
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing high resolution timer"));

	QueryPerformanceCounter(&timeStart);        

	initialized = true;
}

void Game::renderGame()
{
	const int BUF_SIZE = 20;
	static char buffer[BUF_SIZE];

	if (SUCCEEDED(graphics->beginScene()))
	{
		render();          
		graphics->spriteBegin();
		if (fpsOn)
		{
			_snprintf_s(buffer, BUF_SIZE, "fps %d ", (int)fps);
			dxFont.print(buffer, GAME_WIDTH - 100, GAME_HEIGHT - (GAME_HEIGHT - 25));
		}
		graphics->spriteEnd();
		graphics->endScene();
	}
	handleLostGraphicsDevice();

	graphics->showBackbuffer();
}

void Game::handleLostGraphicsDevice()
{
	hr = graphics->getDeviceState();
	if (FAILED(hr))                 
	{
		if (hr == D3DERR_DEVICELOST)
		{
			Sleep(100);             
			return;
		}
		
		else if (hr == D3DERR_DEVICENOTRESET)
		{
			releaseAll();
			hr = graphics->reset(); 
			if (FAILED(hr))          
				return;
			resetAll();
		}
		else
			return;                 
	}
}

void Game::setDisplayMode(graphicsNS::DISPLAY_MODE mode)
{
	releaseAll();                   
	graphics->changeDisplayMode(mode);
	resetAll();                     
}

void Game::run(HWND hwnd)
{
	if (graphics == NULL)           
		return;

	QueryPerformanceCounter(&timeEnd);
	frameTime = (float)(timeEnd.QuadPart - timeStart.QuadPart) / (float)timerFreq.QuadPart;

	if (frameTime < MIN_FRAME_TIME)
	{
		sleepTime = (DWORD)((MIN_FRAME_TIME - frameTime) * 1000);
		timeBeginPeriod(1);         
		Sleep(sleepTime);          
		timeEndPeriod(1);           
		return;
	}

	if (frameTime > 0.0)
		fps = (fps*0.99f) + (0.01f / frameTime);  
	if (frameTime > MAX_FRAME_TIME) 
		frameTime = MAX_FRAME_TIME; 
	timeStart = timeEnd;

	if (!paused)                    
	{
		update();                   
		ai();                       
		collisions();              
		input->vibrateControllers(frameTime); 
	}
	renderGame();                   
	input->readControllers();       
	audio->run();

	if (input->isKeyDown(ALT_KEY) && input->wasKeyPressed(ENTER_KEY))
		setDisplayMode(graphicsNS::TOGGLE);

	if (input->isKeyDown(ESC_KEY))
		setDisplayMode(graphicsNS::WINDOW); 

	input->clear(inputNS::KEYS_PRESSED);
}

void Game::releaseAll()
{

}

void Game::resetAll()
{

}

void Game::deleteAll()
{
	releaseAll();   
	SAFE_DELETE(graphics);
	SAFE_DELETE(input);
	SAFE_DELETE(audio);
	initialized = false;
}