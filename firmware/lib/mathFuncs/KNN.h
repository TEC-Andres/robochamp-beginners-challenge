#pragma once
#include <stddef.h>

// k-nearest-neighbours over 3 features, fixed capacity (no heap). Euclidean distance, majority vote.
template <size_t Capacity>
struct KNN {
    struct Sample {
        float f[3];
        int label;
    };

    Sample samples[Capacity];
    size_t count = 0;

    // Returns false when full.
    bool add(float a, float b, float c, int label) {
        if (count >= Capacity) return false;
        samples[count++] = {{a, b, c}, label};
        return true;
    }

    // Returns the winning label, or -1 if there are no samples. Ties go to the nearest voter.
    int classify(float a, float b, float c, size_t k = 3) const {
        if (count == 0) return -1;
        if (k > count) k = count;

        float dist[Capacity];
        bool used[Capacity] = {};
        int nearest[Capacity]; // labels of the k nearest, closest first
        for (size_t i = 0; i < count; i++) {
            const float da = samples[i].f[0] - a, db = samples[i].f[1] - b, dc = samples[i].f[2] - c;
            dist[i] = da * da + db * db + dc * dc;
        }
        for (size_t n = 0; n < k; n++) {
            size_t best = Capacity;
            for (size_t i = 0; i < count; i++)
                if (!used[i] && (best == Capacity || dist[i] < dist[best])) best = i;
            used[best] = true;
            nearest[n] = samples[best].label;
        }

        int winner = nearest[0];
        size_t winnerVotes = 0;
        for (size_t n = 0; n < k; n++) {
            size_t votes = 0;
            for (size_t m = 0; m < k; m++) votes += nearest[m] == nearest[n];
            if (votes > winnerVotes) { winner = nearest[n]; winnerVotes = votes; }
        }
        return winner;
    }
};
