#include "graphsolving.h"
#include "functions.h"

int dsatur(const QMap<int, QSet<int>> & graph) {
    QMap<int, int> colors;
    QMap<int, QSet<int>> saturColors;

    //Find the most saturated node
    auto findSat = [&]() {
        int maxSat = -1;
        int maxDeg = 0;
        int current = -1;

        for (int key : graph.keys()) {
            if (colors.contains(key)) {
                continue;
            }
            if (saturColors[key].count() > maxSat ||
                    (saturColors[key].count() == maxSat && graph[key].count() > maxDeg)) {
                current = key;
                maxSat = saturColors[key].count();
                maxDeg = graph[key].count();
            }
        }

        return current;
    };


    //color the graph
    int maxColor = 0;
    int node;

    while ( (node = findSat()) != -1) {
        for (int i = 0; ; i++) {
            if(!saturColors[node].contains(i)) {
                colors[node] = i;

                maxColor = std::max(i, maxColor);

                for (int x : graph[node]) {
                    saturColors[x].insert(i);
                }

                break;
            }
        }
    }

    //return the number of colors
    return maxColor + 1;
}

int greedy(const QMap<int, QSet<int>> & graph) {
    QMultiMap<int, int> nodesByDegree;
    QMap<int, QSet<int>> nodesByColors;

    for (int x : graph.keys()) {
        nodesByDegree.insertMulti(graph[x].count(), x);
    }

    //Go through all the nodes
    for (int node : qReversed(nodesByDegree.values())) {
        //Check the colors
        bool colorFound = false;
        for (int x : nodesByColors.keys()) {
            colorFound = true;

            for (int y: nodesByColors[x]) {
                if (graph[node].contains(y)) {
                    colorFound = false;
                    break;
                }
            }

            if (colorFound) {
                nodesByColors[x].insert(node);
                break;
            }
        }

        if (!colorFound) {
            nodesByColors[nodesByColors.count()].insert(node);
        }
    }

    //return the number of colors
    return nodesByColors.count();
}

int neural(const QVector<QByteArray> & _parts) {
    QList<QByteArray> __parts = _parts.toList();
    QMap<int, QByteArray> parts;
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

//        cout << parts.count() << endl;
//        cout << "merging " << p1 << " " << p2 << " - " << byCorrel.lastKey() << endl;

        if (correl(parts[p1], parts[p2]) != byCorrel.lastKey()) {
//            cout << "difference: " << correl(parts[p1], parts[p2]) << ", " << byCorrel.lastKey() << endl;
            byCorrel.remove(byCorrel.lastKey(), lastElem);
            continue;
        }

        /* Merge them */
        QByteArray b2 = parts[p2];
        QByteArray b = parts[p1];

        for (int i = 0; i < b.length(); i++) {
            if (b2[i]) {
                b[i] = b2[i];
            }
        }
        parts[p1] = b;

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
                    //cout << "increase correl between " << x << ", " << p << ": " << cval << endl;
                    byCorrel.insertMulti(cval, QPair<int, int>(x, p));
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

    return parts.count();
}

int neural2(const QVector<QByteArray> & parts) {
//    auto graph = generateGraph(parts);

//    QMultiMap<int, int> nodesByDegree;

//    for (int x : graph.keys()) {
//        nodesByDegree.insertMulti(graph[x].count(), x);
//    }

//    for (int node : qReversed(nodesByDegree.values())) {
//        if (!graph.contains(node)) {
//            continue;
//        }

//        QMultiMap<int, int> nodesByCorrel;

//        for (int x : graph.keys()) {
//            if (x == node) {
//                continue;
//            }

//            nodesByCorrel.insertMulti(correl(parts[x], parts[node]), x);
//        }
//        nodesByCorrel.remove(-1);

//        for (int x: qReversed(nodesByCorrel.values())) {
//            if (!graph[node].contains(x)) {
//                graph[node].unite(graph[x]);
//                graph.remove(x);
//            }
//        }
//    }

//    return graph.count();
    return 0;
}

int rlf(const QMap<int, QSet<int>> & _graph) {
    auto graph = _graph;
    QMultiMap<int, int> nodesByDegree;

    auto correl = [&graph](int x, int y) {
        QSet<int> s = graph[x];
        if (s.contains(y)) {
            return -1;
        }
        s.intersect(graph[y]);
        return s.count();
    };

    for (int x : graph.keys()) {
        nodesByDegree.insertMulti(graph[x].count(), x);
    }

    for (int node : qReversed(nodesByDegree.values())) {
        if (!graph.contains(node)) {
            continue;
        }

        QMultiMap<int, int> nodesByCorrel;

        for (int x : graph.keys()) {
            if (x == node) {
                continue;
            }

            nodesByCorrel.insertMulti(correl(x, node), x);
        }
        nodesByCorrel.remove(-1);

        for (int x: qReversed(nodesByCorrel.values())) {
            if (!graph[node].contains(x)) {
                graph[node].unite(graph[x]);
                graph.remove(x);
            }
        }
    }

    return graph.count();
}
