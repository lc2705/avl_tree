#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdint.h>
#include <vector>
#include "../bloomfilter/bloomfilter.h"
#include "../hashtable/hashtable.h"
using namespace std;

class AVLNode
{
public:
    uint32_t prefix_length;
    class BloomFilter * bloomfilter;
    class HashTable * hashtable;
    
    class BloomFilter * rep_bloomfilter;
    AVLNode * lchild;
    AVLNode * rchild;
public:
    AVLNode(uint32_t length);
    ~AVLNode();
    static int node_cmp(const void *a, const void *b);
};


class AVLTree
{
private:
    AVLNode * root;
    int num;

    AVLNode ** node_array;

    void build_tree(AVLNode ** root, const uint32_t *lens, int num);
    void delete_tree(AVLNode ** root);
    int get_block_num(const char * prefix);
    AVLNode * search_branch(AVLNode * root, const char * prefix);
    void prefix_cut(int block_num, const char * src, char * dst);
public:
    AVLTree(uint32_t lens[], int lens_num);
    ~AVLTree();
    void Insert(const char *prefix);
    void Delete(const char *prefix);
    bool Search(const char *prefix);
};

#endif
