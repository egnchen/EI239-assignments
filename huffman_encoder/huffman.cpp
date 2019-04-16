#include "huffman.h"

void huffmanTree::adjust_down(vector<huffman_treenode> &heap, int size,
                              int pos) {
    int m;
    while ((m = pos * 2 + 1) < size) {
        if ((m + 1) < size && heap[m + 1].weight < heap[m].weight)
            m++;
        if (heap[m].weight < heap[pos].weight) {
            swap(heap[m], heap[pos]);
            pos = m;
        } else
            break;
    }
}

vector<huffmanTree::single_freq_data> huffmanTree::make_list(istream &data) {
    static int buffer[128];
    memset(buffer, 0, sizeof(buffer));
    int sz = 0;
    while (1) {
        static char i;
        data.get(i);
        if (data.eof())
            break;
        if (!buffer[i])
            sz++;
        buffer[i]++;
    }
    if (!buffer[0])
        buffer[0] = 1;
    vector<single_freq_data> rt;
    rt.reserve(sz);
    for (int i = 0; i <= 0x7f; i++) {
        if (buffer[i])
            rt.push_back({char(i), float(buffer[i])});
    }
    return move(rt);
}

void huffmanTree::build_tree_from_list(
    const vector<single_freq_data> &freq_list) {
    const int cnt = freq_list.size();

    tree.clear();
    tree.reserve(2 * cnt - 1);

    for (int i = 0; i < cnt; i++)
        tree.push_back(huffman_treenode(freq_list[i].data, freq_list[i].freq));

    // make heap
    for (int i = cnt / 2; i >= 0; i--)
        adjust_down(tree, cnt, i);

    // let's make the tree!
    int end = cnt;
    while (end > 1) {
        swap(tree[0], tree[end - 1]);
        adjust_down(tree, end - 1, 0);
        tree.push_back(tree[0]);
        // now the two are heap[end-1] & heap.back()
        // combine these two and make new node
        tree[0].data = 0x80;
        tree[0].weight = tree[end - 1].weight + tree.back().weight;
        tree[0].left = &tree[end - 1];
        tree[0].right = &tree.back();
        // new heap is heap[0:end-1)
        end--;
        adjust_down(tree, end, 0);
    }

    // clear mapper
    mapper.clear();
}

ostream &huffmanTree::encode(ostream &out, istream &data) {
    if (mapper.empty())
        walk_tree();
    if (mapper[0].data != 0) {
        cout << "Error: no '\\0' found in mapper." << endl;
        return out;
    }

    unsigned char buffer = '\0', mask = 128;
    char i = 1;
    while (i) {
        static int l, u, m;
        data.get(i);
        if (data.eof())
            i = 0;
        l = 0;
        u = mapper.size();
        do {
            m = (l + u) / 2;
            if (mapper[m].data == i) {
                // output
                for (uint8_t v : mapper[m].enc) {
                    if (v)
                        buffer |= mask;
                    mask >>= 1;
                    if (!mask) {
                        out << buffer;
                        buffer = '\0';
                        mask = 128;
                    }
                }
                break;
            } else if (mapper[m].data < i)
                l = m + 1;
            else
                u = m - 1;
        } while (l <= u);
        if (mapper[m].data != i) {
            cout << "Error: cannot found encoding data in mapper for 0x" << hex
                 << int(i) << endl;
            return out;
        }
    }
    if (mask < 128)
        out << buffer;
    return out;
}

ostream &huffmanTree::decode(ostream &out, istream &data) {
    huffman_treenode *cur = &(tree[0]);
    while (1) {
        static char i;
        data.get(i);
        if (data.eof())
            break;
        unsigned char mask = 128;
        while (mask) {
            if (mask & i)
                cur = cur->right;
            else
                cur = cur->left;
            if (!(cur->data))
                break; // hit \0
            if (cur->data < 0x80) {
                out << (cur->data);
                cur = &(tree[0]);
            }
            mask >>= 1;
        }
    }
    return out;
}

void huffmanTree::walk_tree(huffman_treenode *root, bool output, ostream &out,
                            int depth) {
    static vector<uint8_t> path;
    if (!root) {
        root = &(tree[0]);
        path.clear();
        mapper.clear();
    }
    if (output) {
        // output tree
        for (int i = 0; i < depth - 1; i++)
            if (path[i])
                out << "    ";
            else
                out << "|   ";
        if (depth)
            out << "|---";
        switch (root->data) {
        case 0x80:
            out << '_';
            break;
        case '\0':
            out << "(eof)";
            break;
        case ' ':
            out << "(space)";
            break;
        case '\t':
            out << "\\t";
            break;
        case '\n':
            out << "\\n";
            break;
        default:
            out << root->data;
        }
        out << ' ' << (root->weight) << endl;
    }
    if (root->data < 0x80) {
        mapper.push_back({char(root->data), path});
        return;
    }
    if (root->left) {
        path.push_back(0);
        walk_tree(root->left, output, out, depth + 1);
        path.pop_back();
    }
    if (root->right) {
        path.push_back(1);
        walk_tree(root->right, output, out, depth + 1);
        path.pop_back();
    }

    if (root == &(tree[0]))
        sort(mapper.begin(), mapper.end());
}