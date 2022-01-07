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
    void calcBombsAroundTile(QPushButton* button);
    void resetGame();
    void gameOver(QPushButton*);
public slots:
    void tileButtonPress();
    void tileButtonClick();
    void tileRightClick();
    void resetButtonClick();
private:
    Ui::MinesweeperClass ui;
    std::array<std::array<Tile,16>,16> tiles;
    bool gameActive{ false };
    std::map<QPushButton*, std::array<int,2>> buttonCoords;
};
