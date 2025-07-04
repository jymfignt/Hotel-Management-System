#include "mainwindow.h"
#include "ui_mainWindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);  // Set UI elements
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAddHotel_clicked() {
    bool ok1, ok2, ok3, ok4;

    QString name = QInputDialog::getText(this, "Add Hotel", "Hotel name:", QLineEdit::Normal, "", &ok1);
    // Pop up a dialog box to get the hotel name
    if (!ok1 || name.isEmpty()) return;

    int price = QInputDialog::getInt(this, "Add Hotel", "Price ($):", 100, 1, 10000, 1, &ok2);
    // Get the price, range 0-100000
    if (!ok2) return;

    double rating = QInputDialog::getDouble(this, "Add Hotel", "Rating (0.0-5.0):", 4.0, 0.0, 5.0, 1, &ok3);
    // Get the rating, range 0.0-5.0, keep one decimal place
    if (!ok3) return;

    double distance = QInputDialog::getDouble(this, "Add Hotel", "Distance from city center (km):", 5.0, 0.1, 1000.0, 1, &ok4);
    if (!ok4) return;

    QString result = hotelManager.addHotel(name.toStdString(), price, rating, distance);
    QMessageBox::information(this, "Success", result);
}

void MainWindow::on_btnViewHotels_clicked() {
    const vector<Hotel>& hotels = hotelManager.getHotels();

    if (hotels.empty()) {
        QMessageBox::information(this, "Notice", "No hotel information available.");
        return;
    }

    QString info = "<html><body>";
    info += "<table border='0' cellspacing='10'>";

    const int columns = 5;
    int total = hotels.size();
    int rows = (total + columns - 1) / columns;  // 自动计算行数

    for (int row = 0; row < rows; ++row) {
        info += "<tr>";
        for (int col = 0; col < columns; ++col) {
            int index = row * columns + col;
            if (index < total) {
                const Hotel& h = hotels[index];
                info += "<td valign='top' style='padding:8px; border:1px solid #ddd; min-width:160px;'>"
                        "<b>" + QString::fromStdString(h.getName()) + "</b><br>"
                                                                "💲<b>Price:</b> $" + QString::number(h.getPrice()) + "<br>"
                                                          "⭐<b>Rating:</b> " + QString::number(h.getRating()) + "<br>"
                                                           "📍<b>Distance:</b> " + QString::number(h.getDistance()) + " km"
                                                             "</td>";
            } else {
                info += "<td></td>";
            }
        }
        info += "</tr>";
    }

    info += "</table></body></html>";

    QMessageBox msgBox;
    msgBox.setWindowTitle("Hotel List");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(info);
    msgBox.exec();
}


void MainWindow::on_btnSortByPrice_clicked() {
    vector<Hotel> hotels = hotelManager.getHotels();
    if (hotels.empty()) {
        QMessageBox::information(this, "Notice", "No hotel information");
        return;
    }

    // bubble sort
    for (size_t i = 0; i < hotels.size() - 1; i++) {
        for (size_t j = 0; j < hotels.size() - i - 1; j++) {
            if (hotels[j].getPrice() > hotels[j + 1].getPrice()) {
                swap(hotels[j], hotels[j + 1]);
            }
        }
    }

    QString info = "Sort by price (low to high):\n\n";
    for (const auto& h : hotels) {
        info += QString::fromStdString(h.getName()) + " - $" + QString::number(h.getPrice()) + "\n";
    }
    QMessageBox::information(this, "Sort by price", info);
}

void MainWindow::on_btnSortByRating_clicked() {
    vector<Hotel> hotels = hotelManager.getHotels();
    if (hotels.empty()) {
        QMessageBox::information(this, "Notice", "No hotel information");
        return;
    }

    function<void(int, int)> mergeSort;
    mergeSort = [&](int left, int right) {
        if (left >= right) return;
        int mid = (left + right) / 2;
        mergeSort(left, mid);
        mergeSort(mid + 1, right);

        vector<Hotel> temp;
        int i = left, j = mid + 1;
        while (i <= mid && j <= right) {
            if (hotels[i].getRating() >= hotels[j].getRating()) temp.push_back(hotels[i++]);
            else temp.push_back(hotels[j++]);
        }
        while (i <= mid) temp.push_back(hotels[i++]);
        while (j <= right) temp.push_back(hotels[j++]);
        for (int k = left; k <= right; ++k) hotels[k] = temp[k - left];
    };

    mergeSort(0, hotels.size() - 1);

    QString info = "Sort by rating (high to low):\n\n";
    for (const auto& h : hotels) {
        info += QString::fromStdString(h.getName()) + " - ⭐" + QString::number(h.getRating()) + "\n";
    }
    QMessageBox::information(this, "Sort by rating", info);
}

void MainWindow::on_btnSearchHotel_clicked() {
    vector<Hotel> hotels = hotelManager.getHotels();
    if (hotels.empty()) {
        QMessageBox::warning(this, "Search failed", "No hotel information");
        return;
    }

    bool ok;
    QString name = QInputDialog::getText(this, "Search Hotel", "Enter hotel name：", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    sort(hotels.begin(), hotels.end(), [](const Hotel& a, const Hotel& b) {
        return a.getName() < b.getName();
    });

    int left = 0, right = hotels.size() - 1, index = -1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (hotels[mid].getName() == name.toStdString()) {
            index = mid;
            break;
        } else if (hotels[mid].getName() < name.toStdString()) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    if (index != -1) {
        Hotel h = hotels[index];
        QString info = QString::fromStdString(h.getName()) +
                       "\nPrice: $" + QString::number(h.getPrice()) +
                       "\nRating: " + QString::number(h.getRating()) +
                       "\nDistance: " + QString::number(h.getDistance()) + "km";
        QMessageBox::information(this, "Search results", info);
    } else {
        QMessageBox::information(this, "Result", "The hotel was not found.");
    }
}

void MainWindow::on_btnKnapsack_clicked() {
    vector<Hotel> hotels = hotelManager.getHotels();
    if (hotels.empty()) {
        QMessageBox::information(this, "Notice", "No hotel information");
        return;
    }

    bool ok1, ok2;
    int budget = QInputDialog::getInt(this, "Enter budget", "budgeted amount ($):", 500, 0, 10000, 1, &ok1);
    float minRating = QInputDialog::getDouble(this, "Minimum rating", "Minimum rating for each hotel:", 4.0, 0, 5, 1, &ok2);
    if (!ok1 || !ok2) return;

    int n = hotels.size();
    vector<vector<float>> dp(n + 1, vector<float>(budget + 1, 0));
    // Dynamic programming -- dp[i][j] represents the maximum score that the first i hotels can get within budget j

    for (int i = 1; i <= n; i++) {
        for (int w = 1; w <= budget; w++) {
            if (hotels[i - 1].getPrice() <= w && hotels[i - 1].getRating() >= minRating) {
                dp[i][w] = max(dp[i - 1][w],
                               dp[i - 1][w - hotels[i - 1].getPrice()] + hotels[i - 1].getRating());
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    vector<bool> selected(n, false);
    int w = budget;
    for (int i = n; i > 0 && w > 0; i--) {
        if (dp[i][w] != dp[i-1][w]) {
            selected[i-1] = true;
            w -= hotels[i-1].getPrice();
        }
    }

    QString result;
    float totalRating = dp[n][budget];
    int totalCost = 0;
    result += "Recommended hotel combinations\n\n";
    result += "Overall rating：" + QString::number(totalRating) + "\n\n";
    result += "Selected hotel:\n";
    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            Hotel h = hotels[i];
            result += QString::fromStdString(h.getName()) + " - $" +
                      QString::number(h.getPrice()) + " - ⭐" +
                      QString::number(h.getRating()) + "\n";
            totalCost += h.getPrice();
        }
    }
    result += "\nTotal cost: $" + QString::number(totalCost);
    result += "\nRemaining budget: $" + QString::number(budget - totalCost);
    QMessageBox::information(this, "Recommendations", result);
}

void MainWindow::on_btnAddPath_clicked() {
    bool ok1, ok2, ok3;
    QString from = QInputDialog::getText(this, "Add path", "Starting City：", QLineEdit::Normal, "", &ok1);
    QString to = QInputDialog::getText(this, "Add path", "End City：", QLineEdit::Normal, "", &ok2);
    if (!ok1 || !ok2 || from.isEmpty() || to.isEmpty()) return;

    float dist = QInputDialog::getDouble(this, "Add path", "Distance between two cities (km)：", 10, 1, 10000, 1, &ok3);
    if (!ok3) return;

    QString result = graph.addPath(from.toStdString(), to.toStdString(), dist);
    QMessageBox::information(this, "Success", result);
}

void MainWindow::on_btnDijkstra_clicked() {
    if (graph.adjList.empty()) {
        QMessageBox::warning(this, "Notice", "Please add some path information first.");
        return;
    }

    bool ok1, ok2;
    QString from = QInputDialog::getText(this, "Shortest path", "Starting City：", QLineEdit::Normal, "", &ok1);
    QString to = QInputDialog::getText(this, "Shortest path", "End City：", QLineEdit::Normal, "", &ok2);
    if (!ok1 || !ok2 || from.isEmpty() || to.isEmpty()) return;

    QString result = graph.dijkstra(from.toStdString(), to.toStdString());
    QMessageBox::information(this, "Shortest path results", result);
}

void MainWindow::on_btnSortByDistance_clicked() {
    vector<Hotel> hotels = hotelManager.getHotels();
    if (hotels.empty()) {
        QMessageBox::information(this, "Notice", "No hotel information");
        return;
    }

    sort(hotels.begin(), hotels.end(), [](const Hotel& a, const Hotel& b) {
        return a.getDistance() < b.getDistance();
    });

    QString msg = "Sort results by distance from the city centre(from nearest to farthest):\n\n";
    for (auto& h : hotels) {
        msg += QString::fromStdString(h.getName()) + " - " + QString::number(h.getDistance()) + " km\n";
    }

    QMessageBox::information(this, "Sort by distance", msg);
}

void MainWindow::on_btnFloydWarshall_clicked() {
    if (graph.adjList.empty()) {
        QMessageBox::warning(this, "Notice", "Please add some path information first");
        return;
    }

    QString result = graph.floydWarshall();
    QMessageBox::information(this, "Shortest path from all sources", result);
}

void MainWindow::on_btnExit_clicked() {
    int ret = QMessageBox::question(this, "Confirm exit", "Are you sure you want to log out of the system?", QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        QApplication::quit();
    }
}
