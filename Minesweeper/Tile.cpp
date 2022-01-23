#include "Tile.h"
#include <qDebug>

void Tile::changeFlag(){
	flagged = !flagged;
}

void Tile::changeHidden() {
	hidden = !hidden;
}


Tile::Tile(bool hidden, QPushButton* button, TILE_TYPE tileType, int bombsAround)
{
	this->hidden = hidden;
	this->button = button;
	this->tileType = tileType;
	this->bombsAround = bombsAround;
	this->flagged = false;
}

Tile::Tile() {}

void Tile::calculateAndSetBombs(std::array<std::array<Tile, 16>, 16>& tiles, std::map<QPushButton*, std::array<int, 2>>& buttonCoords) {
    std::array<int, 2> coords = buttonCoords.at(button);
    int row = coords[0];
    int col = coords[1];
    int total = 0;
    for (int rowOffset = -1; rowOffset < 2; ++rowOffset) {
        for (int colOffset = -1; colOffset < 2; ++colOffset) {
            if ((rowOffset != 0) || (colOffset != 0)) { //avoid checking the square we're on
                int checkRow = row + rowOffset;
                int checkCol = col + colOffset;
                if (checkRow >= 0 && checkRow <  tiles.size()) {
                    if (checkCol >= 0 && checkCol < tiles.size()) {
                        Tile tile = tiles[checkRow][checkCol];
                        if (tile.tileType == TILE_TYPE::BOMB) {
                            ++total;
                        }
                    }
                }
            }
        }
    }
    bombsAround = total;
}

void Tile::reveal() {
    button->setText((bombsAround > 0) ? QString::number(bombsAround) : "");
    tileType = (bombsAround == 0) ? TILE_TYPE::SAFE : TILE_TYPE::BOMBS_AROUND;
    changeHidden();
}
