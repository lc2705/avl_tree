#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <fstream>
#include <math.h>
#include <bitset>

#include "bloomfilter.h"

using namespace std;

double FALSE_POSITIVE_RATE = 0.1;
uint32_t BLOOM_HASH_SEED = 0;

#define BITS_PER_CHAR       8

static const uint8_t bit_mask[BITS_PER_CHAR] =
{
    0x01,  //00000001
    0x02,  //00000010
    0x04,  //00000100
    0x08,  //00001000
    0x10,  //00010000
    0x20,  //00100000
    0x40,  //01000000
    0x80   //10000000
};

/**
 *
 * This is the implementation of class BloomFilter.
 *
 */
BloomFilter::BloomFilter(uint32_t max_items)
{
    if(0 == max_items)
    {
        cout << "Error: Cannot create a bloom filter." << endl;
        exit(1);
    }

    num_items = 0;
    num_bits = (uint32_t) ceil(max_items * 1.44 * log2(1 / FALSE_POSITIVE_RATE));
    num_chars = (uint32_t) ceil(1.0 * num_bits / BITS_PER_CHAR);
    num_bits = num_chars * BITS_PER_CHAR;
    num_hashes = (uint32_t) ceil(log2(1 / FALSE_POSITIVE_RATE));
    bloom_vector = new uint8_t [num_chars];
    memset(bloom_vector, 0, num_chars);
    item_seed_array = new uint32_t [num_hashes];
    memset(item_seed_array, 0, num_hashes * sizeof(uint32_t));
//    initBloomVector(&bloom_vector);
//    initItemSeedArray(&item_seed_array);
    InitHashParameters();
}

BloomFilter::~BloomFilter()
{
    delete [] item_seed_array;
    delete [] bloom_vector;
}

/*
void BloomFilter::initBloomVector(uint8_t ** bv)
{
    *bv = new uint8_t [num_chars];
    memset(*bv, 0, num_chars);    
}

void BloomFilter::initItemSeedArray(uint32_t ** isa)
{
    *isa = new uint32_t [num_hashes];
    memset(*isa, 0, num_hashes * sizeof(uint32_t));    
}
*/

// initialize the parameters for hash functions.
void BloomFilter::InitHashParameters()
{
    uint32_t salt_array[128] =
    {
        0xAAAAAAAA, 0x55555555, 0x33333333, 0xCCCCCCCC,
        0x66666666, 0x99999999, 0xB5B5B5B5, 0x4B4B4B4B,
        0xAA55AA55, 0x55335533, 0x33CC33CC, 0xCC66CC66,
        0x66996699, 0x99B599B5, 0xB54BB54B, 0x4BAA4BAA,
        0xAA33AA33, 0x55CC55CC, 0x33663366, 0xCC99CC99,
        0x66B566B5, 0x994B994B, 0xB5AAB5AA, 0xAAAAAA33,
        0x555555CC, 0x33333366, 0xCCCCCC99, 0x666666B5,
        0x9999994B, 0xB5B5B5AA, 0xFFFFFFFF, 0xFFFF0000,
        0xB823D5EB, 0xC1191CDF, 0xF623AEB3, 0xDB58499F,
        0xC8D42E70, 0xB173F616, 0xA91A5967, 0xDA427D63,
        0xB1E8A2EA, 0xF6C0D155, 0x4909FEA3, 0xA68CC6A7,
        0xC395E782, 0xA26057EB, 0x0CD5DA28, 0x467C5492,
        0xF15E6982, 0x61C6FAD3, 0x9615E352, 0x6E9E355A,
        0x689B563E, 0x0C9831A8, 0x6753C18B, 0xA622689B,
        0x8CA63C47, 0x42CC2884, 0x8E89919B, 0x6EDBD7D3,
        0x15B6796C, 0x1D6FDFE4, 0x63FF9092, 0xE7401432,
        0xEFFE9412, 0xAEAEDF79, 0x9F245A31, 0x83C136FC,
        0xC3DA4A8C, 0xA5112C8C, 0x5271F491, 0x9A948DAB,
        0xCEE59A8D, 0xB5F525AB, 0x59D13217, 0x24E7C331,
        0x697C2103, 0x84B0A460, 0x86156DA9, 0xAEF2AC68,
        0x23243DA5, 0x3F649643, 0x5FA495A8, 0x67710DF8,
        0x9A6C499E, 0xDCFB0227, 0x46A43433, 0x1832B07A,
        0xC46AFF3C, 0xB9C8FFF0, 0xC9500467, 0x34431BDF,
        0xB652432B, 0xE367F12B, 0x427F4C1B, 0x224C006E,
        0x2E7E5A89, 0x96F99AA5, 0x0BEB452A, 0x2FD87C39,
        0x74B2E1FB, 0x222EFD24, 0xF357F60C, 0x440FCB1E,
        0x8BBE030F, 0x6704DC29, 0x1144D12F, 0x948B1355,
        0x6D8FD7E9, 0x1C11A014, 0xADD1592F, 0xFB3C712E,
        0xFC77642F, 0xF9C4CE8C, 0x31312FB9, 0x08B0DD79,
        0x318FA6E7, 0xC040D23D, 0xC0589AA7, 0x0CA5C075,
        0xF874B172, 0x0CF914D5, 0x784D3280, 0x4E8CFEBC,
        0xC569F575, 0xCDB2A091, 0x2CC016B4, 0x5C5F4421
    };

    // randomly select each item seed for hash functions
    srand(BLOOM_HASH_SEED);
    uint32_t num_selects = 0;
    while(num_selects < num_hashes)
    {
        uint32_t index = rand() % 128;
        uint32_t new_seed = salt_array[index];

        // check whether there is a duplicate seed.
        Status select_result = OK;
        for(size_t i = 0; i < num_selects; i++)
        {
            if(new_seed == item_seed_array[i])
            {
                select_result = Failure;
                break;
            }
        }

        if(OK == select_result)
        {
            item_seed_array[num_selects] = new_seed;
            num_selects++;
        }
    }
}

// compute a hashed bit index of an item.
uint32_t BloomFilter::IndexHash(const char * item, uint32_t item_seed)
{
    int len = strlen(item);
    uint32_t hash_value = MurmurHash3(item, len, item_seed) % num_bits;
    return hash_value;
}

void BloomFilter::Insert(const char * item)
{
    //LogHashIndex(item);

    for(size_t i = 0; i < num_hashes; i++)
    {
        uint32_t hash_value = IndexHash(item, item_seed_array[i]);
        uint32_t char_index = hash_value / BITS_PER_CHAR;
        uint32_t bit_index = hash_value % BITS_PER_CHAR;

        bloom_vector[char_index] |= bit_mask[bit_index];
    }
    num_items++;

    //LogBloomFilter();
}

Status BloomFilter::Lookup(const char *item)
{
    Status lookup_result = Failure;

    for(size_t i = 0; i < num_hashes; i++)
    {
        uint32_t hash_value = IndexHash(item, item_seed_array[i]);
        uint32_t char_index = hash_value / BITS_PER_CHAR;
        uint32_t bit_index = hash_value % BITS_PER_CHAR;

        // check whether the hashed bit is 1.
        if(bit_mask[bit_index] == (bloom_vector[char_index] & bit_mask[bit_index]))
        {
            lookup_result = OK;
        }
        else
        {
            lookup_result = Failure;
            break;
        }
    }

    return lookup_result;
}

// delete an item from the filter.
void BloomFilter::Delete(const char *item)
{
    for(size_t i = 0; i < num_hashes; i++)
    {
        uint32_t hash_value = IndexHash(item, item_seed_array[i]);
        uint32_t char_index = hash_value / BITS_PER_CHAR;
        uint32_t bit_index = hash_value % BITS_PER_CHAR;

        bloom_vector[char_index] &= ~bit_mask[bit_index];
    }
    num_items--;
}

// compute the space size of the filter.
uint32_t BloomFilter::SpaceSize()
{
    return num_chars;
}

// compute the number of hash functions of the filter.
uint32_t BloomFilter::NumHashes()
{
    return num_hashes;
}

// compute the bits per items of the filter.
double BloomFilter::BitsPerItem()
{
    return (1.0 * num_chars * BITS_PER_CHAR / num_items);
}

// log the hashed indexes of each item.
void BloomFilter::LogHashIndex(const char *item)
{
    ofstream bloom_result_txt;
    bloom_result_txt.open("results/bloom_result.txt", ios_base::app);
//    bloom_result_txt << item_key << ": ";

    for(size_t i = 0; i < num_hashes; i++)
    {
        uint32_t hash_value = IndexHash(item, item_seed_array[i]);
        uint32_t char_index = hash_value / BITS_PER_CHAR;
        uint32_t bit_index = hash_value % BITS_PER_CHAR;

        bloom_result_txt << hash_value << "[" << char_index << ", " << bit_index << "] ";
    }
    bloom_result_txt << endl;

    bloom_result_txt.close();
}

// log the bloom filter
void BloomFilter::LogBloomFilter()
{
    ofstream bloom_result_txt;
    bloom_result_txt.open("results/bloom_result.txt", ios_base::app);

    bloom_result_txt << "--------------------------------" << endl;
    bloom_result_txt << "Total items: " << num_items << endl;
    bloom_result_txt << "Total bits: " << num_bits << endl;
    bloom_result_txt << "Total chars: " << num_chars << endl;
    bloom_result_txt << "Total hashes: " << num_hashes << endl;

    for(size_t i = 0; i < num_chars; i++)
    {
        bitset <8> char_bitset(bloom_vector[i]);
        bloom_result_txt << "[" << i << "]: " << char_bitset << "(" << (uint32_t) bloom_vector[i] << ")" << endl;
    }

    bloom_result_txt.close();
}

