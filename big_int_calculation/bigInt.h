#ifndef __BIGINT
#define __BIGINT

#include <iostream>

class bigInt {
    // double-linked list node definition
    struct node {
        int data;
        static const int NODE_SIZE = 10000;
        node *next, *prev;
        node(int data, node *prev = NULL) : data(data), next(NULL), prev(prev) {
            if (prev)
                prev->next = this;
        }
    };
    // double-linked list head & tail
    node *first, *last;

    // interger sign bit
    static const int SIGN_POSITIVE = 1;
    static const int SIGN_NEGATIVE = -1;
    int sign;

    // private methods
    node *push_back(int);           // regular insert node
    node *push_reverse(int);        // reversly insert node
    void clear_redundant_zero(void);// clear dangling zero nodes
    void clear(void);               // erase entire list
    // carry bit parser
    static inline void parse_carry(int &, int &);
    // vector adder
    static node *do_add(const node *, const node *, node *, int);
    // compare absolute values
    friend bool abs_cmp(node *, node *);
    // input/output overrides
    friend std::istream &operator>>(std::istream &, bigInt &);
    friend std::ostream &operator<<(std::ostream &, const bigInt &);

  public:
    // constructors
    bigInt(void) : sign(SIGN_POSITIVE), first(NULL), last(NULL){};
    bigInt(long long int);
    bigInt(const bigInt &);
    bigInt(bigInt &&);
    // deconstructor
    ~bigInt(void);
    // operator overrides
    bigInt &operator=(const bigInt &);
    bigInt &operator=(bigInt &&);
    bigInt operator+(const bigInt &) const;
    bigInt operator-(const bigInt &) const;
    bigInt operator*(const bigInt &)const;
    bigInt operator/(const bigInt &) const;
    bigInt &operator--(void);
    bigInt &operator++(void);
    // power methods
    bigInt power(long long int) const;  // by 2
    bigInt power(const bigInt &) const; // by node::NODE_SIZE
    bigInt factorial(void);         // factorial method
    inline operator bool() const;   // bool conversion
};

#endif