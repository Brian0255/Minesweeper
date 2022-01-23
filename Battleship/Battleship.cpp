#include "Battleship.h"
#include<QPushButton>
#include<QRegularExpression>
#include<QMouseEvent>
#include"QRightClickButton.h"

const QString HIDDEN_TILE_COLOR{ "background-color: rgb(200,200,200);" };
const QString HIDDEN_HOVER_COLOR{ "background-color: rgb(220,220,220);" };
const QString HIDDEN_PRESS_COLOR{ "background-color: rgb(210,210,210);" };
const QString UNCOVERED_COLOR{ "background-color: rgb(170,170,170);" };
const QString HIT_SHIP_COLOR{ "background-color: rgb(220,0,0);" };
const QString HIT_PART_OF_SHIP_COLOR{ "background-color: rgb(255,139,160);" };
const int SHIP_START_AMOUNT{ 12 };
const int CLICK_START_AMOUNT{ 60 };
const int GRID_SIZE{ 10 };

Battleship::Battleship(QWidget* parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	srand(time(0));
	connect(ui.ResetButton, &QPushButton::clicked, this, &Battleship::resetButtonClick);

	QGridLayout* mainGrid = ui.mainGrid;
	for (int i = 0; i < GRID_SIZE; ++i) {
		for (int j = 0; j < GRID_SIZE; ++j) {
			QRightClickButton* tileBtn = new QRightClickButton(this);
			tileBtn->setStyleSheet(
				"border: 0px;"
				"background-color: rgb(200,200,200);"
				"border-radius: 2px;"
				"font: 18pt \"Segoe UI Semilight\";"
				"color: rgb(255, 255, 255); "
			);
			tileBtn->setFixedSize(48, 48);
			tileBtn->installEventFilter(this);
			Tile tile = Tile(tileBtn);
			connect(tileBtn, &QRightClickButton::leftClicked, this, &Battleship::tileButtonClick);
			connect(tileBtn, &QRightClickButton::rightClicked, this, &Battleship::tileRightClick);
			std::array<int, 2> coords = { i,j };
			tiles[i][j] = tile;
			buttonCoords.insert({ tileBtn, coords });
			mainGrid->addWidget(tileBtn, i, j);
		}
	}
	startGame();
}

bool Battleship::eventFilter(QObject* watched, QEvent* event) {
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

void Battleship::changeColor(QWidget* mbutton, const QString color) {
	mbutton->ensurePolished();
	QString sheet = mbutton->styleSheet();
	QRegularExpression re = QRegularExpression("background-color.*?;");
	sheet.replace(re, color);
	mbutton->setStyleSheet(sheet);

}

void Battleship::startGame() {
	ui.StatusLabel->setText("");
	shipsRemaining = SHIP_START_AMOUNT;
	clicksLeft = CLICK_START_AMOUNT;
	ui.ShipsLeftLabel->setText("Ships: " + QString::number(shipsRemaining));
	ui.ClicksLeftLabel->setText("Clicks left: " + QString::number(clicksLeft));
	int max = tiles.size();
	for (int size : shipLengths) {
		createShip("horizontal", size);
		createShip("horizontal", size);
		createShip("vertical", size);
		createShip("vertical", size);
	}
	setupTileShipCounts();
}

void Battleship::revealTile(QPushButton* button) {
	std::array<int, 2> coords = buttonCoords.at(button);
	int row = coords[0];
	int col = coords[1];
	tiles[row][col].reveal();
	changeColor(button, UNCOVERED_COLOR);
	if (tiles[row][col].isShip) {
		changeColor(button, HIT_PART_OF_SHIP_COLOR);
	}
	button->removeEventFilter(this);
	checkIfWin();
}

void Battleship::resetGame() {
	for (int i = 0; i < tiles.size(); ++i) {
		for (int j = 0; j < tiles.size(); ++j) {
			Tile* tile = &tiles[i][j];
			changeColor(tile->button, HIDDEN_TILE_COLOR);
			tile->button->installEventFilter(this);
			tile->button->setEnabled(true);
			tile->button->setText("");
			tile->hidden = true;
			tile->isShip = false;
		}
	}
	ships.clear();
	startGame();
}

void Battleship::gameOver(QPushButton* hit) {
	ui.StatusLabel->setText("Game over!");
	resetGame();
}

void Battleship::disableButtons() {
	for (std::array<Tile, GRID_SIZE> tileRow : tiles) {
		for (Tile tile : tileRow) {
			tile.button->setEnabled(false);
		}
	}
}

void Battleship::checkIfWin() {
	calculateShipsRemaining();
	if (shipsRemaining == 0) {
		disableButtons();
		ui.StatusLabel->setText("You win!");
	}
}

void Battleship::resetButtonClick() {
	resetGame();
}

void Battleship::tileRightClick() {
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	std::array<int, 2> coords = buttonCoords.at(button);
	Tile tile = tiles[coords[0]][coords[1]];
	if (tile.hidden&&button->isEnabled()) {
		button->setText((button->text() == "O") ? "" : "O");
	}
}

void Battleship::calculateShipsRemaining() {
	shipsRemaining = 0;
	for (Ship ship : ships) {
		shipsRemaining += (shipUncovered(ship)) ? 0 : 1;
	}
	ui.ShipsLeftLabel->setText("Ships remaining: " + QString::number(shipsRemaining));
}

bool Battleship::shipUncovered(Ship ship) {
	bool uncovered{ true };
	for (std::array coord : ship.coords) {
		int row = coord[0];
		int col = coord[1];
		if (tiles[row][col].hidden) {
			uncovered = false;
			break;
		}
	}
	if (uncovered) {
		for (std::array coord : ship.coords) {
			int row = coord[0];
			int col = coord[1];
			changeColor(tiles[row][col].button, HIT_SHIP_COLOR);
		}
	}
	return uncovered;
}

bool Battleship::checkIfShipFits(Ship ship) {
	bool fits{ true };
	for (std::array<int, 2> coord : ship.coords) {
		int row = coord[0];
		int col = coord[1];
		if (!tiles[row][col].hidden) {
			fits = false;
			break;
		}
	}
	return fits;
}

void Battleship::createShip(std::string orientation, int size) {
	if (orientation == "horizontal") {
		createHorizontalShip(size);
	}
	else if (orientation == "vertical") {
		createVerticalShip(size);
	}
}

void Battleship::createHorizontalShip(int size) {
	//horizontal will get built left to right
	int max = tiles.size();
	int limit = max - size + 1;
	bool valid{ false };
	Ship ship;
	while (!valid) {
		int row = rand() % limit;
		int col = rand() % max;
		Ship potentialShip;
		valid = true;
		for (int i = 0; i < size; ++i) {
			int newRow = row + i;
			int newCol = col;
			std::array<int, 2> coord = { newRow,newCol };
			potentialShip.coords.push_back(coord);
			if (tiles[newRow][newCol].isShip) {
				valid = false;
				break;
			}
		}
		if (valid) {
			char letter = 'A' + ships.size();
			ship = potentialShip;
			for (auto& coord : ship.coords) {
				int row = coord[0];
				int col = coord[1];
				tiles[row][col].isShip = true;
				tiles[row][col].letter = letter;
			}
		}
	}
	ships.push_back(ship);
}

void Battleship::createVerticalShip(int size) {
	//vertical will get built top to bottom
	int max = tiles.size();
	int limit = max - size + 1;
	bool valid{ false };
	Ship ship;
	while (!valid) {
		int row = rand() % max;
		int col = rand() % limit;
		valid = true;
		Ship potentialShip;
		for (int i = 0; i < size; ++i) {
			int newRow = row;
			int newCol = col+i;
			std::array<int, 2> coord = { newRow,newCol };
			potentialShip.coords.push_back(coord);
			if (tiles[newRow][newCol].isShip) {
				valid = false;
				break;
			}
		}
		if (valid) {
			char letter = 'A' + ships.size();
			ship = potentialShip;
			for (auto& coord : ship.coords) {
				int row = coord[0];
				int col = coord[1];
				tiles[row][col].isShip = true;
				tiles[row][col].letter = letter;
			}
		}
	}
	ships.push_back(ship);
}

void Battleship::tileButtonPress() {
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if (button->isEnabled())
		changeColor(button, HIDDEN_PRESS_COLOR);
}

void Battleship::tileButtonClick() {
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if (button->isEnabled()) {
		changeColor(button, UNCOVERED_COLOR);
		std::array<int, 2> coords = buttonCoords.at(button);
		Tile tile = tiles[coords[0]][coords[1]];
		--clicksLeft;
		ui.ClicksLeftLabel->setText("Clicks left: " + QString::number(clicksLeft));
		revealTile(button);
		checkIfWin();
	}
}


void Battleship::setupTileShipCounts() {
	int size = tiles.size();
	for (int i{ 0 }; i < size; ++i) {
		for (int j{ 0 }; j < size; ++j) {
			tiles[i][j].calculateShipsIntersecting(tiles, buttonCoords);
		}
	}
}
