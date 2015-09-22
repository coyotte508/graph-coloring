#include <QSet>
#include "functions.h"

std::default_random_engine &randg() {
   static thread_local std::default_random_engine r = std::default_random_engine(std::random_device()());

   return r;
}

bool happens(double p) {
    return norm(randg()) < p;
}

int correl(const QByteArray &b1, const QByteArray &b2) {
    int c = 0;

    for (int i = 0; i < b1.length(); i++) {
        if (b1[i] == 0 || b2[i] == 0) {
            continue;
        }
        if (b1[i] != b2[i]) {
            return -1;
        }
        c++;
    }

    return c;
}


/* n messages of length l repeated s times each with erasure probability p */
QVector<QByteArray> generateParts(int n, int l, int s, double p) {
    /* Generate noisy messages */
    QVector<QByteArray> parts;

    for (int i = n; i > 0; i--) {
        /* Create the original message */
        QByteArray ori;

        for (int i = l; i > 0; i--) {
            ori.push_back(zeroone(randg()) ? 1 : -1);
        }

        /* Get noisy renditions */
        for (int i = s; i > 0; i--) {
            QByteArray copy = ori;

            for (int i = 0; i < l; i++) {
                if (happens(p)) {
                    copy[i] = 0;
                }
            }

            parts.push_back(copy);
        }
    }

    std::shuffle(parts.begin(), parts.end(), randg());

    return parts;
}

double density(const QMap<int, QSet<int>> & graph) {
    int total = 0;

    for (const QSet<int> &s : graph) {
        total += s.count();
    }

    return (double(total) / graph.count()) / (graph.count()-1);
}
