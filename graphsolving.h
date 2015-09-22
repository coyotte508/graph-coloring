#ifndef GRAPHSOLVING_H
#define GRAPHSOLVING_H

#include <QSet>
#include <QMap>
#include <QByteArray>

int dsatur(const QMap<int, QSet<int>> & graph);

int greedy(const QMap<int, QSet<int>> & graph);

int neural(const QVector<QByteArray> & _parts);

int neural2(const QVector<QByteArray> & parts);

int rlf(const QMap<int, QSet<int>> & _graph);

#endif // GRAPHSOLVING_H
