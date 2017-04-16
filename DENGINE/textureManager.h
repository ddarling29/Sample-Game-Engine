#ifndef _TEXTUREMANAGER_H
#define _TEXTUREMANAGER_H
#define WIN32_LEAN_AND_MEAN

#include "graphics.h"
#include "constants.h"

class TextureManager
{
private:
	UINT       width;       
	UINT       height;     
	LP_TEXTURE texture;     
	const char *file;       
	Graphics *graphics;     
	bool    initialized;    
	HRESULT hr;

public:
	TextureManager();

	virtual ~TextureManager();

	LP_TEXTURE getTexture() const { return texture; }

	UINT getWidth() const { return width; }

	UINT getHeight() const { return height; }

	virtual bool initialize(Graphics *g, const char *file);

	virtual void onLostDevice();

	virtual void onResetDevice();
};
 
#endif