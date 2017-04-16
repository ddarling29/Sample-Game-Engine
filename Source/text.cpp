#include "text.h"

Text::Text() : Image()
{
	file = NULL;                       
	graphics = NULL;                    
	color = graphicsNS::WHITE;          
	backColor = TRANSCOLOR;             
	align = textNS::LEFT;
	width = textNS::GRID_WIDTH - 3;    
	height = textNS::GRID_HEIGHT - 3;
	fontHeight = textNS::FONT_HEIGHT;
	spriteData.width = 0;
	spriteData.height = 0;
	spriteData.rect.bottom = 1;         
	spriteData.rect.right = 1;
	for (int row = 0; row<textNS::ROWS; row++)        
	{
		for (int col = 0; col<textNS::COLUMNS; col++) 
		{
			fontData[row][col].left = MAXINT;
			fontData[row][col].right = 0;
		}
	}
	proportional = false;
	proportionalSpacing = textNS::PROPORTIONAL_SPACING;
	underline = false;
	bold = false;
	tabSize = textNS::TAB_SIZE;
}

Text::~Text()
{
	Image::~Image();
}

bool Text::initialize(Graphics *g, const char *file)
{
	try {
		graphics = g;           

		UINT w, h;
		HRESULT result = graphics->loadTextureSystemMem(file, TRANSCOLOR, w, h, textureData);
		if (FAILED(result))
		{
			SAFE_RELEASE(textureData);
			return false;
		}

		D3DLOCKED_RECT rect;
		result = textureData->LockRect(0, &rect, NULL, D3DLOCK_READONLY);
		if (FAILED(result))                       
		{
			SAFE_RELEASE(textureData);
			return false;
		}

		for (DWORD row = 0; row<textNS::ROWS; row++)   
		{
			for (DWORD col = 0; col<textNS::COLUMNS; col++)    
			{
				fontData[row][col].left = MAXINT;    
				fontData[row][col].right = 0;

				for (DWORD y = row*textNS::GRID_HEIGHT + 1; y<(row + 1)*textNS::GRID_HEIGHT - 1; y++)
				{
	
					DWORD* pBits = (DWORD*)((BYTE*)rect.pBits + y*rect.Pitch);

					for (DWORD x = col*textNS::GRID_WIDTH + 1; x<(col + 1)*textNS::GRID_WIDTH - 1; x++)
					{

						DWORD dwPixel = pBits[x];

						if ((dwPixel & 0xff000000) != 0x00)
						{
							if (x < fontData[row][col].left)     
								fontData[row][col].left = x;    
							if (x > fontData[row][col].right)    
								fontData[row][col].right = x;   
						}
					}
				}
			}
		}

		textureData->UnlockRect(0);

		SAFE_RELEASE(textureData);

		if (!fontTexture.initialize(graphics, file))
			return false;                
		
		if (!Image::initialize(graphics, textNS::FONT_WIDTH, textNS::FONT_HEIGHT, 0, &fontTexture))
			return false;                   
	}
	catch (...)
	{
		return false;
	}
	return true;                 
}


void Text::setXY(int x2, int y2)
{
	if (x2 < 0 || y2 < 0)
		return;
	spriteData.x = (float)x2;
	spriteData.y = (float)y2;
}

void Text::print(const std::string &str)
{
	print(str, (int)spriteData.x, (int)spriteData.y);
}

void Text::print(const std::string &str, int x, int y, textNS::Alignment al)
{
	align = al;             
	print(str, x, y);
	align = textNS::LEFT;
}


void Text::print(const std::string &str, int x, int y)
{
	UCHAR ch = 0, chN = 0;
	std::string str2;
	width = textNS::FONT_WIDTH;
	int scaledWidth = static_cast<int>(textNS::FONT_WIDTH*spriteData.scale);
	float saveY = 0;
	int tabX = 0, tabW = 0;

	spriteData.x = (float)x;
	spriteData.y = (float)y;
	doAlign(str);

	for (UINT i = 0; i<str.length(); i++)
	{
		ch = str.at(i);
		if (ch > textNS::MIN_CHAR && ch <= textNS::MAX_CHAR)   
		{
			chN = ch - textNS::MIN_CHAR;               
			spriteData.rect.top = chN / textNS::COLUMNS * textNS::GRID_HEIGHT + 1;
			spriteData.rect.bottom = spriteData.rect.top + textNS::FONT_HEIGHT;
			if (proportional)
			{
				spriteData.rect.left = fontData[chN / textNS::COLUMNS][chN % textNS::COLUMNS].left;
				spriteData.rect.right = fontData[chN / textNS::COLUMNS][chN % textNS::COLUMNS].right + 1;
				width = spriteData.rect.right - spriteData.rect.left;
				if (width >= textNS::FONT_WIDTH)       
				{
					width = textNS::FONT_WIDTH;      
					spriteData.rect.left = chN % textNS::COLUMNS * textNS::GRID_WIDTH + 1;
					spriteData.rect.right = spriteData.rect.left + textNS::FONT_WIDTH;
				}
				else    
					width += proportionalSpacing;
				scaledWidth = static_cast<int>(width*spriteData.scale);
				drawChar(ch);
			}
			else    
			{
				width = textNS::FONT_WIDTH;
				spriteData.rect.left = chN % textNS::COLUMNS * textNS::GRID_WIDTH + 1;
				spriteData.rect.right = spriteData.rect.left + textNS::FONT_WIDTH;
				drawChar(ch);
			}
			spriteData.x += scaledWidth;
		}
		else    
		{
			switch (ch)
			{
			case ' ':                           
				if (proportional)
				{
					width = textNS::FONT_WIDTH / 2;
					scaledWidth = static_cast<int>(width*spriteData.scale);
				}
				drawChar(' ');
				spriteData.x += scaledWidth;
				break;
			case '\n':                            
				spriteData.x = (float)x;
				spriteData.y += static_cast<int>(height*spriteData.scale);
				saveY = spriteData.y;
				str2 = str.substr(i, str.length());
				doAlign(str2);
				spriteData.y = saveY;
				break;
			case '\r':                         
				spriteData.x = (float)x;
				str2 = str.substr(i, str.length());
				doAlign(str2);
				break;
			case '\t':                          
				width = textNS::FONT_WIDTH;
				scaledWidth = static_cast<int>(width*spriteData.scale);
				tabX = static_cast<int>(spriteData.x) / (scaledWidth * tabSize);
				tabX = (tabX + 1) * scaledWidth * tabSize;
				tabW = tabX - static_cast<int>(spriteData.x);
				while (tabW > 0)
				{
					if (tabW >= scaledWidth)
					{
						drawChar(' ');
						spriteData.x += scaledWidth;
					}
					else
					{
						width = tabW;       
						drawChar(' ');
						spriteData.x += tabW;
					}
					tabW -= scaledWidth;
				}
				break;
			case '\b':                          
				spriteData.x -= scaledWidth;
				if (spriteData.x < 0)
					spriteData.x = 0;
				break;
			case '\v':                           
				spriteData.y += static_cast<int>(height*spriteData.scale);
				break;
			case 0x01:                          
				spriteData.rect.top = 1;
				spriteData.rect.bottom = 1 + textNS::FONT_HEIGHT;
				spriteData.rect.left = 1;
				spriteData.rect.right = 1 + textNS::FONT_WIDTH;
				draw(spriteData);
				spriteData.x += scaledWidth;
				break;
			}
		}
	}
	return;
}


void Text::doAlign(const std::string &str)
{
	if (spriteData.texture == NULL)  
		return;

	UINT w, h;
	switch (align) {
	case textNS::CENTER:        
		getWidthHeight(str, w, h);
		spriteData.x -= w / 2;
		break;
	case textNS::RIGHT:             
		getWidthHeight(str, w, h);
		spriteData.x -= w;
		break;
	case textNS::CENTER_MIDDLE:     
		getWidthHeight(str, w, h);
		spriteData.x -= w / 2;
		spriteData.y -= h / 2;
		break;
	case textNS::CENTER_BOTTOM:   
		getWidthHeight(str, w, h);
		spriteData.x -= w / 2;
		spriteData.y -= h;
		break;
	case textNS::LEFT_BOTTOM:      
		getWidthHeight(str, w, h);
		spriteData.y -= h;
		break;
	case textNS::RIGHT_BOTTOM:      
		getWidthHeight(str, w, h);
		spriteData.x -= w;
		spriteData.y -= h;
		break;
	}
}


void Text::getWidthHeight(const std::string &str, UINT &w, UINT &h)
{
	if (spriteData.texture == NULL)        
		return;

	UCHAR ch = 0, chN = 0;
	width = textNS::FONT_WIDTH;
	int scaledWidth = static_cast<int>(width*spriteData.scale);
	int strW = 0;
	h = 0;
	int stringWidth = 0;

	for (UINT i = 0; i<str.length(); i++)
	{
		ch = str.at(i);

		if (ch > textNS::MIN_CHAR && ch <= textNS::MAX_CHAR)
		{
			chN = ch - textNS::MIN_CHAR;    
			if (proportional)
			{
				spriteData.rect.left = fontData[chN / textNS::COLUMNS][chN % textNS::COLUMNS].left;
				
				spriteData.rect.right = fontData[chN / textNS::COLUMNS][chN % textNS::COLUMNS].right + 1;
				width = spriteData.rect.right - spriteData.rect.left + proportionalSpacing;
				scaledWidth = static_cast<int>(width*spriteData.scale);
			}
			else    
			{
				width = textNS::FONT_WIDTH;
				spriteData.rect.left = chN % textNS::COLUMNS * textNS::GRID_WIDTH + 1;
				spriteData.rect.right = spriteData.rect.left + textNS::FONT_WIDTH;
			}
			stringWidth += scaledWidth;
		}
		else    
		{
			switch (ch)
			{
			case ' ':  
				if (proportional)
				{
					width = (textNS::FONT_WIDTH) / 2;
					scaledWidth = static_cast<int>(width*spriteData.scale);
				}
				stringWidth += scaledWidth;
				break;
			case '\n': 
				if (strW == 0)
					strW = stringWidth;
				stringWidth = 0;
				h += static_cast<int>(height*spriteData.scale);
				break;
			case '\r': 
				if (strW == 0)
					strW = stringWidth;
				stringWidth = 0;
				break;
			case '\t':  
			{
				width = textNS::FONT_WIDTH;
				scaledWidth = static_cast<int>(width*spriteData.scale);
				int tabX = static_cast<int>(spriteData.x) / (scaledWidth * tabSize);
				tabX = (tabX + 1) * scaledWidth * tabSize;
				int tabW = tabX - static_cast<int>(spriteData.x);
				while (tabW > 0)
				{
					if (tabW >= scaledWidth)
						stringWidth += scaledWidth;
					else
					{
						width = tabW;
						stringWidth += tabW;
					}
					tabW -= scaledWidth;
				}
			}
				break;
			case '\b':     
				stringWidth -= scaledWidth;
				if (stringWidth < 0)
					stringWidth = 0;
				break;
			case 0x01:     
				stringWidth += scaledWidth;
				break;
			}
		}
	}
	if (strW == 0)
		strW = stringWidth;
	w = strW;
	return;
}

void Text::drawChar(UCHAR ch)
{
	SpriteData sd2 = spriteData;   

	
	if (backColor != TRANSCOLOR)    
	{
		spriteData.rect.top = (textNS::SOLID - textNS::MIN_CHAR) / textNS::COLUMNS * textNS::GRID_HEIGHT + 1;
		spriteData.rect.bottom = spriteData.rect.top + textNS::GRID_HEIGHT - 2;
		spriteData.rect.left = (textNS::SOLID - textNS::MIN_CHAR) % textNS::COLUMNS * textNS::GRID_WIDTH + 1;
		spriteData.rect.right = spriteData.rect.left + width;
		draw(backColor);        
		spriteData.rect = sd2.rect;     
	}


	if (underline)
	{
		spriteData.rect.top = (textNS::UNDERLINE - textNS::MIN_CHAR) / textNS::COLUMNS * textNS::GRID_HEIGHT + 1;
		spriteData.rect.bottom = spriteData.rect.top + textNS::GRID_HEIGHT - 2;
		spriteData.rect.left = (textNS::UNDERLINE - textNS::MIN_CHAR) % textNS::COLUMNS * textNS::GRID_WIDTH + 1;
		spriteData.rect.right = spriteData.rect.left + width;
		draw(color);
		spriteData.rect = sd2.rect;     
	}


	if (ch > textNS::MIN_CHAR && ch <= textNS::MAX_CHAR) 
	{
		draw(spriteData, color);
		if (bold)   
		{
			spriteData.x += textNS::BOLD_SIZE*spriteData.scale;
			draw(spriteData, color);
			spriteData.x = sd2.x;
		}
	}
}

void Text::onLostDevice()
{
	try{
		if (!initialized)
			return;
		fontTexture.onLostDevice();
	}
	catch (...){}
}


void Text::onResetDevice()
{
	try{
		if (!initialized)
			return;
		fontTexture.onResetDevice();
	}
	catch (...){}
}