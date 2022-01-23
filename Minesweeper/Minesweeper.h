#pragma once

#include <QtWidgets/QMainWindow>
#include<QPushButton>
#include "ui_Minesweeper.h"
#include "Tile.h"
#include<array>

class Minesweeper : public QMainWindow
{
    Q_OBJECT

public:
    Minesweeper(QWidget *parent = Q_NULLPTR);
    bool eventFilter(QObject* watched, QEvent* event);
    void changeColor(QWidget* mbutton, const QString color);
    void startGame(QPushButton*, int);
    void revealTile(QPushButton* button);
    void resetGame();
    void gameOver(QPushButton*);
    void checkIfWin();
    void disableButtons();
    int calcFlagsAroundTile(QPushButton* button);
    void setupTileBombCounts();
public slots:
    void tileButtonPress();
    void tileButtonClick();
    void tileRightClick();
    void tileMiddleClick();
    void resetButtonClick();
private:
    Ui::MinesweeperClass ui;
    int bombs;
    int bombStartAmount{ 30 };
    std::array<std::array<Tile,16>,16> tiles;
    bool gameActive{ false };
    std::map<QPushButton*, std::array<int,2>> buttonCoords;
    std::map<std::array<int, 2>, bool> bombCoordsUsed;
};
