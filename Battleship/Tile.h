#pragma once
#include<QPushButton>

class Tile {
private:
	friend class Battleship;
	char letter;
	bool hidden;
	bool isShip;
	QPushButton* button;
	int shipsIntersecting;
public:
	void reveal();
	Tile(QPushButton* button);
	Tile();
	void calculateShipsIntersecting(std::array<std::array<Tile, 10>, 10>& tiles, std::map<QPushButton*, std::array<int, 2>>& buttonCoords);
	int calculateShipsNorth(std::map<QPushButton*, std::array<int, 2>>& buttonCoords, std::array<std::array<Tile, 10>, 10>& tiles);
	int calculateShipsSouth(std::map<QPushButton*, std::array<int, 2>>& buttonCoords, std::array<std::array<Tile, 10>, 10>& tiles);
	int calculateShipsEast(std::map<QPushButton*, std::array<int, 2>>& buttonCoords, std::array<std::array<Tile, 10>, 10>& tiles);
	int calculateShipsWest(std::map<QPushButton*, std::array<int, 2>>& buttonCoords, std::array<std::array<Tile, 10>, 10>& tiles);
};