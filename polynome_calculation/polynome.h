#ifndef __POLY
#define __POLY

#include <iostream>
using namespace std;
class polynome {
    struct node {
        int ratio, exp;
        node *next;
        node(int ratio, int exp = 0) : ratio(ratio), exp(exp), next(NULL){};
        ~node() {
            if (next)
                delete next;
        }
    };
    node *first;

    friend istream &operator>>(istream &, polynome &);
    friend ostream &operator<<(ostream &, const polynome &);

  public:
    polynome(int v = 0) { first = v ? new node(v) : NULL; }
    polynome(const polynome &);
    polynome(polynome &&rhs) {
        first = rhs.first;
        rhs.first = NULL;
    }
    ~polynome() {
        if (first)
            delete first;
    }
    void autoclean(void);

    polynome operator+(const polynome &) const;
    polynome operator*(const polynome &)const;
    polynome &operator=(polynome &&rhs) {
        if (first)
            delete first;
        first = rhs.first;
        rhs.first = NULL;
        return *this;
    }
    polynome differential(void) const;
};

#endif