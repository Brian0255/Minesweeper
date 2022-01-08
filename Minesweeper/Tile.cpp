#include "Tile.h"
#include <qDebug>

void Tile::flag(){
	flagged = !flagged;
}

bool Tile::isFlagged(){
	return flagged;
}

void Tile::setTileType(TILE_TYPE newType) {
	this->tileType = newType;
}

TILE_TYPE Tile::getTileType(){
	return tileType;
}

void Tile::setHidden() {
	hidden = !hidden;
}

bool Tile::isHidden() {
	return hidden;
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
	this->flagged = false;
}

Tile::Tile()
{
	qDebug() << "called";
}
