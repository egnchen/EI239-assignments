#include <cstring>
#include <iostream>
#include <algorithm>
using namespace std;

struct score {
    short scores[6];
    short score_sum;
    static short perm[6];
    score() {}
    bool operator<(const score &rhs) const {
        if (score_sum != rhs.score_sum)
            return score_sum < rhs.score_sum;
        for (int i = 0; i < 6; i++)
            if (scores[perm[i]] != rhs.scores[perm[i]])
                return scores[perm[i]] < rhs.scores[perm[i]];
        return false; // equal
    }
};

short score::perm[6] = {0};

void quick_sort(score to_sort[], int s, int e) {
    if (e - s < 1)
        return;
    score flag = to_sort[s];
    int i = s, j = e - 1;
    while (i < j) {
        while (i < j && !(flag < to_sort[j]))
            j--;
        to_sort[i] = to_sort[j];
        while (i < j && !(to_sort[i] < flag))
            i++;
        to_sort[j] = to_sort[i];
    }
    to_sort[i] = flag;
    quick_sort(to_sort, s, i);
    quick_sort(to_sort, i + 1, e);
}

int main(int argc, char const *argv[]) {
    int n;
    do {
        cin.clear();
        cin.sync();
        cout << "Please enter the size of dataset:";
        cin >> n;
    } while (!cin);
    score *data = new score[n];
    cout << "Please enter the order of scores by integers:" << endl;
    for (int i = 0; i < 6; i++) {
        cin >> score::perm[i];
    }
    cout << "Please enter all scores in order:" << endl;
    for (int i = 0; i < n; i++) {
        int s = 0;
        for (int j = 0; j < 6; j++) {
            cin >> (data[i].scores[j]);
            s += data[i].scores[j];
        }
        data[i].score_sum = s;
    }
    quick_sort(data, 0, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 6; j++)
            cout << data[i].scores[j] << ' ';
        cout << '\t' << data[i].score_sum << endl;
    }
    return 0;
}
