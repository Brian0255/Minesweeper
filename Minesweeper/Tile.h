#pragma once
#include<QPushButton>

enum  TILE_TYPE{ HIDDEN,BOMB, SAFE, BOMBS_AROUND };

class Tile {
private:
	bool hidden;
	QPushButton* button;
	TILE_TYPE tileType;
	int bombsAround;
	bool flagged;
public:
	void flag();
	bool isFlagged();
	bool isHidden();
	void setHidden();
	void setTileType(TILE_TYPE newType);
	TILE_TYPE getTileType();
	QPushButton* getButton();
	Tile(bool hidden, QPushButton* button, TILE_TYPE tileType, int bombsAround);
	Tile();
};