#include "myHash.hpp"
#include <functional>
#include <iostream>
#include <string>

using namespace std;

const int SIZE = 31;
int main(int argc, char const *argv[]) {
    linearHashTable<string, string> *hash1;
    openHashTable<string, string> *hash2;

    hasher<string> *hashers[] = { new stringSumHasher(), new stringSepSumHasher(),
                                new nameHasher(), new nameSquareMidHasher()};
    const char *names[] = {"stringSumHasher", "stringSepSumHasher",
                           "nameHasher", "nameSquareMidHasher"};
    cout << "Choose the hasher you want:" << endl;
    for (int i = 0; i < 4; i++)
        cout << '\t' << i << ' ' << names[i] << endl;
    int h = 0;
    cin >> h;
    
    hash1 = new linearHashTable<string, string>(SIZE, *hashers[h]);
    hash2 = new openHashTable<string, string>(SIZE, *hashers[h]);
    hash1->start_stat();
    hash2->start_stat();
    
    cout << "Please enter 30 names." << endl;
    for (int i = 0; i < 30; i++) {
        string name;
        cin >> name;
        if (!hash1->insert(name, name))
            cout << "linear insertion failed" << endl;
        if (!hash2->insert(name, name))
            cout << "open insertion failed" << endl;
    }
    cout << "Linear hash table collision count:" << hash1->collision << endl;
    cout << "Open hash table collision count:" << hash2->collision << endl;
    while (1) {
        cout << "enter a name to check:" << endl;
        string name;
        cin >> name;
        try {
            cout << "In linear hashtable: " << (*hash1)[name] << endl;
            cout << "Found." << endl;
        } catch (out_of_range) {
            cout << "Not found." << endl;
        }
        try {
            cout << "In open hashtable: " << (*hash2)[name] << endl;
            cout << "Found." << endl;
        } catch (out_of_range) {
            cout << "Not found." << endl;
        }
    }
    return 0;
}
