#ifndef _MYINT
#define _MYINT
#include <cassert>
#include <cstring>
#include <iostream>

struct mInt {
    int data;
    
    static int cmp_cnt, move_cnt;
    static void start_stat() { cmp_cnt = move_cnt = 0; }

    mInt(){}
    mInt(int data) : data(data) {}
    mInt(const mInt &rhs) : data(rhs.data) { move_cnt++; }
    mInt &operator=(int rhs) {
        move_cnt++;
        data = rhs;
        return *this;
    }
    mInt &operator=(const mInt &rhs) {
        move_cnt++;
        data = rhs.data;
        return *this;
    }
    bool operator<(const mInt &rhs) const {
        cmp_cnt++;
        return data < rhs.data;
    }
    bool operator>(const mInt &rhs) const {
        cmp_cnt++;
        return data > rhs.data;
    }
    bool operator==(const mInt &rhs) const {
        cmp_cnt++;
        return data == rhs.data;
    }
    bool operator>=(const mInt &rhs) const { return !((*this) < rhs); }
    bool operator<=(const mInt &rhs) const { return !((*this) > rhs); }
    bool operator!=(const mInt &rhs) const { return !((*this) == rhs); }
};

mInt *int_array_to_mint_array(int data[], int size) {
    // conversion is based on this assertion:
    assert(sizeof(mInt) == sizeof(int));
    // otherwise the behaviour of this method is undefined.
    int *p = new int[size];
    memcpy(p, data, size * sizeof(int));
    return (mInt *)p;
}

int mInt::cmp_cnt = 0, mInt::move_cnt = 0;

inline std::ostream &operator<<(std::ostream &out, const mInt &rhs) {
    out << rhs.data;
    return out;
}

#endif