#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include "../murmurhash/murmurhash3.h"

extern double   FALSE_POSITIVE_RATE;
extern uint32_t BLOOM_HASH_SEED;

enum Status
{
    OK = 0,
    Failure = 1,
};

class BloomFilter
{
    private:
        uint32_t num_items;
        uint32_t num_bits;
        uint32_t num_chars;
        uint32_t num_hashes;
        uint8_t * bloom_vector;
        uint32_t * item_seed_array;

    private:
        void InitHashParameters();
        uint32_t IndexHash(const char * item, uint32_t item_seed);
        void LogHashIndex(const char * item);
//        void initBloomVector(uint8_t ** bv);
//        void initItemSeedArray(uint32_t ** isa);
    public:
        BloomFilter(uint32_t max_items = 1 << 8);
        ~BloomFilter();
        void Insert(const char * item);
        Status Lookup(const char * item);
        void Delete(const char * item);
        uint32_t SpaceSize();
        uint32_t NumHashes();
        double BitsPerItem();
        void LogBloomFilter();
};


#endif
