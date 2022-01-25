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
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    srand(time(0));
    bombs = bombStartAmount;
    ui.BombLabel->setText("Bombs: " + QString::number(bombs));
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
                    (tiles[row][col].tileType != TILE_TYPE::BOMB)) {
                    valid = true;
                    tiles[row][col].tileType = TILE_TYPE::BOMB;
                }
            }
        }
        ui.BombLabel->setText("Bombs: " + QString::number(bombs));
    }

    void Minesweeper::tileRightClick() {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        std::array<int, 2> coords = buttonCoords.at(button);
        Tile tile = tiles[coords[0]][coords[1]];
        if (tile.tileType != TILE_TYPE::SAFE && tile.tileType != TILE_TYPE::BOMBS_AROUND) {
            button->setText( (button->text() == "X") ? "" : "X");
            bombs += (button->text() == "X") ? -1 : 1;
            tiles[coords[0]][coords[1]].changeFlag();
            ui.BombLabel->setText("Bombs: " + QString::number(bombs));
        }
    }

    void Minesweeper::tileMiddleClick() {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        std::array<int, 2> coords = buttonCoords.at(button);
        int row = coords[0];
        int col = coords[1];
        if (tiles[row][col].tileType == TILE_TYPE::BOMBS_AROUND) {
            if (calcFlagsAroundTile(button) == button->text().toInt()) {
                for (int rowOffset = -1; rowOffset < 2; ++rowOffset) {
                    for (int colOffset = -1; colOffset < 2; ++colOffset) {
                        if ((rowOffset != 0) || (colOffset != 0)) {
                            int checkRow = row + rowOffset;
                            int checkCol = col + colOffset;
                            if (checkRow >= 0 && checkRow < tiles.size()) {
                                if (checkCol >= 0 && checkCol < tiles.size()) {
                                    Tile tile = tiles[checkRow][checkCol];
                                    if (tile.hidden && (!tile.flagged)) {
                                        revealTile(tiles[checkRow][checkCol].button);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        checkIfWin();
    }

    void Minesweeper::revealTile(QPushButton* button) {
        std::array<int, 2> coords = buttonCoords.at(button);
        int row = coords[0];
        int col = coords[1];
        Tile* mainTile = &tiles[row][col];
        if (mainTile->tileType == TILE_TYPE::BOMB) {
            gameOver(button);
        }
        else{    
            mainTile->reveal();
            changeColor(button, UNCOVERED_COLOR);
            button->removeEventFilter(this);
            if (mainTile->tileType == TILE_TYPE::SAFE) {
                for (int rowOffset = -1; rowOffset < 2; ++rowOffset) {
                    for (int colOffset = -1; colOffset < 2; ++colOffset) {
                        if ((rowOffset != 0) || (colOffset != 0)) {
                            int checkRow = row + rowOffset;
                            int checkCol = col + colOffset;
                            if (checkRow >= 0 && checkRow < tiles.size()) {
                                if (checkCol >= 0 && checkCol < tiles.size()) {
                                    Tile tile = tiles[checkRow][checkCol];
                                    if (tile.hidden) {
                                        revealTile(tile.button);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void Minesweeper::resetGame(){
        for (int i = 0; i < tiles.size(); ++i){
            for (int j = 0; j < tiles.size(); ++j) {
                Tile* tile = &tiles[i][j];
                tile->tileType = (TILE_TYPE::HIDDEN);
                changeColor(tile->button, HIDDEN_TILE_COLOR);
                tile->button->installEventFilter(this);
                tile->button->setEnabled(true);
                tile->button->setText("");
                if(!tile->hidden) {
                    tile->changeHidden();
                }
                if (tile->flagged) {
                    tile->changeFlag();
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
                if (tile.tileType == TILE_TYPE::BOMB && tile.button != hit) {
                    changeColor(tile.button, UNCOVERED_COLOR);
                    tile.button->setText("X");
                }
            }
        }
        ui.StatusLabel->setText("Game over!");
    }

    void Minesweeper::disableButtons() {
        for (std::array<Tile, 16> tileRow : tiles) {
            for (Tile tile : tileRow) {
                tile.button->setEnabled(false);
            }
        }
    }

    void Minesweeper::checkIfWin() {
        bool win{ true };
        for (std::array<Tile, 16> tileRow : tiles) {
            for (Tile tile : tileRow) {
                if (tile.tileType == TILE_TYPE::HIDDEN) {
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
                setupTileBombCounts();
            }
            std::array<int, 2> coords = buttonCoords.at(button);
            Tile tile = tiles[ coords[0] ][ coords[1] ];
            if (tile.tileType == TILE_TYPE::BOMB) {
                gameOver(button);
            }
            else {
                revealTile(button);
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
                            if (tile.flagged) {
                                ++total;
                            }
                        }
                    }
                }
            }
        }
        return total;
    }

    void Minesweeper::setupTileBombCounts() {
        for (auto& array : tiles) {
            for (Tile& tile : array) {
                tile.calculateAndSetBombs(tiles,buttonCoords);
            }
        }
    }
