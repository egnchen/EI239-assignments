#ifndef _HUFFMAN
#define _HUFFMAN

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class huffmanTree {
  public:
    struct huffman_treenode {
        unsigned char data;
        float weight;
        huffman_treenode *left, *right;
        huffman_treenode(char data = 0, float weight = 0)
            : data(data), weight(weight), right(nullptr), left(nullptr) {}
        ~huffman_treenode() {}
    };

  private:
    vector<huffman_treenode> tree;
    void adjust_down(vector<huffman_treenode> &, int, int);
    void walk_tree(huffman_treenode *root = nullptr, bool output = false,
                   ostream & = std::cout, int depth = 0);

  public:
    struct single_freq_data {
        char data;
        float freq;
    };
    struct single_encode_data {
        char data;
        vector<uint8_t> enc;
        bool operator<(const single_encode_data &rhs) const {
            return data < rhs.data;
        }
    };
    vector<single_encode_data> mapper;

    // pseudo constructor
    huffmanTree(void) {
        // tree.clear();mapper.clear();
    }

    // make frequency data list from string
    vector<single_freq_data> make_list(istream &);
    // build tree from frequency data list
    void build_tree_from_list(const vector<single_freq_data> &);

    // encoder & decoder
    ostream &encode(ostream &, istream &);
    ostream &decode(ostream &, istream &);

    // syntactic sugar
    void build_tree(istream &data) { build_tree_from_list(make_list(data)); }
    void print_tree(ostream &out = std::cout) {
        if (!tree.empty())
            walk_tree(nullptr, true, out);
    }
    bool empty(void) { return tree.empty(); }
    void clear(void) {
        tree.clear();
        mapper.clear();
    }
    const vector<huffman_treenode> &get_tree() { return tree; }
};

#endif