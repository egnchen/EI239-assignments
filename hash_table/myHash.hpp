#ifndef _MYHASH
#define _MYHASH
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
using std::out_of_range;
using std::string;

template <class keyT, class valT> class hashTable {
  public:
    virtual bool insert(const keyT &, const valT &) = 0;
    virtual valT &operator[](const keyT &) = 0;
    virtual ~hashTable(){};
};

template <class keyT> struct hasher {
    virtual int operator()(const keyT &key) const = 0;
};

struct stringSumHasher : public hasher<string> {
    virtual int operator()(const string &key) const {
        int h = 0;
        for (string::const_iterator i = key.begin(); i != key.end(); ++i)
            h += *i;
        return h;
    }
};

struct stringSepSumHasher : public hasher<string> {
    virtual int operator()(const string &key) const {
        int h = 0;
        int t;
        for (string::const_iterator i = key.begin(); i != key.end(); ++i) {
            t += *i;
            t <<= 7;
            if (t & 0xffc000) {
                h += t;
                t = 0;
            }
        }
        return h;
    }
};

struct nameHasher : public hasher<string> {
    virtual int operator()(const string &key) const {
        int h = 0;
        for (string::const_iterator i = key.begin(); i != key.end(); ++i)
            h += *i - 'a';
        return h;
    }
};

struct nameSquareMidHasher : public hasher<string> {
    virtual int operator()(const string &key) const {
        int h = 0;
        for (string::const_iterator i = key.begin(); i != key.end(); ++i) 
            h += (*i - 'a') * (*i - 'a');
        // I love digital fundamentals
        int m = h & (h-1);
        int pos = 0;
        if(m & 0xffff0000) pos += 16;
        if(m & 0xff00ff00) pos += 8;
        if(m & 0xf0f0f0f0) pos += 4;
        if(m & 0xcccccccc) pos += 2;
        if(m & 0x22222222) pos += 1;
        // concatenate the middle half
        return (h & ((2 << (pos * 3 / 4)) - 1)) >> (pos / 4);
    }
};

template <class keyT, class valT>
class linearHashTable : public hashTable<keyT, valT> {
    struct pr {
        keyT *k;
        valT *v;
    };
    int size;
    const hasher<keyT> *hash_gen;
    pr *storage;

  public:
    linearHashTable(int size, const hasher<keyT> &hash) : size(size) {
        hash_gen = &hash;
        storage = new pr[size];
        memset(storage, 0, size * sizeof(pr));
    };

    int collision;
    void start_stat() { collision = 0; }

    virtual valT &operator[](const keyT &key) {
        int pre = (*hash_gen)(key) % size;
        int h = pre;
        do{
            if(!storage[h].k || *storage[h].k == key)
                break;
            if(++h == size)
                h = 0;
            collision++;
        } while (h != pre);
        if (storage[h].k && *storage[h].k == key && storage[h].v)
            return *storage[h].v;
        throw out_of_range("Not inserted.");
    }
    virtual bool insert(const keyT &key, const valT& val) {
        int pre = (*hash_gen)(key) % size;
        int h = pre;
        do{
            if(!storage[h].k || *storage[h].k == key)
                break;
            if(++h == size)
                h = 0;
            collision++;
        } while (h != pre);
        if (h == pre && storage[h].k)
            // 404 not found
            return false;
        if (storage[h].k && *storage[h].k == key)
            // key already exists
            return false;
        storage[h].k = new keyT(key);
        storage[h].v = new valT(val);
        return true;
    }
    virtual ~linearHashTable() {
        for (int i = 0; i < size; i++) {
            delete storage[i].k;
            delete storage[i].v;
        }
        delete[] storage;
    }
};

template <class keyT, class valT>
class openHashTable : public hashTable<keyT, valT> {
    struct node {
        keyT k;
        valT v;
        node *next;
    };
    node **heads;
    int size;
    const hasher<keyT> *hash_gen;

  public:
    openHashTable(int size, const hasher<keyT> &hash) : size(size) {
        hash_gen = &hash;
        heads = new node *[size];
        memset(heads, 0, size * sizeof(node *));
    };

    int collision;
    void start_stat() { collision = 0; }

    virtual valT &operator[](const keyT &key) {
        int h = (*hash_gen)(key) % size;
        node **indirect = &heads[h];
        while (*indirect && (*indirect)->k != key) {
            indirect = &(*indirect)->next;
        }
        if (*indirect)
            return (*indirect)->v;
        throw out_of_range("Not inserted.");
    }

    virtual bool insert(const keyT &key, const valT& val) {
        int h = (*hash_gen)(key) % size;
        node **indirect = &heads[h];
        while (*indirect && (*indirect)->k != key) {
            indirect = &(*indirect)->next;
            collision++;
        }
        if (!(*indirect)) {
            *indirect = new node;
            (*indirect)->k = key;
            (*indirect)->v = val;
            (*indirect)->next = NULL;
            return true;
        }
        return false;
    }
    virtual ~openHashTable() {
        int p = 0;
        for (int i = 0; i < size; i++) {
            node *cur = heads[i];
            node *cur2;
            while (cur) {
                cur2 = cur->next;
                delete cur;
                cur = cur2;
            }
        }
        delete[] heads;
    }
};

#endif