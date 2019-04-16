#include <cstring>
#include <iostream>
#include "polynome.h"
using namespace std;

int main(void) {
    polynome a, b;
    static char ops[] = "+*'";
    cout << "Polynome calculation." << endl;
    while (1) {
        char op;
        cout << "Choose the operation you want to conduct:" << endl;
        cout << "add(+) mul(*) differential(')" << endl;
        do
            cin >> op;
        while (strchr(ops, op) == NULL);
        if (op != '\'') {
            cout << "Enter two polynomes:" << endl;
            cout << "Enter the first polynome:" << endl;
            cin >> a;
            cout << "Enter the second polynome:" << endl;
            cin >> b;
            cout << a << ' ' << op << ' ' << b << " = ";
            if (op == '+')
                cout << a + b << endl;
            else
                cout << a * b << endl;
        } else {
            cout << "Enter one polynome:" << endl;
            cin >> a;
            cout << "d(" << a << ")/dx = " << a.differential() << endl;
        }
    }
    return 0;
}
