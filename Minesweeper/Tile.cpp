#include "Tile.h"
#include <qDebug>

void Tile::setTileType(TILE_TYPE newType) {
	this->tileType = newType;
}

TILE_TYPE Tile::getTileType()
{
	return tileType;
}

QPushButton* Tile::getButton(){
	return button;
}

Tile::Tile(bool hidden, QPushButton* button, TILE_TYPE tileType, int bombsAround)
{
	this->hidden = hidden;
	this->button = button;
	this->tileType = tileType;
	this->bombsAround = bombsAround;
}

Tile::Tile()
{
	qDebug() << "called";
}
