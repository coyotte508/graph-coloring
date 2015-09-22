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
    QVector<QVector<QRgb>> data;
};

int neuralimage(const QList<ImagePart> & parts, QList<QSet<int>> &results);

void imageBenchmark()
{
    QList<QImage> images;

    int n = 9;
    int s = 100;
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
        for (int i = 0; i < partH; i++) {
            for (int j = 0; j < partW; j++) {
                image.setPixel(p.x+j, p.y+i, p.data[i][j]);
            }
        }
    };

    QList<QSet<int>> results;
    cout << neuralimage(parts, results) << endl;

    for (int x = 0; x < results.length(); x++) {
        auto set = results[x];
        QImage test(image_width, image_height, QImage::Format_ARGB32);
        test.fill(qRgba(0,0,0,255));

        for (int i : set) {
            stuffPart(parts[i], test);
        }

        test.save("test" + QString::number(x) + ".png");
    }

    //qDebug() << results;
}

template int correl<>(const ImagePart &p1, const ImagePart &p2);

int correl(const ImagePart &p1, const ImagePart &p2){
    QRect intersection = QRect(p1.x, p1.y, partW, partH).intersected(QRect(p2.x, p2.y, partW, partH));

    if (!intersection.isValid()) {
        return 0;
    }

    for (int x = intersection.x(); x < intersection.x() + intersection.width(); x++) {
        for (int y = intersection.y(); y < intersection.y() + intersection.height(); y++) {
            if (p1.data[y-p1.y][x-p1.x] != p2.data[y-p2.y][x-p2.x]) {
                return -1;
            }
        }
    }

    return intersection.width() * intersection.height();
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
            if (x == node) {
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
