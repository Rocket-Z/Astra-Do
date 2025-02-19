#include "mainwindow.h"

// #include <QApplication>
// #include <QWidget>
// #include <QPushButton>
// #include <array>

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QGraphicsProxyWidget>


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow window;
    window.resize(800, 800);
    window.show();

    return a.exec();
}
