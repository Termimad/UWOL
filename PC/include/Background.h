#pragma once

#include "GameData.h"
#include "Graphics.h"
#include "TextureMgr.h"

class Background
{
public:
	Background(void);
	Background(TilesFondo tileFondo, 
			   int tilesX, 
			   int tilesY);
	~Background(void);
	void setTileFondo(TilesFondo tileFondo);
	void setSize(int width, int height);
	void setTileCount(int tilesX, int tilesY);
	void Draw();

private:
	Graphics* _g;

	vector<Frame> _frames;
	Frame _frame;

	TilesFondo _tileFondo;
	int _width;
	int _height;
	int _tilesX;
	int _tilesY;

	void InitBackground(TilesFondo tileFondo, 
			   int tilesX, 
			   int tilesY);
};
