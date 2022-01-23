#include "Battleship.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Battleship w;
    w.show();
    return a.exec();
}
