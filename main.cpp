#include <QCoreApplication>
#include <QMap>
#include <random>
#include <QVector>
#include <algorithm>
#include <QDebug>
#include <iostream>
#include <QSet>
#include <QFile>
#include "functions.h"
#include "graphsolving.h"
#include <QImage>

using namespace std;


void regularBenchmark();
void imageBenchmark();

int main(int argc, char *argv[])
{
    (void) argc, (void) argv;

    //regularBenchmark();
    imageBenchmark();

    return 0;
}

static constexpr int partW = 15;
static constexpr int partH = 15;

struct ImagePart {
    int x = 0;
    int y = 0;
    int w = partW;
    int h = partH;
    QVector<QVector<QRgb>> data;
    QVector<QVector<bool>> mask;

    ImagePart() {
        mask.resize(partH);

        for (QVector<bool>& v : mask) {
            v.resize(partW);
            v.fill(true);
        }
    }

    ImagePart united(const ImagePart &other) const{
        ImagePart p;
        QRect united = QRect(x,y,w,h).united(QRect(other.x, other.y, other.w, other.h));

        p.x = united.x();
        p.y = united.y();
        p.w = united.width();
        p.h = united.height();

        p.data.resize(p.h);
        p.mask.resize(p.h);

        for (QVector<bool> &v : p.mask) {
            v.resize(p.w);
            v.fill(true);
        }
        for (QVector<QRgb> &v: p.data) {
            v.resize(p.w);
        }

        for (int x = p.x; x < p.x + p.w; x++) {
            for (int y = p.y; y < p.y + p.h; y++) {
                if (y >= this->y && y < this->y + this->h && x >= this->x && x < this->x + this->w
                        && this->mask[y-this->y][x-this->x]) {
                    p.data[y-p.y][x-p.x] = this->data[y-this->y][x-this->x];
                } else if (y >= other.y && y < other.y + other.h && x >= other.x && x < other.x + other.w
                           && other.mask[y-other.y][x-other.x]) {
                    p.data[y-p.y][x-p.x] = other.data[y-other.y][x-other.x];
                } else {
                    p.mask[y-p.y][x-p.x] = false;
                }
            }
        }

        return p;
    }
};

int neuralimage(const QList<ImagePart> & parts, QList<QSet<int>> &results);
int neuralimage0(const QList<ImagePart> & __parts, QList<ImagePart> &results);

void imageBenchmark()
{
    QList<QImage> images;

    int n = 9;
    int s = 200;
    int image_width(0), image_height(0);

    QList<ImagePart> parts;

    for (int i = 0; i < n; i++) {
        images.push_back(QImage());
        images.back().load("images/" + QString::number(images.size()) + ".png");

        const QImage &image = images.back();

        image_width = max(image_width, image.width());
        image_height = max(image_height, image.height());

        uniform_int_distribution<> randx(0, image.width()-1 -partW);
        uniform_int_distribution<> randy(0, image.height()-1 -partH);

        for (int n = 0; n < s; n++) {
            ImagePart p;
            p.x = randx(randg());
            p.y = randy(randg());

            qDebug() << p.x << p.y;

            p.data.resize(partH);

            for (int i = 0; i < partH; i++) {
                p.data[i].resize(partW);
                for (int j = 0; j < partW; j++) {
                    p.data[i][j] = image.pixel(p.x+j, p.y+i);
                }
            }

            parts.push_back(p);
        }
    }

    std::shuffle(parts.begin(), parts.end(), randg());

    auto stuffPart = [](const ImagePart &p, QImage &image) {
        for (int i = 0; i < p.h; i++) {
            for (int j = 0; j < p.w; j++) {
                if (p.mask[i][j]) {
                    image.setPixel(p.x+j, p.y+i, p.data[i][j]);
                }
            }
        }
    };

//    QList<QSet<int>> results;
//    cout << neuralimage(parts, results) << endl;

//    for (int x = 0; x < results.length(); x++) {
//        auto set = results[x];
//        QImage test(image_width, image_height, QImage::Format_ARGB32);
//        test.fill(qRgba(0,0,0,255));

//        for (int i : set) {
//            stuffPart(parts[i], test);
//        }

//        test.save("test" + QString::number(x) + ".png");
//    }

    qDebug() << "Starting benchmark" << endl;

    QList<ImagePart> results;
    cout << neuralimage0(parts, results) << endl;

    for (int x = 0; x < results.length(); x++) {
        auto part = results[x];
        QImage test(image_width, image_height, QImage::Format_ARGB32);
        test.fill(qRgba(0,0,0,255));

        stuffPart(part, test);

        test.save("test" + QString::number(x) + ".png");
    }


    //qDebug() << results;
}

template int correl<>(const ImagePart &p1, const ImagePart &p2);

int correl(const ImagePart &p1, const ImagePart &p2){
    QRect intersection = QRect(p1.x, p1.y, p1.w, p1.h).intersected(QRect(p2.x, p2.y, p2.w, p2.h));

    if (!intersection.isValid()) {
        return 0;
    }

    if (p1.w  > partH || p2.w  > partH || p1.h > partH || p2.h > partH) {
        //qDebug() << "big";
    }

    int invisible = 0;

    for (int x = intersection.x(); x < intersection.x() + intersection.width(); x++) {
        for (int y = intersection.y(); y < intersection.y() + intersection.height(); y++) {
            if (!p1.mask[y-p1.y][x-p1.x] || !p2.mask[y-p2.y][x-p2.x]) {
                invisible++;
                continue;
            }
            auto d1 = p1.data[y-p1.y][x-p1.x];
            auto d2 = p2.data[y-p2.y][x-p2.x];

            if (d1 != d2) {
                return -1;
            }
        }
    }

    auto res = intersection.width() * intersection.height() - invisible;

    return res;
}


int neuralimage0(const QList<ImagePart> & __parts, QList<ImagePart> &results) {
    QMap<int, ImagePart> parts;
    for (int i = 0; i < __parts.length(); i++) {
        parts[i] = __parts[i];
    }

    //Sort items by correlation
    QMultiMap<int, QPair<int, int>> byCorrel;
    QMap<int, QMap<int, int>> cache;

    for (int i = 0; i < __parts.length(); i++) {
        for (int j = i+1; j < __parts.length(); j++) {
            int x = correl(__parts[i], __parts[j]);

            if (x >= 0) {
                byCorrel.insertMulti(x, QPair<int,int>(i,j));
            }
            cache[i][j] = x;
        }
    }

    while (byCorrel.count() > 0) {
        auto lastElem = byCorrel.last();
        auto p1 = lastElem.first;
        auto p2 = lastElem.second;

        if (!parts.contains(p1) || !parts.contains(p2)) {
            byCorrel.remove(byCorrel.lastKey(), lastElem);
            continue;
        }

        cout << parts.count() << endl;
        cout << "merging " << p1 << " " << p2 << " - " << byCorrel.lastKey() << endl;

        if (correl(parts[p1], parts[p2]) != byCorrel.lastKey()) {
            cout << "difference: " << correl(parts[p1], parts[p2]) << ", " << byCorrel.lastKey() << endl;
            byCorrel.remove(byCorrel.lastKey(), lastElem);
            continue;
        }

        /* Merge them */
        ImagePart b2 = parts[p2];
        ImagePart b = parts[p1];

        parts[p1] = b.united(b2);

        //Update elements
        parts.remove(p2);
        cache.remove(p2);

        for (int x : parts.keys()) {
            if (x == p1) {
                continue;
            }

            int p = p1;

            cache[x].remove(p2);
            int cval = correl(parts[x], parts[p1]);

            if (x > p) {
                std::swap(x, p);
            }

            if (cache[x][p] != cval) {
                byCorrel.remove(cache[x][p], QPair<int,int>(x, p));

                if (cval >= 0) {
                    cout << "increase correl between " << x << ", " << p << ": " << cval << " (old: "
                         << cache[x][p] << ")" << endl;
                    byCorrel.insertMulti(cval, QPair<int, int>(x, p));
                } else {
                    cout << "destroyed correl between " << x << ", " << p << ": " << cval << " (old: "
                         << cache[x][p] << ")" << endl;
                }
            }
            cache[x][p] = cval;
        }
    }

//    for (int x : parts.keys()) {
//        for (int y : parts.keys()) {
//            if (y > x && correl(parts[x], parts[y]) >= 0) {
//                cout << "Missed opportunity: " << x << " " << y << endl;
//            }
//        }
//    }
    results = parts.values();

    return parts.count();
}

int neuralimage(const QList<ImagePart> & parts, QList<QSet<int>> &results)
{
    auto graph = generateGraph(parts);

    QMultiMap<int, int> nodesByDegree;

    for (int x : graph.keys()) {
        nodesByDegree.insertMulti(graph[x].count(), x);
    }

    auto order = qReversed(nodesByDegree.values());

    QMultiMap<int, int> res2;

    QSet<int> done;
    for (int node : order) {
        if (!graph.contains(node)) {
            continue;
        }

        done.insert(node);

        results.push_back(QSet<int>() << node);

        cout << results.count() << " " <<  graph.size() << " " << res2.keys().toSet().count() << endl;

        QMultiMap<int, int> nodesByCorrel;

        for (int x : graph.keys()) {
            if (x == node || done.contains(x)) {
                continue;
            }

            nodesByCorrel.insertMulti(correl(parts[x], parts[node]), x);
        }
        nodesByCorrel.remove(-1);
        //nodesByCorrel.remove(0);

        for (int x: qReversed(nodesByCorrel.values())) {
            if (!graph[node].contains(x)) {
                if (done.contains(node)) {
                    qDebug() << "Error, already done";
                }
                graph[node].unite(graph[x]);
                graph.remove(x);
                res2.insertMulti(node, x);
                //order.removeOne(x);

                results.back() << x;
            }
        }
    }

    return graph.count();
}

//void regularBenchmark()
//{
//    long long int total= 0;
//    int n = 64;
//    int l = 16;
//    int s = 20;
//    int type = 'd';//argv[1][0];
//    const char *prefix;

//    for (double p = 0.4; p <= 0.8; p += 0.1) {
//        total = 0;
//        QFile out;
//        switch (type) {
//            case 'd':
//                for (int x = 0; x < 1000; x++) {
//                    cout << char(type) << x << "..." << endl;
//                    auto parts = generateParts(n, l, s, p);
//                    auto graph = generateGraph(parts);
//                    total += dsatur(graph);
//                }
//                prefix = "dsatur";
//                break;
//            case 'c':
//                for (int x = 0; x < 1000; x++) {
//                    cout << char(type) << x << "..." << endl;
//                    auto parts = generateParts(n, l, s, p);
//                    total += neural2(parts);
//                }
//                prefix = "correl";
//                break;
//            case 'r':
//                for (int x = 0; x < 1000; x++) {
//                    cout << char(type) << x << "..." << endl;
//                    auto parts = generateParts(n, l, s, p);
//                    auto graph = generateGraph(parts);
//                    total += rlf(graph);
//                }
//                prefix = "rlf";
//                break;
//            case 'l':
//                for (int x = 0; x < 1000; x++) {
//                    cout << char(type) << x << "..." << endl;
//                    auto parts = generateParts(n, l, s, p);
//                    auto graph = generateGraph(parts);
//                    total += greedy(graph);
//                }
//                prefix = "greedy";
//                break;
//       }
//       out.setFileName(prefix + QString("-") + QString::number(p) +
//                        QString("r-") + QString::number(s) + ".txt");
//       out.open(QFile::WriteOnly);
//       out.write((QString::number(total) + " " +QString::number(double(total) / 1000) + "\n").toUtf8());
//       out.close();
//     }
//}
