#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Initialize the Qt application with command-line arguments
    MainWindow w;
    w.show();

    return a.exec();
    // Enter the Qt event loop and wait for user interaction
}
