#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stack>
#include "avl_tree.h"
#include "../bloomfilter/bloomfilter.h"
#include "../hashtable/hashtable.h"

using namespace std;


AVLNode::AVLNode(uint32_t length)
{
    prefix_length = length;
    bloomfilter = new class BloomFilter(1 << 2);
    hashtable = new class HashTable(1 << 2);

    rep_bloomfilter = new class BloomFilter(1 << 4);
    lchild  = NULL;
    rchild = NULL;    
}

AVLNode::~AVLNode()
{    
    delete bloomfilter;
    delete hashtable;
    delete rep_bloomfilter;
}

int AVLNode::node_cmp(const void *a, const void *b)
{
    AVLNode *node_a = (AVLNode*)a;
    AVLNode *node_b = (AVLNode*)b;
    return node_a->prefix_length-node_b->prefix_length;    
}

void AVLTree::build_tree(AVLNode ** myroot, const uint32_t *lens, int num)
{
    if(num < 1)
        return;
    
    int mid = num/2;
    *myroot = new class AVLNode(*(lens+mid));
    build_tree(&((*myroot)->lchild), lens, mid);
    build_tree(&((*myroot)->rchild), lens+mid+1, num-mid-1);
}

void AVLTree::delete_tree(AVLNode ** root)
{
    
    if((*root)->lchild != NULL)
        delete_tree(&((*root)->lchild));
    if((*root)->rchild != NULL)
        delete_tree(&((*root)->rchild));

    delete *root;
    *root = NULL;
        
}

int AVLTree::get_block_num(const char * prefix)
{
    int block_num = 0;
    int length = strlen(prefix);
    for(int i = 0; i < length; i++)
    {
        if(prefix[i] == '/')
            block_num++;
    }
    return block_num;   
}


void AVLTree::prefix_cut(int block_num, const char * src, char * dst)
{
    int length = strlen(src);
    int block_cnt = 0;
    int str_cnt = 0;
    for( ; str_cnt < length && block_cnt < block_num; str_cnt++)
    {
        if(src[str_cnt] == '/')
            block_cnt++;
    }

    memcpy(dst, src, str_cnt);
    dst[str_cnt] = '\0';
}

AVLNode * AVLTree::search_branch(AVLNode * root, const char * prefix)
{
    if(!root)
        return root;

    AVLNode * node = NULL;
    char new_prefix[128];
    prefix_cut(root->prefix_length, prefix, new_prefix);
    cout << "node length: " << root->prefix_length << " prefix " << new_prefix << endl;

    if(OK == (root->rep_bloomfilter)->Lookup(new_prefix))
    {
        node = search_branch(root->rchild, prefix);
        if(node != NULL) 
        {
            cout << "node " << root->prefix_length << " prefix found in right branch" << endl;
            return node; 
        }
    }   
    
    if(OK == (root->bloomfilter)->Lookup(new_prefix))
    {
        if(true == (root->hashtable)->Lookup(new_prefix))
        {
            cout << "node " << root->prefix_length << " prefix found in this node" << endl;
            return root;
        }
    }

    cout << "node " << root->prefix_length <<" search the left branch" << endl;
    node = search_branch(root->lchild,prefix);
    return node;
}

AVLTree::AVLTree(uint32_t lens[], int lens_num)
{
    root = NULL;
    num = lens_num;
    qsort(lens, lens_num-1, sizeof(uint32_t),AVLNode::node_cmp);
    build_tree(&root, lens, lens_num);    
}

AVLTree::~AVLTree()
{
    delete_tree(&root);
}

void AVLTree::Insert(const char *prefix)
{
    cout << "Insert " << prefix ;
    int length = get_block_num(prefix);
    cout << " block num " << length << endl;
    AVLNode *s = root;
    while(s)
    {
        if(s->prefix_length == length)
            break;
        else if(s->prefix_length > length)
            s = s->lchild;
        else
        {
            char new_prefix[128];
            prefix_cut(s->prefix_length, prefix, new_prefix);
            (s->rep_bloomfilter)->Insert(new_prefix);
            cout << "prefix_length " << s->prefix_length << " new prefix " << new_prefix << endl;
            s = s->rchild;    
        }
    }
    (s->bloomfilter)->Insert(prefix);
    (s->hashtable)->Insert(prefix);
}

void AVLTree::Delete(const char *prefix)
{
    int length = get_block_num(prefix);
    AVLNode *s = root;
    while(s)
    {
        if(s->prefix_length == length)
            break;
        else if(s->prefix_length > length)
            s = s->lchild;
        else
        {
            char new_prefix[128];
            prefix_cut(s->prefix_length, prefix, new_prefix);
            (s->rep_bloomfilter)->Insert(new_prefix);
            cout << "prefix_length " << s->prefix_length << " new prefix " << new_prefix << endl;
            s = s->rchild;    
        }
    }
    (s->bloomfilter)->Delete(prefix);
    (s->hashtable)->Delete(prefix);
    
}

bool AVLTree::Search(const char *prefix)
{
    if(search_branch(root, prefix))
        return true;
    else
        return false;
}
