#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int max_semesters;
int max_limit = 10;
int max_N = 0;

struct course {
    int credit;
    struct node {
        course *c;
        node *next;
        node() {}
    };
    node *tails;
    course() {}
};
course *courses;
uint8_t *pl;
inline int limited_max(int a, int b) {
    if (a > b) {
        if (a > max_limit)
            return b;
        else
            return a;
    } else {
        if (b > max_limit)
            return a;
        else
            return b;
    }
}

// Problem 1
// Use bfs and dp programming
bool bfs(ostream &out) {
    out << "Aggressive course arrangement:" << endl;
    vector<course *> *p = new vector<course *>;
    vector<course *> *q = new vector<course *>;
    uint8_t *pre_left = new uint8_t[max_N + 1];
    memcpy(pre_left, pl, sizeof(uint8_t) * (max_N + 1));
    int total = 0;
    for (int i = 1; i <= max_N; i++)
        if (!pre_left[i])
            p->push_back(courses + i);

    int sem = 0;
    do {
        out << "\tSemester " << ++sem << ':';
        // dp programming
        int *dp = new int[p->size()];
        dp[0] = limited_max(0, p->front()->credit);
        for (int i = 1; i < p->size(); i++) {
            dp[i] = limited_max((*p)[i]->credit, dp[i - 1]);
            for (int j = 0; j < i; j++)
                dp[i] = limited_max(dp[i], dp[j] + (*p)[i]->credit);
        }
        // regenerate the plan and prepare for next
        q->clear();
        int i = p->size() - 1;
        while (i >= 0) {
            if (dp[i] != dp[i - 1]) {
                // (*p)[i] is chosen
                total++;
                out << "C" << (*p)[i] - courses << ' ';
                course::node *cur = (*p)[i]->tails;
                while (cur) {
                    if (pre_left[cur->c - courses] >= 1)
                        if (!--pre_left[cur->c - courses])
                            q->push_back(cur->c);
                    cur = cur->next;
                }
                if ((*p)[i]->credit == dp[i]) {
                    // choose itself and discard the rest
                    break;
                } else {
                    // choose itself and still something else
                    int tmp = i;
                    i -= 1;
                    while (i >= 0 && dp[tmp] - dp[i] != (*p)[tmp]->credit) {
                        q->push_back((*p)[i]);
                        i--;
                    }
                }
            } else {
                // not chosen
                q->push_back((*p)[i]);
                i--;
            }
        }
        out << endl;
        delete[] dp;
        swap(p, q);
    } while (sem < 6 && !p->empty());
    delete[] pre_left;
    delete p, q;
    return total == max_N;
}

vector<course *> best_topo;
vector<int> best_pos;
float min_segma2 = 1e10; // +inf
void output_best_arrangement(ostream &out){
    // output
    out << "Uniform distributed course arrangement:" << endl;
    for (int s = 0, e, i = 0; i < best_pos.size(); i++) {
        out << "\tSemester " << (i + 1) << ':';
        e = best_pos[i];
        for (int j = s, su = 0; j < e; j++)
            out << 'C' << (best_topo[j] - courses) << ' ';
        out << endl;
        s = e;
    }
    out << "segma^2 = " << min_segma2 << endl;
}

bool save_best_arrangement(const vector<course *> &topo, vector<int> &pos,
                           int depth) {
    // calculate mu and segma^2
    float mu = 0, segma2 = 0;
    for (vector<course *>::const_iterator i = topo.cbegin(); i != topo.cend();
         ++i)
        mu += (*i)->credit;
    mu /= (depth + 1);
    for (int s = 0, i = 0; i <= depth; s = pos[i++]) {
        float su = 0;
        for (int j = s; j < pos[i]; j++)
            su += topo[j]->credit;
        su -= mu;
        segma2 += su * su;
    }
    segma2 /= (depth + 1);
    // save best arrangement
    if (segma2 < min_segma2) {
        best_topo = topo;
        best_pos = pos;
        min_segma2 = segma2;
        output_best_arrangement(cout);
        return true;
    }
    return false;
}

bool check_validity(const vector<course *> &topo, const vector<int> &sep,
                    int depth, bool vis[]) {
    // check validity
    memset(vis, 0, sizeof(bool) * (topo.size() + 1));
    for (int s = 0, i = 0; i <= depth; s = sep[i++]) {
        // set visit flags and check credit limit
        for (int j = s, su = 0; j < sep[i]; j++) {
            su += topo[j]->credit;
            if (su > max_limit)
                return false;
            vis[topo[j] - courses] = 1;
        }
        // check preliminary courses
        for (int j = s; j < sep[i]; j++) {
            course::node *cur = topo[j]->tails;
            while (cur) {
                if (vis[cur->c - courses])
                    return false;
                cur = cur->next;
            }
        }
    }
    return true;
}

void run_check_iter(const vector<course *> &topo, vector<int> &sep, bool vis[],
                    int depth) {
    // enumerate all possible solutions recursively
    if (depth + 1 < max_semesters) {
        for (int i = depth ? sep[depth - 1] + 1 : 1; i < topo.size(); i++) {
            sep[depth] = i;
            run_check_iter(topo, sep, vis, depth + 1);
        }
    }
    sep[depth] = topo.size();
    if (check_validity(topo, sep, depth, vis))
        // new solution
        save_best_arrangement(topo, sep, depth);
}

void run_check(const vector<course *> &topo) {
    // wrapper
    bool *vis = new bool[topo.size() + 1];
    vector<int> sep(max_semesters);
    run_check_iter(topo, sep, vis, 0);
    delete[] vis;
}

// problem 2
// use topo sort and enumeration
void topo2() {
    uint8_t *pre_left = new uint8_t[max_N + 1];
    memcpy(pre_left, pl, sizeof(uint8_t) * (max_N + 1));
    queue<course *> q;
    vector<course *> topo;
    vector<int> sep;
    // topo sort
    for (int i = 1; i <= max_N; i++) {
        if (pre_left[i] == 0)
            q.push(courses + i);
    }
    while (!q.empty()) {
        course *cur = q.front();
        q.pop();
        topo.push_back(cur);
        course::node *p = cur->tails;
        while (p) {
            if (--pre_left[p->c - courses] == 0)
                q.push(p->c);
            p = p->next;
        }
    }
    run_check(topo);
    delete[] pre_left;
}

int get_course_id(istream &in, ostream &out, const string prompt = "") {
    string s;
    do {
        if (!prompt.empty())
            cout << prompt << endl;
        cin >> s;
    } while (!(toupper(s[0]) == 'C'));
    return atoi(s.c_str() + 1);
}

int main(int argc, char const *argv[]) {
    cout << "Enter amount of courses:" << endl;
    cin >> max_N;
    cout << "Enter number of semesters:" << endl;
    cin >> max_semesters;
    cout << "Enter credit limitation per semester:" << endl;
    cin >> max_limit;
    courses = new course[max_N + 1];
    pl = new uint8_t[max_N + 1];
    memset(courses, 0, sizeof(course) * (max_N + 1));
    memset(pl, 0, sizeof(uint8_t) * (max_N + 1));
    for (int i = 1; i <= max_N; i++) {
        cout << "Please enter information for course C" << i << ':' << endl;
        cout << "Course credit:";
        cin >> (int &)(courses[i].credit);
        cout << "Number of prerequisite courses:";
        cin >> (int &)(pl[i]);
        if (pl[i]) {
            cout << "Prerequisite courses(Cxx, seperate by space or newline):"
                 << endl;
            for (int j = 0; j < pl[i]; j++) {
                int cur = get_course_id(cin, cout);
                course::node *p = new course::node();
                p->c = courses + i;
                p->next = courses[cur].tails;
                courses[cur].tails = p;
            }
        }
    }
    ofstream fout("syllabus.txt");
    if (!bfs(fout)) {
        cout << "Arrangement can't be done: loop exists." << endl;
        fout.close();
        fout.open("syllabus.txt", fout.trunc);
        fout << "Arrangement can't be done: loop exists." << endl;
        fout.close();
        return 1;
    }
    topo2();
    output_best_arrangement(fout);
    cout << "Arrangement generated. View syllabus.txt for detail." << endl;
    return 0;
}