#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <random>
#include <QVector>
#include <QByteArray>
#include <QMap>
#include <QSet>

std::default_random_engine &randg();

static std::uniform_real_distribution<> norm(0, 1.0);
static std::uniform_int_distribution<> zeroone(0, 1);

bool happens(double p);

/* n messages of length l repeated s times each with erasure probability p */
QVector<QByteArray> generateParts(int n, int l, int s, double p);

QMap<int, QSet<int>> generateGraph(const QVector<QByteArray> &parts);

double density(const QMap<int, QSet<int>> & graph);

template <typename T>
QList<T> qReversed( const QList<T> & in ) {
    QList<T> result;
    result.reserve( in.size() ); // reserve is new in Qt 4.7
    std::reverse_copy( in.begin(), in.end(), std::back_inserter( result ) );
    return result;
}

template<class T>
int correl(T , T ) {
    return -1;
}

template<class T>
QMap<int, QSet<int>> generateGraph(const T &parts) {
    QMap<int, QSet<int>> graph;

    for(int i = 0; i < parts.length(); i++) {
        for (int j = i+1; j < parts.length(); j++) {
            const auto &p1 = parts[i];
            const auto &p2 = parts[j];

            graph[i]; graph[j]; //Create default elements

            if (correl(p1, p2) < 0) {
                graph[i].insert(j);
                graph[j].insert(i);
            }
        }
    }

    return graph;
}

#endif // FUNCTION_HPP
