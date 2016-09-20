#include <iostream>
#include <fstream>
#include <stdint.h>

#include "avl_tree.h"

using namespace std;

const char *delete_string[2] = 
{
    "a/"
};

const char *search_string[2] = 
{
    "a/b/c/d/e/f/",
    "a/b/c/d/f/d/"
};


char buffer[16];

int main()
{
    int lens_num = 7;
    uint32_t lens[7];
    class BloomFilter ** bf_array = new class BloomFilter* [lens_num];
    for(int i = 0; i < lens_num; i++)
    {
        lens[i] = (uint32_t)i+1;
        bf_array[i] = new class BloomFilter(1 << 8);
    }

    class AVLTree * avl_t = new class AVLTree(lens,lens_num);
    ifstream file("string.txt");

    file.getline(buffer,16);
    while(!file.eof())
    {
        cout << buffer << endl;    
        avl_t->Insert(buffer);
        file.getline(buffer,16);
    }

    for(int i = 0; i < 2; i++)
    {
        if(avl_t->Search(search_string[i]) == true)
            cout << search_string[i] << " found" << endl;
        else
            cout << search_string[i] << " unfound" << endl;  
    }
/*
    for(int i = 0; i < 2; i++)
    {
        avl_t->Delete(delete_string[i]);
        cout << delete_string[i] << " deleted" << endl;
    }

    for(int i = 0; i < 12; i++)
    {
        if(avl_t->Search(search_string[i]) == true)
            cout << search_string[i] << " found" << endl;
        else
            cout << search_string[i] << " unfound" << endl;  
    }
*/
    return 0;    
}
