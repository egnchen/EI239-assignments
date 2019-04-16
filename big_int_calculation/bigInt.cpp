#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>

#include "bigInt.h"
using namespace std;

// constructors
// construct from a large int
bigInt::bigInt(long long int value) {
    // determine the sign
    first = last = NULL;
    if (value < 0) {
        sign = SIGN_NEGATIVE;
        value = -value;
    } else
        sign = SIGN_POSITIVE;
    do {
        push_back(value % node::NODE_SIZE);
        value /= node::NODE_SIZE;
    } while (value);
}

// copy constructor
bigInt::bigInt(const bigInt &rhs) {
    // create a new copy
    sign = rhs.sign;
    first = last = NULL;
    node *iter_orig = rhs.first;
    while (iter_orig) {
        push_back(iter_orig->data);
        iter_orig = iter_orig->next;
    }
}

// move constructor
bigInt::bigInt(bigInt &&rhs) {
    sign = rhs.sign;
    first = rhs.first;
    last = rhs.last;
    rhs.first = rhs.last = NULL;
}

// deconstructor
bigInt::~bigInt(void) { clear(); }

// erase the entire double-linked list
// needed in deconstruction & reassignment
void bigInt::clear(void) {
    if (!last)
        delete first;
    else if (first) {
        node *p = first->next;
        while (p) {
            delete p->prev;
            p = p->next;
        }
        delete last;
    }
}

// copy reassignment
// make a new copy
bigInt &bigInt::operator=(const bigInt &rhs) {
    this->clear();
    first = last = NULL;
    sign = rhs.sign;
    node *iter_orig = rhs.first;
    while (iter_orig) {
        push_back(iter_orig->data);
        iter_orig = iter_orig -> next;
    }
    return *this;
}

// move reassignment
// take control of the existing double-linked list
// own by the rvalue
bigInt &bigInt::operator=(bigInt &&rhs) {
    this->clear();
    sign = rhs.sign;
    first = rhs.first;
    last = rhs.last;
    rhs.first = rhs.last = NULL;
    return *this;
}

// input
istream &operator>>(istream &in, bigInt &rhs) {
    // buffer declarations
    static const int INPUT_BUFFER_SIZE = 85;
    static char input_buffer[INPUT_BUFFER_SIZE];

    int i = 0;
    char *p = input_buffer;
    do
        in.getline(p, INPUT_BUFFER_SIZE);
    while(!(*p));
    if (p[0] == '-') {
        rhs.sign = bigInt::SIGN_NEGATIVE;
        p++; // omit the first char
    } else
        rhs.sign = bigInt::SIGN_POSITIVE;

    rhs.clear();
    rhs.first = rhs.last = NULL;
    // use strtok to separate integer tokens
    p = strtok(p, ",");
    while (p) {
        rhs.push_reverse(atoi(p));
        p = strtok(NULL, ",");
    }

    return in;
}

// output
ostream &operator<<(ostream &out, const bigInt &rhs) {
    bigInt::node *p = rhs.last ? rhs.last : rhs.first;
    if (rhs.sign == bigInt::SIGN_NEGATIVE)
        out << '-';
    out << (p->data);
    p = p->prev;
    while (p) {
        out << ',' << setw(4) << setfill('0') << (p->data);
        p = p->prev;
    }
    return out;
}

// compare the two values by absolute value
// private method
// @return  true    if p1 >= p2
// @return  false   if p1 < p2
// @arg     p1      LSB of the first bigInt
// @arg     p2      LSB of the second bigInt
bool abs_cmp(bigInt::node *p1, bigInt::node *p2) {
    bool cmp = true; // default value abs(lhs)>=abs(rhs)
    while (p1 && p2) {
        // store the last comparison result
        if (p1->data != p2->data)
            cmp = p1->data >= p2->data;
        p1 = p1->next;
        p2 = p2->next;
    }
    if (p1 && !p2)
        return true;
    else if (!p1 && p2)
        return false;
    else
        return cmp;
}

// add a node to the list from the back
// new node follows the current last node
// and becomes the new last node
bigInt::node *bigInt::push_back(int data) {
    if (first)
        return last = new node(data, last ? last : first);
    else
        return first = new node(data);
}

// add a node to the list reversely
// new node is before the current first node
// and becomes the new first node
bigInt::node *bigInt::push_reverse(int data) {
    if (first) {
        first->prev = new node(data);
        first->prev->next = first;
        if (!last)
            last = first;
        return first = first->prev;
    } else
        return first = new node(data);
}

// clear dangling nodes whose data is zero
// private method
inline void bigInt::clear_redundant_zero(void) {
    if (!last || last->data)
        return;
    node *p = last;
    while (p && !(p->data) && p->prev) {
        p = p->prev;
        delete p->next;
    }
    p->next = NULL;
    last = p;
}

// bool conversion
// bool(num) = num != 0 where num is any bigInt
inline bigInt::operator bool(void) const { return first->data || first->next; }

// self decrement
bigInt &bigInt::operator--(void) {
    if (!(*this)) {
        // if *this == zero
        this->sign = SIGN_NEGATIVE;
        this->first->data = -1;
        return *this;
    }
    if (this->sign == SIGN_NEGATIVE)
        return ++(*this);
    node *p = this->first;
    do {
        p->data--;
        if (p->data < 0) {
            p->data += node::NODE_SIZE;
            p = p->next;
        } else
            break;
    } while (1);
    clear_redundant_zero();
    return *this;
}

// self increment
bigInt &bigInt::operator++(void) {
    if (!(*this)) {
        // if *this == zero
        this->sign = SIGN_POSITIVE;
        this->first->data = 1;
        return *this;
    }
    if (this->sign == SIGN_NEGATIVE)
        return --(*this);
    node *p = this->first;
    bool carry;
    do {
        (p->data)++;
        carry = p->data >= node::NODE_SIZE;
        if (carry)
            p->data -= node::NODE_SIZE;
        p = p->next;
    } while (p && carry);
    if (!p && carry)
        push_back(1);
    return *this;
}

// parse digit according to NODE_SIZE
// private method
// @arg     now     reference to current digit value
// @arg     carry   reference to carry bit
inline void bigInt::parse_carry(int &now, int &carry) {
    if (now < 0) {
        carry = (-now - 1) / node::NODE_SIZE + 1;
        now += carry * node::NODE_SIZE;
        carry *= -1;
    } else if (now >= node::NODE_SIZE) {
        carry = now / node::NODE_SIZE;
        now %= node::NODE_SIZE;
    } else
        carry = 0;
}

// add two non-negative bigInt values
// private method
// @arg     p1      LSB of the first bigInt
// @arg     p2      LSB of the second bigInt
// @arg     prt     LSB of the return value
// @arg     op_sign integer value, 1 for addition, -1 for subtraction
// @return  MSB of the return value
// Value stored in prt will not be reserved.
bigInt::node *bigInt::do_add(const node *p1, const node *p2, node *prt, int op_sign) {
    int now, carry = 0;
    while (p1 || p2) {
        if (p2)
            now = p1->data + op_sign * (p2->data) + carry;
        else
            now = p1->data + carry;
        parse_carry(now, carry);
        prt->data = now;
        p1 = p1->next;
        if (p2)
            p2 = p2->next;
        if (!(prt->next))
            prt = new node(0, prt);
        else
            prt = prt->next;
    }
    if (carry)
        prt->data = carry;
    return prt;
}

// addition
bigInt bigInt::operator+(const bigInt &rhs) const {
    bigInt rt(0);
    int op_sign = this->sign * rhs.sign; // operation sign
    node *p1, *p2;
    // convert into a non-negative operation
    if (abs_cmp(this->first, rhs.first)) {
        rt.last = do_add(this->first, rhs.first, rt.first, op_sign);
        rt.sign = this->sign;
    } else {
        rt.last = do_add(rhs.first, this->first, rt.first, op_sign);
        rt.sign = rhs.sign;
    }

    rt.clear_redundant_zero();
    return move(rt);
}

// subtraction
bigInt bigInt::operator-(const bigInt &rhs) const {
    bigInt tmp, rt;
    // borrow rhs's data for a second
    tmp.first = rhs.first;
    tmp.last = rhs.last;
    tmp.sign = -rhs.sign;
    rt = *this + tmp;   // move construction
    tmp.first = tmp.last = NULL;
    return move(rt);
}

// multiplication
bigInt bigInt::operator*(const bigInt &rhs) const {
    bigInt rt;
    node *pb = rhs.first, *pr = rt.first;
    int carry;
    if (!(*this) || !rhs) {
        rt.push_back(0);
        return move(rt);
    }
    while (pb) {
        carry = 0;
        if (!pr)
            pr = rt.push_back(0);
        node *pa = this->first, *pc = pr;
        while (pa) {
            pc->data += pa->data * pb->data + carry;
            parse_carry(pc->data, carry);
            pa = pa->next;
            if (pa || carry)
                if (pc->next)
                    pc = pc->next;
                else
                    pc = rt.push_back(0);
        }
        if (carry)
            pc->data += carry;
        pb = pb->next;
        pr = pr->next;
    }
    rt.sign = this->sign * rhs.sign;
    rt.clear_redundant_zero();
    return move(rt);
}

// division
bigInt bigInt::operator/(const bigInt &rhs) const {
    bigInt rt(0);
    if(!rhs){ // divider = 0
        cout << "Error: Division by zero." << endl;
        return move(rt);
    }
    if (!abs_cmp(this->first, rhs.first))
        return move(rt);

    bigInt dividend = *this;
    int digit_cnt = 0;
    node *p = rhs.last ? rhs.last : rhs.first;
    do
        ++digit_cnt;
    while (p = p->prev);

    node *pa;
    while (1) {
        pa = dividend.last ? dividend.last : dividend.first;
        // make sure there're sufficient digits to divide
        int d = 0;
        while (pa && ++d < digit_cnt)
            pa = pa->prev;

        int lbound = 1, ubound = 1;
        if (!abs_cmp(pa, rhs.first)) {
            // dividend is still smaller than divider
            if (pa && pa->prev) {
                // supply one more digit if there is one
                pa = pa->prev;
                lbound = ubound = node::NODE_SIZE;
            } else
                // exit otherwise
                break;
        }
        // when dividend is one digit longer than divider
        // A*10000/(X+1) <= quotient <= (A+1)*10000/X
        // when they're of equal length
        // A/(X+1) <= quotient <= (A+1)/X
        // where A = MSB of dividend & X = MSB of divider
        lbound *= dividend.last ? dividend.last->data : dividend.first->data;
        lbound /= 1 + (rhs.last ? rhs.last->data : rhs.first->data);
        ubound *=
            1 + (dividend.last ? dividend.last->data : dividend.first->data);
        ubound /= rhs.last ? rhs.last->data : rhs.first->data;
        ubound += 1;

        // binary search the quotient
        bigInt tmp;
        do {
            int m = (ubound + lbound) / 2;
            tmp = rhs * bigInt(m);
            if (abs_cmp(pa, tmp.first)) // m*divider <= dividend
                lbound = m;
            else
                ubound = m;
        } while (lbound + 1 < ubound);
        // ans = lbound = m
        rt.push_reverse(lbound);
        tmp = rhs * bigInt(lbound);
        dividend.last = do_add(pa, tmp.first, pa, -1);
        dividend.clear_redundant_zero();
    }
    rt.clear_redundant_zero();
    return move(rt);
}

// small index power
// returns a^b where b is any non-negative integer value less than 2^63
// takes advantage of the exponentiating by squaring method
bigInt bigInt::power(long long int rhs) const {
    bigInt tmp = *this, rt(1);
    while (rhs) {
        if (rhs & 1)
            rt = rt * tmp;
        rhs >>= 1;
        if (rhs)
            tmp = tmp * tmp;
    }
    return move(rt);
}

// large index power
// returns a^b where b is any non-negative bigInt
bigInt bigInt::power(const bigInt &rhs) const {
    bigInt tmp = *this, rt(1);
    node *p = rhs.first;
    while (p) {
        rt = rt * tmp.power(p->data);
        p = p->next;
        if (p)
            tmp = tmp.power(10000);
    }
    return move(rt);
}

// factorial
bigInt bigInt::factorial(void) {
    if(this->sign == SIGN_NEGATIVE){
        cout << "Error: negative number factorial." << endl;
        return 0;
    }
    if(!(*this)) return 1;  // 0! = 1
    bigInt rt = *this, tmp = *this;
    while (--tmp)
        rt = rt * tmp;
    return move(rt);
}