#pragma once

#include <QtWidgets/QMainWindow>
#include<QPushButton>
#include "ui_Battleship.h"
#include "Tile.h"
#include"Ship.h"
#include<array>

class Battleship : public QMainWindow
{
    Q_OBJECT

public:
    Battleship(QWidget *parent = Q_NULLPTR);
    bool eventFilter(QObject* watched, QEvent* event);
    void changeColor(QWidget* mbutton, const QString color);
    void startGame();
    void revealTile(QPushButton* button);
    void resetGame();
    void gameOver(QPushButton*);
    void checkIfWin();
    void disableButtons();
    void setupTileShipCounts();
public slots:
    void tileButtonPress();
    void tileButtonClick();
    void resetButtonClick();
    void tileRightClick();
private:
    Ui::MinesweeperClass ui;
    int clicksLeft;
    int shipsRemaining;
    std::vector<Ship> ships;
    std::array<int, 3> shipLengths{ 4,3,2 };
    std::array<std::array<Tile,10>,10> tiles;
    bool gameActive{ false };
    std::map<QPushButton*, std::array<int,2>> buttonCoords;
    void calculateShipsRemaining();
    bool shipUncovered(Ship ship);
    bool checkIfShipFits(Ship ship);
    void createShip(std::string orientation, int size);
    void createHorizontalShip(int size);
    void createVerticalShip(int size);
};
