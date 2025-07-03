#ifndef MANAGEMENT_H
#define MANAGEMENT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <map>
#include <QString>
#include <QDebug>
#include <QCoreApplication>
using namespace std;

class Hotel{
private:
    int id;
    string name;
    int price;
    float rating;
    float distance;

public:
    Hotel()=default;
    Hotel(int i, string n, int p, float r, float d)
        : id(i), name(n), price(p), rating(r), distance(d) {}

    int getId() const { return id; }
    string getName() const { return name; }
    int getPrice() const { return price; }
    float getRating() const { return rating; }
    float getDistance() const { return distance; }

    void setId(int i) { id = i; }
    void setName(const string& n) { name = n; }
    void setPrice(int p) { price = p; }
    void setRating(float r) { rating = r; }
    void setDistance(float d) { distance = d; }

    void printDetail()const
    {
        string ID = "H" + to_string(id);
        cout<<ID<<" "<<name<<endl;
        cout<<"Price: "<<price<<endl;
        cout<<"Rating: "<<rating<<endl;
        cout<<"Distance: "<<distance<<endl;
    }

    QString getDetailString() const
    {
        QString ID = "H" + QString::number(id);
        return ID + " " + QString::fromStdString(name) + "\n" +
               "Price: $" + QString::number(price) + "\n" +
               "Rating: " + QString::number(rating) + "\n" +
               "Distance: " + QString::number(distance) + " km";
    }
};

class Graph {
public:
    map<string, map<string, float>> adjList;

    QString addPath(const string& rawFrom, const string& rawTo, float distance) {
        string from = rawFrom;
        string to = rawTo;

        // 去除首尾空格 + 全部转小写
        transform(from.begin(), from.end(), from.begin(), ::tolower);
        transform(to.begin(), to.end(), to.begin(), ::tolower);

        adjList[from][to] = distance;
        adjList[to][from] = distance;

        return QString("Path added: %1 <-> %2 (%3 km)")
            .arg(QString::fromStdString(from))
            .arg(QString::fromStdString(to))
            .arg(distance);
    }

    QString dijkstra(const string& rawStart, const string& rawEnd) {
        string start = rawStart, end = rawEnd;
        transform(start.begin(), start.end(), start.begin(), ::tolower);
        transform(end.begin(), end.end(), end.begin(), ::tolower);

        if (adjList.find(start) == adjList.end() || adjList.find(end) == adjList.end()) {
            return "The starting or ending city does not exist!";
        }

        map<string, float> dist;
        map<string, string> prev;
        vector<string> nodes;

        for (const auto& [city, _] : adjList) {
            dist[city] = (city == start) ? 0.0f : 1e9f;
            nodes.push_back(city);
        }

        while (!nodes.empty()) {
            // 选出当前未访问中距离最小的节点
            auto it = min_element(nodes.begin(), nodes.end(), [&](const string& a, const string& b) {
                return dist[a] < dist[b];
            });
            string curr = *it;
            nodes.erase(it);

            if (dist[curr] == 1e9f || curr == end) break;

            for (const auto& [neighbor, cost] : adjList[curr]) {
                float alt = dist[curr] + cost;
                if (alt < dist[neighbor]) {
                    dist[neighbor] = alt;
                    prev[neighbor] = curr;
                }
            }
        }

        if (dist[end] == 1e9f) {
            return QString("Unable to reach %2 from %1").arg(QString::fromStdString(start), QString::fromStdString(end));
        }

        // 回溯路径
        vector<string> path;
        for (string at = end; !at.empty(); at = prev.count(at) ? prev[at] : "") {
            path.push_back(at);
            if (at == start) break;
        }
        reverse(path.begin(), path.end());

        QString result;
        result += QString("Shortest distance: %1 km\nPath: ").arg(dist[end]);
        for (size_t i = 0; i < path.size(); ++i) {
            result += QString::fromStdString(path[i]);
            if (i < path.size() - 1) result += " -> ";
        }

        return result;
    }

    QString floydWarshall() {
        if (adjList.empty()) return "The figure is empty, and the Floyd-Warshall algorithm cannot be executed.";

        vector<string> nodes;
        map<string, int> idx;
        int i = 0;
        for (auto& pair : adjList) {
            string city = pair.first;
            transform(city.begin(), city.end(), city.begin(), ::tolower);
            nodes.push_back(city);
            idx[city] = i++;
        }

        int n = nodes.size();
        vector<vector<float>> dist(n, vector<float>(n, 1e9f));
        for (int i = 0; i < n; ++i) dist[i][i] = 0;

        for (auto& [from, neighbors] : adjList) {
            string f = from;
            transform(f.begin(), f.end(), f.begin(), ::tolower);
            for (auto& [to, d] : neighbors) {
                string t = to;
                transform(t.begin(), t.end(), t.begin(), ::tolower);
                dist[idx[f]][idx[t]] = d;
            }
        }

        for (int k = 0; k < n; ++k)
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    if (dist[i][k] + dist[k][j] < dist[i][j])
                        dist[i][j] = dist[i][k] + dist[k][j];

        QString output = "=== Shortest paths between all cities (unit: km) ===\n\n\t";
        for (const auto& name : nodes) output += QString::fromStdString(name) + "\t";
        output += "\n";

        for (int i = 0; i < n; ++i) {
            output += QString::fromStdString(nodes[i]) + ":\t";
            for (int j = 0; j < n; ++j) {
                output += (dist[i][j] >= 1e9f ? "INF" : QString::number(dist[i][j])) + "\t";
            }
            output += "\n";
        }

        return output;
    }

};

class HotelManager {
private:
    vector<Hotel> hotels;
public:
    HotelManager() {readFromFile();}

    void readFromFile()
    {


            QString path = QCoreApplication::applicationDirPath();
            qDebug() << "Current execution path:" << path;

            string filepath = path.toStdString() + "/information.txt";
            qDebug() << "Attempt to read file:" << QString::fromStdString(filepath);

            ifstream infile(filepath);
            if (!infile)
            {
                qDebug() << "Unable to open file";
                return;
            }

            hotels.clear();
            int id, price;
            string name;
            float rating, distance;

            while (infile >> id >> name >> price >> rating >> distance)
            {
                qDebug() << "Read hotel:" << QString::fromStdString(name);
                Hotel h(id, name, price, rating, distance);
                hotels.push_back(h);
            }

            infile.close();
            qDebug() << "Successfully read" << hotels.size() << "hotels";


    }



    void saveToFile(const Hotel& h)
    {
        QString path = QCoreApplication::applicationDirPath() + "/information.txt";
        ofstream outfile(path.toStdString(), ios::app);
        if (!outfile) {
            cout << "Failed to write to file at: " << path.toStdString() << endl;
            return;
        }
        outfile << h.getId() << " " << h.getName() << " "
                << h.getPrice() << " " << h.getRating() << " "
                << h.getDistance() << endl;
        outfile.close();
        cout << "Data saved to file: " << path.toStdString() << endl;
    }



    QString addHotel(const string& name, int price, float rating, float distance) {
        int maxId = 0;
        for (const auto& h : hotels) {
            if (h.getId() > maxId) maxId = h.getId();
        }

        Hotel newHotel(maxId + 1, name, price, rating, distance);
        hotels.push_back(newHotel);
        saveToFile(newHotel);

        return QString("Hotel added successfully with ID: H%1").arg(newHotel.getId());
    }

    void addHotel() {
        int maxId = 0;
        for (const auto& h : hotels) {
            if (h.getId() > maxId) maxId = h.getId();
        }

        Hotel newHotel;
        newHotel.setId(maxId + 1);

        string name;
        int price;
        float rating, distance;

        cout << "Enter hotel name: ";
        cin.ignore();
        getline(cin, name);
        newHotel.setName(name);

        cout << "Enter price: $";
        cin >> price;
        newHotel.setPrice(price);

        cout << "Enter rating (0.0-5.0): ";
        cin >> rating;
        newHotel.setRating(rating);

        cout << "Enter distance from city center (km): ";
        cin >> distance;
        newHotel.setDistance(distance);

        hotels.push_back(newHotel);
        saveToFile(newHotel);
        cout << "Hotel added successfully with ID: H" << newHotel.getId() << endl;
    }

    /*void viewAllHotels() {
        if (hotels.empty()) {
            cout << "No hotels available." << endl;
            return;
        }

        cout << "\n=== All Hotels ===" << endl;
        for (const auto& hotel : hotels) {
            hotel.printDetail();
        }
    }*/
    void viewAllHotels() {
        // 强制刷新，从文件中读取最新数据（防止程序运行中途文件被改动后看不到）
        readFromFile();

        if (hotels.empty()) {
            cout << "No hotels available." << endl;
            return;
        }

        cout << "\n=== All Hotels (2 rows × 5 columns) ===\n" << endl;

        const int columns = 5;
        for (size_t i = 0; i < hotels.size(); ++i) {
            const Hotel& h = hotels[i];

            cout << "H" << h.getId()
                 << " | " << h.getName()
                 << " | $" << h.getPrice()
                 << " | ⭐" << h.getRating()
                 << " | " << h.getDistance() << "km\t";

            if ((i + 1) % columns == 0)
                cout << endl;
        }

        if (hotels.size() % columns != 0)
            cout << endl;
    }


    vector<Hotel>& getHotels() { return hotels; }
};

#endif // MANAGEMENT_H
