#include "Tile.h"
#include <qDebug>

Tile::Tile(QPushButton* button) 
    :hidden{ true }, isShip{ false }, shipsIntersecting{ 0 }, button{ button } {}

Tile::Tile() {}

void Tile::calculateShipsIntersecting(std::array<std::array<Tile, 10>, 10>& tiles, std::map<QPushButton*, std::array<int, 2>>& buttonCoords) {
    shipsIntersecting = 0;
    shipsIntersecting += calculateShipsNorth(buttonCoords, tiles);
    shipsIntersecting += calculateShipsSouth(buttonCoords, tiles);
    shipsIntersecting += calculateShipsEast(buttonCoords, tiles);
    shipsIntersecting += calculateShipsWest(buttonCoords, tiles);
}

int Tile::calculateShipsNorth(std::map<QPushButton*, std::array<int, 2>>& buttonCoords, std::array<std::array<Tile, 10>, 10>& tiles) {
    std::array<int, 2> coords = buttonCoords.at(button);
    int row = coords[0];
    int col = coords[1];
    int total{ 0 };
    for (int i{ row - 1 }; i >= 0; --i) {
        Tile tile = tiles[i][col];
        if (tile.isShip) {
            ++total;
        }
    }
    return total;
}

int Tile::calculateShipsSouth(std::map<QPushButton*, std::array<int, 2>>& buttonCoords, std::array<std::array<Tile, 10>, 10>& tiles) {
    std::array<int, 2> coords = buttonCoords.at(button);
    int row = coords[0];
    int col = coords[1];
    int total{ 0 };
    for (int i{ row + 1 }; i < tiles.size(); ++i) {
        Tile tile = tiles[i][col];
        if (tile.isShip) {
            ++total;
        }
    }
    return total;
}

int Tile::calculateShipsWest(std::map<QPushButton*, std::array<int, 2>>& buttonCoords, std::array<std::array<Tile, 10>, 10>& tiles) {
    std::array<int, 2> coords = buttonCoords.at(button);
    int row = coords[0];
    int col = coords[1];
    int total{ 0 };
    for (int i{ col - 1 }; i >= 0; --i) {
        Tile tile = tiles[row][i];
        if (tile.isShip) {
            ++total;
        }
    }
    return total;
}

int Tile::calculateShipsEast(std::map<QPushButton*, std::array<int, 2>>& buttonCoords, std::array<std::array<Tile, 10>, 10>& tiles) {
    std::array<int, 2> coords = buttonCoords.at(button);
    int row = coords[0];
    int col = coords[1];
    int total{ 0 };
    for (int i{ col + 1 }; i < tiles[0].size(); ++i) {
        Tile tile = tiles[row][i];
        if (tile.isShip) {
            ++total;
        }
    }
    return total;
}

void Tile::reveal() {
    if (isShip) {
        button->setText(QString(letter));
    }
    else {
        button->setText((shipsIntersecting > 0) ? QString::number(shipsIntersecting) : "");
    }
    hidden = false;
}
