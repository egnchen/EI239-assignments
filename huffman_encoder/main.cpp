#include "huffman.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

huffmanTree hfEncoder;

inline bool check_init(
    const string &prompt = "Error: you should initialize the tree first.") {
    if (hfEncoder.empty()) {
        cout << prompt << endl;
        return false;
    }
    return true;
}

void init() {
    int n;
    if (!hfEncoder.empty()) {
        hfEncoder.clear();
        cout << "Previous data cleared." << endl;
    }
    cout << "Initialization: Please enter number of key-value pairs:" << endl;
    cin >> n;
    cout << "Please enter key-value pairs." << endl;
    cout << "Use \\ + white space for white space, \\t for tab, \\n for "
            "newline, \\\\ for \\."
         << endl;
    vector<huffmanTree::single_freq_data> pairs;
    while (n--) {
        char data;
        float freq;
        cin >> data;
        if (data == '\\') {
            cin.get(data);
            switch (data) {
            case ' ':
                data = ' ';
                break;
            case 't':
                data = '\t';
                break;
            case 'n':
                data = '\n';
                break;
            case '\\':
                data = '\\';
            default:
                cout << "Invalid character. Please enter again." << endl;
                cin.ignore(1024, '\n');
                n++;
                continue;
            }
        }
        cin >> freq;
        pairs.push_back({data, freq});
    }
    pairs.push_back({0, 1}); // THIS IS A MUST
    hfEncoder.build_tree_from_list(pairs);
    const vector<huffmanTree::huffman_treenode> &tree = hfEncoder.get_tree();
    ofstream fout("hfmTree");
    if (!fout) {
        cout << "Error: failed to open file." << endl;
        return;
    }
    auto first = &(tree[0]);
    for (auto i: tree) {
        fout << int(i.data) << ' ' << (i.left ? i.left - &tree[0] : -1)  << ' '
             << (i.right ? i.right - &tree[0] : -1) << ' ' << i.weight << endl;
    }
    fout.close();
}

void encode() {
    if (!check_init())
        return;
    cout << "Read string from plainFile and write encoded data to codeFile."
         << endl;
    ofstream fout("codeFile");
    ifstream fin("plainFile");
    if (!fin || !fout) {
        cout << "Error: file cannot be opened." << endl;
        return;
    }
    hfEncoder.encode(fout, fin);
    fout.close();
    fin.close();
    cout << "Encoded." << endl;
}

void encode_from_stdin() {
    cin.ignore(1024, '\n');
    cout << "Read string from stdin, build new tree and write encoded data to "
            "codeFile."
         << endl;
    cout << "Please enter the content you want to encode:" << endl;
    string content;
    getline(cin, content);
    hfEncoder.clear();
    istringstream ss1(content), ss2(content);
    ofstream fout("codeFile");
    if (!fout) {
        cout << "Error: file cannot be opened." << endl;
    }
    hfEncoder.build_tree(ss1);
    cout << "Tree built." << endl;
    hfEncoder.encode(fout, ss2);
    cout << "Encoded." << endl;
    fout.close();
}

void decode() {
    if (!check_init())
        return;
    cout << "Read encoded data from codeFile and write decoded string to "
            "textFile."
         << endl;
    ofstream fout("textFile");
    ifstream fin("codeFile");
    if (!fin || !fout) {
        cout << "Error: file cannot be opened." << endl;
        return;
    }
    hfEncoder.decode(fout, fin);
    fin.close();
    fout.close();
    cout << "Decoded." << endl;
}

void print_code() {
    ifstream fin("codeFile");
    ofstream fout("codePrint");
    if (!fin || !fout) {
        cout << "Error: file cannot be opened." << endl;
        return;
    }
    char nl = 0;
    while (1) {
        static unsigned int i;
        i = fin.get();
        if (fin.eof())
            break;
        cout << hex << setw(2) << setfill('0') << i << ' ';
        fout << hex << setw(2) << setfill('0') << i << ' ';
        if (++nl == 50) {
            cout << endl;
            fout << endl;
            nl = 0;
        }
    }
    cout << endl;
}

void print_tree() {
    if (!check_init())
        return;
    hfEncoder.print_tree();
    ofstream fout("treePrint");
    hfEncoder.print_tree(fout);
    fout.close();
}

void help() {
    cout << "List of possible operations:" << endl;
    cout << "I(nitialization)\tE(ncode)" << endl;
    cout << "P(rint code)\tD(ecode)" << endl;
    cout << "T(ree print)\tR(ead from stdin)" << endl;
    cout << "H(elp)\tC(lear)\tQ(uit)" << endl;
}

inline void clear() { hfEncoder.clear(); }
inline void quit() { exit(0); }

int main(void) {
    char ops[] = "IEDPTRHCQ";
    void (*funcs[])(void) = {init,       encode,     decode,
                             print_code, print_tree, encode_from_stdin,
                             help,       clear,      quit};
    cout << "Huffman encoder." << endl;
    cout << "Enter 'H' for help." << endl;
    while (1) {
        char op;
        do {
            cout << ">>> ";
            cin >> op;
            op = toupper(op);
        } while (!strchr(ops, op));
        funcs[strchr(ops, op) - ops]();
    }
    return 0;
}