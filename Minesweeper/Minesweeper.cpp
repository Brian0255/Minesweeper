#include "Minesweeper.h"
#include<QPushButton>
#include<QRegularExpression>
#include<QMouseEvent>
#include "QRightClickButton.h"

const QString HIDDEN_TILE_COLOR{ "background-color: rgb(200,200,200);" };
const QString HIDDEN_HOVER_COLOR{ "background-color: rgb(220,220,220);" };
const QString HIDDEN_PRESS_COLOR{ "background-color: rgb(210,210,210);" };
const QString UNCOVERED_COLOR{ "background-color: rgb(170,170,170);" };
const QString HIT_BOMB_COLOR{ "background-color: rgb(255,0,0);" };

Minesweeper::Minesweeper(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    srand(time(0));
    bombs = bombStartAmount;
    connect(ui.ResetButton, &QPushButton::clicked, this, &Minesweeper::resetButtonClick);

    QGridLayout* mainGrid = ui.mainGrid;
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            QRightClickButton* tileBtn = new QRightClickButton(this);
            tileBtn->setStyleSheet(
                "border: 0px;"
                "background-color: rgb(200,200,200);"
                "border-radius: 2px;"
                "font: 14pt \"Segoe UI Semilight\";"
                "color: rgb(255, 255, 255); "
            );
            tileBtn->setFixedSize(32, 32);
            tileBtn->installEventFilter(this);
            Tile tile = Tile(true, tileBtn, TILE_TYPE::HIDDEN, 0);
            connect(tileBtn, &QRightClickButton::leftClicked, this, &Minesweeper::tileButtonClick);
            connect(tileBtn, &QRightClickButton::rightClicked, this, &Minesweeper::tileRightClick);
            connect(tileBtn, &QRightClickButton::middleClicked, this, &Minesweeper::tileMiddleClick);
            std::array<int,2> coords = { i,j };
            tiles[i][j] = tile;
            bombCoordsUsed.insert({ { i,j }, false });
            buttonCoords.insert({ tileBtn, coords });
            mainGrid->addWidget(tileBtn, i, j);
        }
    }
}

    bool Minesweeper::eventFilter(QObject * watched, QEvent * event)
    {
        //if (watched->parent() == ui.TileFrame) {
            QPushButton* btn = static_cast<QPushButton*>(watched);
            if (btn && btn->isEnabled()) {

                if (event->type() == QEvent::Enter) {
                    changeColor(btn, HIDDEN_HOVER_COLOR);
                }

                else if (event->type() == QEvent::Leave) {
                    changeColor(btn, HIDDEN_TILE_COLOR);
                }
            }

            return QMainWindow::eventFilter(watched, event);
        //}
    }

    void Minesweeper::changeColor(QWidget* mbutton, const QString color) {
        mbutton->ensurePolished();
        QString sheet = mbutton->styleSheet();
        QRegularExpression re = QRegularExpression("background-color.*?;");
        sheet.replace(re, color);
        mbutton->setStyleSheet(sheet);

    }

    void Minesweeper::startGame(QPushButton* firstBtnClicked, int numBombs){
        int max = tiles.size();
        std::array<int,2> coords = buttonCoords.at(firstBtnClicked);
        for (int i = 0; i < numBombs; ++i) {
            bool valid{ false };
            while (!valid) {
                int row = rand() % max;
                int col = rand() % max;
                if ((row != coords[0] || col != coords[1]) &&
                    (tiles[row][col].getTileType() != TILE_TYPE::BOMB)) {
                    valid = true;
                    tiles[row][col].setTileType(TILE_TYPE::BOMB);
                }
            }
        }
        ui.BombLabel->setText("Bombs: " + QString::number(bombs));
    }

    void Minesweeper::tileRightClick() {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        std::array<int, 2> coords = buttonCoords.at(button);
        Tile tile = tiles[coords[0]][coords[1]];
        if (tile.getTileType() != TILE_TYPE::SAFE && tile.getTileType() != TILE_TYPE::BOMBS_AROUND) {
            button->setText( (button->text() == "X") ? "" : "X");
            bombs += (button->text() == "X") ? -1 : 1;
            tiles[coords[0]][coords[1]].flag();
            ui.BombLabel->setText("Bombs: " + QString::number(bombs));
        }
    }

    void Minesweeper::tileMiddleClick() {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        std::array<int, 2> coords = buttonCoords.at(button);
        int row = coords[0];
        int col = coords[1];
        if (tiles[row][col].getTileType() == TILE_TYPE::BOMBS_AROUND) {
            if (calcFlagsAroundTile(button) == button->text().toInt()) {
                for (int rowOffset = -1; rowOffset < 2; ++rowOffset) {
                    for (int colOffset = -1; colOffset < 2; ++colOffset) {
                        if ((rowOffset != 0) || (colOffset != 0)) {
                            int checkRow = row + rowOffset;
                            int checkCol = col + colOffset;
                            if (checkRow >= 0 && checkRow < tiles.size()) {
                                if (checkCol >= 0 && checkCol < tiles.size()) {
                                    Tile tile = tiles[checkRow][checkCol];
                                    if (tile.isHidden() && (!tile.isFlagged())) {
                                        calcBombsAroundTile(tiles[checkRow][checkCol].getButton());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void Minesweeper::calcBombsAroundTile(QPushButton* button) {
        std::array<int, 2> coords = buttonCoords.at(button);
        int row = coords[0];
        int col = coords[1];
        Tile mainTile = tiles[row][col];
        if (mainTile.getTileType() == TILE_TYPE::BOMB) {
            gameOver(button);
        }
        else {
            int total = 0;
            for (int rowOffset = -1; rowOffset < 2; ++rowOffset) {
                for (int colOffset = -1; colOffset < 2; ++colOffset) {
                    if ((rowOffset != 0) || (colOffset != 0)) { //avoid checking the square we're on
                        int checkRow = row + rowOffset;
                        int checkCol = col + colOffset;
                        if (checkRow >= 0 && checkRow < tiles.size()) {
                            if (checkCol >= 0 && checkCol < tiles.size()) {
                                Tile tile = tiles[checkRow][checkCol];
                                if (tile.getTileType() == TILE_TYPE::BOMB) {
                                    ++total;
                                }
                            }
                        }
                    }
                }
            }
            button->setText((total > 0) ? QString::number(total) : "");

            if (total == 0) {
                tiles[row][col].setTileType(TILE_TYPE::SAFE);
                for (int rowOffset = -1; rowOffset < 2; ++rowOffset) {
                    for (int colOffset = -1; colOffset < 2; ++colOffset) {
                        if ((rowOffset != 0) || (colOffset != 0)) {
                            int checkRow = row + rowOffset;
                            int checkCol = col + colOffset;
                            if (checkRow >= 0 && checkRow < tiles.size()) {
                                if (checkCol >= 0 && checkCol < tiles.size()) {
                                    Tile tile = tiles[checkRow][checkCol];
                                    if (tile.getTileType() == TILE_TYPE::HIDDEN) {
                                        calcBombsAroundTile(tile.getButton());
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                tiles[row][col].setTileType(TILE_TYPE::BOMBS_AROUND);
            }
            changeColor(button, UNCOVERED_COLOR);
            tiles[row][col].setHidden();
            button->removeEventFilter(this);
        }
    }

    void Minesweeper::resetGame(){
        for (int i = 0; i < tiles.size(); ++i){
            for (int j = 0; j < tiles.size(); ++j) {
                tiles[i][j].setTileType(TILE_TYPE::HIDDEN);
                changeColor(tiles[i][j].getButton(), HIDDEN_TILE_COLOR);
                tiles[i][j].getButton()->setEnabled(true);
                tiles[i][j].getButton()->setText("");
                if(!tiles[i][j].isHidden()) {
                    tiles[i][j].setHidden();
                }
                if (tiles[i][j].isFlagged()) {
                    tiles[i][j].flag();
                }
                gameActive = false;
            }
        }
        ui.StatusLabel->setText("");
        bombs = bombStartAmount;
        ui.BombLabel->setText("Bombs: " + QString::number(bombs));
    }

    void Minesweeper::gameOver(QPushButton* hit) {
        hit->setText("X");
        changeColor(hit, HIT_BOMB_COLOR);
        disableButtons();
        for (std::array<Tile,16> tileRow : tiles) {
            for (Tile tile : tileRow) {
                if (tile.getTileType() == TILE_TYPE::BOMB && tile.getButton() != hit) {
                    changeColor(tile.getButton(), UNCOVERED_COLOR);
                    tile.getButton()->setText("X");
                }
            }
        }
        ui.StatusLabel->setText("Game over!");
    }

    void Minesweeper::disableButtons() {
        for (std::array<Tile, 16> tileRow : tiles) {
            for (Tile tile : tileRow) {
                tile.getButton()->setEnabled(false);
            }
        }
    }

    void Minesweeper::checkIfWin() {
        bool win{ true };
        for (std::array<Tile, 16> tileRow : tiles) {
            for (Tile tile : tileRow) {
                if (tile.getTileType() == TILE_TYPE::HIDDEN) {
                    win = false;
                }
            }
        }
        if (win) {
            disableButtons();
            ui.StatusLabel->setText("You win!");
        }
    }

    void Minesweeper::resetButtonClick(){
        resetGame();
    }

    void Minesweeper::tileButtonPress() {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if(button->isEnabled())
            changeColor(button, HIDDEN_PRESS_COLOR);
    }

    void Minesweeper::tileButtonClick() {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if (button->isEnabled()) {
            changeColor(button, UNCOVERED_COLOR);

            if (!gameActive) {
                gameActive = true;
                startGame(button, bombStartAmount);
            }
            std::array<int, 2> coords = buttonCoords.at(button);
            Tile tile = tiles[ coords[0] ][ coords[1] ];
            if (tile.getTileType() == TILE_TYPE::BOMB) {
                gameOver(button);
            }
            else {
                calcBombsAroundTile(button);
                checkIfWin();
            }
        }
    }

    int Minesweeper::calcFlagsAroundTile(QPushButton* button) {
        std::array<int, 2> coords = buttonCoords.at(button);
        int row = coords[0];
        int col = coords[1];
        Tile mainTile = tiles[row][col];
        int total = 0;
        for (int rowOffset = -1; rowOffset < 2; ++rowOffset) {
            for (int colOffset = -1; colOffset < 2; ++colOffset) {
                if ((rowOffset != 0) || (colOffset != 0)) { 
                    int checkRow = row + rowOffset;
                    int checkCol = col + colOffset;
                    if (checkRow >= 0 && checkRow < tiles.size()) {
                        if (checkCol >= 0 && checkCol < tiles.size()) {
                            Tile tile = tiles[checkRow][checkCol];
                            if (tile.isFlagged()) {
                                ++total;
                            }
                        }
                    }
                }
            }
        }
        return total;
    }