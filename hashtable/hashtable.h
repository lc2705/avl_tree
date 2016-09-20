#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "../murmurhash/murmurhash3.h"

extern uint32_t RAND_SEED;

#define BUCKET_AVG_ITEMS    16

struct Slot
{
    char * item;
    struct Slot * next_ptr;
};

class Bucket
{
    public:
        uint32_t num_slots;
        struct Slot * slot_array;

    public:
        Bucket();
        ~Bucket();
        bool InsertItem(const char * item);
        bool SearchItem(const char * item);
        bool DeleteItem(const char * item);
};

class HashTable
{
    private:
        uint32_t hash_seed;
        uint32_t num_items;
        uint32_t num_buckets;
        class Bucket * bucket_array;

    private:
        void GenerateHashSeed();
        uint32_t ComputeHashBucketIndex(const char * item);

    public:
        HashTable(uint32_t max_items);
        ~HashTable();
        bool Insert(const char * item);
        bool Lookup(const char * item);
        bool Delete(const char * item);
        uint32_t CalculateMemoryUsage();
        void WriteLog();
};

#endif 
