#pragma once
#include<QPushButton>

enum  TILE_TYPE{ HIDDEN,BOMB, SAFE, BOMBS_AROUND };

class Tile {
private:
	friend class Minesweeper;

	bool hidden;
	QPushButton* button;
	TILE_TYPE tileType;
	int bombsAround;
	bool flagged;
public:
	void changeFlag();
	void changeHidden();
	Tile(bool hidden, QPushButton* button, TILE_TYPE tileType, int bombsAround);
	Tile();
	void calculateAndSetBombs(std::array<std::array<Tile, 16>, 16>& tiles, std::map<QPushButton*, std::array<int, 2>>& buttonCoords);
	void reveal();
};