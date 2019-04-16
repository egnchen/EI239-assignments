#include "polynome.h"

// clear dangling zero nodes
void polynome::autoclean(void) {
    node *p = first, *prev = NULL;
    while (p) {
        if (p->ratio == 0) {
            if (prev)
                prev->next = p->next;
            else
                first = p->next;
            p->next = NULL;
            delete p;
        }
        prev = p;
        p = p->next;
    }
}

// copy constructor
polynome::polynome(const polynome &rhs) {
    if (rhs.first) {
        first = new node(rhs.first->ratio, rhs.first->exp);
        node *p1 = first, *p2 = rhs.first->next;
        while (p2) {
            p1->next = new node(p2->ratio, p2->exp);
            p1 = p1->next;
            p2 = p2->next;
        }
    } else
        first = NULL;
}

// input
istream &operator>>(istream &in, polynome &poly) {
    delete poly.first;
    poly.first = NULL;
    int n, c, e;
    in >> n;
    while (n--) {
        in >> c >> e;
        if (!c)
            continue;
        polynome::node *p = poly.first, *prev = NULL;
        while (p && p->exp >= e) {
            prev = p;
            p = p->next;
        }
        if (prev && prev->exp == e)
            prev->ratio += c;
        else {
            p = new polynome::node(c, e);
            if (prev) {
                p->next = prev->next;
                prev->next = p;
            } else {
                // first should be after the node to be inserted
                // or first node doesn't exist
                p->next = poly.first;
                poly.first = p;
            }
        }
    }
    poly.autoclean();
    return in;
}

// output
ostream &operator<<(ostream &out, const polynome &poly) {
    polynome::node *p = poly.first;
    if (!p)
        return out << 0;
    bool not_first = false;
    while (p) {
        if (p->ratio) {
            if (not_first && p->ratio > 0)
                out << '+';
            if (p->ratio != 1)
                out << (p->ratio);
            if (p->exp) {
                out << 'x';
                if (p->exp != 1){
                    out << '^';
                    if(p->exp > 0) out << (p->exp);
                    else out << '(' << (p->exp) << ')';
                }
            }
            not_first = true;
        }
        p = p->next;
    }
    return out;
}

// addition
polynome polynome::operator+(const polynome &rhs) const {
    polynome poly = *this;
    node *p1 = poly.first, *p2 = rhs.first;
    node *prev = NULL;
    while (p2) {
        while (p1 && p1->exp >= p2->exp) {
            prev = p1;
            p1 = p1->next;
        }
        if (prev && prev->exp == p2->exp)
            prev->ratio += p2->ratio;
        else {
            p1 = new node(p2->ratio, p2->exp);
            if (prev) {
                p1->next = prev->next;
                prev->next = p1;
            } else {
                p1->next = poly.first;
                poly.first = p1;
            }
        }
        p2 = p2->next;
    }
    poly.autoclean();
    return move(poly);
}

// multiplication
polynome polynome::operator*(const polynome &rhs) const {
    node *p1, *p2 = rhs.first;
    polynome rt;
    while (p2) {
        polynome tmp = *this;
        p1 = tmp.first;
        while (p1) {
            p1->ratio *= p2->ratio;
            p1->exp += p2->exp;
            p1 = p1->next;
        }
        rt = rt + tmp;
        p2 = p2->next;
    }
    rt.autoclean();
    return move(rt);
}

// derivation
polynome polynome::differential(void) const {
    polynome rt = *this;
    node *p = rt.first;
    while (p) {
        if (p->exp) {
            p->ratio *= p->exp;
            p->exp--;
        } else
            p->ratio = 0;
        p = p->next;
    }
    rt.autoclean();
    return move(rt);
}