#include <iostream>
#include <cstring>

#include "bigInt.h"
using namespace std;

int main(void) {
    bigInt a, b;
    static char ops[] = "+-*/^!";
    cout << "Big integer calculation." << endl;
    while(1){
        char op;
        cout << "Choose the operation you want to conduct:" << endl;
        cout << "add(+) sub(-) mul(*) div(/) pow(^) factorial(!)" << endl;
        do
            cin >> op;
        while (strchr(ops, op) == NULL);

        if(op != '!'){
            cout << "Enter two numbers:(a" << op << "b)" << endl;
            cin >> a >> b;
            cout << a << ' ' << op << ' ' << b << " = ";
            switch(op){
                case '+': cout << a + b << endl; break;
                case '-': cout << a - b << endl; break;
                case '*': cout << a * b << endl; break;
                case '/': cout << a / b << endl; break;
                case '^': cout << a.power(b) << endl;
            }
        } else {
            cout << "Enter a number(not too large please...)" << endl;
            cin >> a;
            cout << a << "! = " << a.factorial() << endl;
        }
    }
    return 0;
}