#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QString>
#include "management.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAddHotel_clicked();
    void on_btnViewHotels_clicked();
    void on_btnSortByPrice_clicked();
    void on_btnSortByRating_clicked();
    void on_btnSearchHotel_clicked();
    void on_btnKnapsack_clicked();
    void on_btnAddPath_clicked();
    void on_btnDijkstra_clicked();
    void on_btnSortByDistance_clicked();
    void on_btnFloydWarshall_clicked();
    void on_btnExit_clicked();

private:
    Ui::MainWindow *ui;
    HotelManager hotelManager;
    Graph graph;
};

#endif // MAINWINDOW_H
