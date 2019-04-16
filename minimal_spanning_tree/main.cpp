#include <cstring>
#include <iostream>
using namespace std;

struct node {
    struct tail_node {
        node *n;
        char weight;
        tail_node *next;
        tail_node(node *n, int weight, tail_node *next)
            : n(n), weight(weight), next(next) {}
    };
    char name;
    tail_node *tails;
};

node *root;
node *nodes;
int n;
const int INT_INF = 0x7f7f7f7f;
void prim(int n) {
    int *low_cost = new int[n];
    node **start_node = new node *[n];
    bool *flags = new bool[n];
    memset(low_cost, 0x7f, sizeof(int) * n);
    memset(start_node, 0, sizeof(node *) * n);
    int start;
    start = 0;
    for (int i = 0; i < n-1; i++) {
        node::tail_node *tn = nodes[start].tails;
        flags[start] = true;
        low_cost[start] = INT_INF;
        while (tn) {
            if (!flags[tn->n - nodes] && low_cost[tn->n - nodes] > tn->weight) {
                low_cost[tn->n - nodes] = tn->weight;
                start_node[tn->n - nodes] = nodes + start;
            }
            tn = tn->next;
        }
        int min_cost = INT_INF;
        int min_end_node;
        for (int j = 0; j < n; j++) {
            if (low_cost[j] < min_cost) {
                min_cost = low_cost[j];
                min_end_node = j;
            }
        }
        cout << "Choose edge (" << start_node[min_end_node]->name << ','
             << nodes[min_end_node].name << ',' << min_cost << ')' << endl;
        start = min_end_node;
    }
    delete[] flags;
    delete[] start_node;
    delete[] low_cost;
}

int main(int argc, char const *argv[]) {
    cout << "Please input number of nodes:";
    cin >> n;
    nodes = new node[n];
    memset(nodes, 0, sizeof(node) * n);
    for (int i = 0; i < n; i++)
        nodes[i].name = 'a' + i;
    while (1) {
        cout << "Please input edge(startNode endNode weight):";
        char b, e;
        int weight;
        cin >> b;
        if (b == '_')
            break;
        cin >> e >> weight;
        nodes[b - 'a'].tails =
            new node::tail_node(nodes + e - 'a', weight, nodes[b - 'a'].tails);
        nodes[e - 'a'].tails =
            new node::tail_node(nodes + b - 'a', weight, nodes[e - 'a'].tails);
    }
    cout << endl;
    prim(n);
    return 0;
}
