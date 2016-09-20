#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <fstream>
#include <math.h>
#include <bitset>

#include "hashtable.h"

using namespace std;

uint32_t RAND_SEED = 1;

/**
 *
 * This is the implementation of class Bucket.
 *
 **/
Bucket::Bucket()
{
    num_slots = 0;
    slot_array = NULL;
}

Bucket::~Bucket()
{
    while(NULL != slot_array)
    {
        struct Slot * cur_slot = slot_array;
        slot_array = slot_array->next_ptr;
        delete cur_slot;
        num_slots--;
    }
    num_slots = 0;
}

// insert an item in the bucket.
bool Bucket::InsertItem(const char * item)
{
    int item_len = strlen(item);
    struct Slot * new_slot = new struct Slot;
    new_slot->item = new char[item_len + 1];
    strcpy(new_slot->item, item);
    new_slot->next_ptr = NULL;

    new_slot->next_ptr = slot_array;
    slot_array = new_slot;
    num_slots++;

    return true;
}

// search whether an item is in the bucket and return the value of the matching item.
bool Bucket::SearchItem(const char * item)
{
    bool search_result = false;

    struct Slot * cur_slot = slot_array;
    while(NULL != cur_slot)
    {
        if(!strcmp(item, cur_slot->item))
        {
            search_result = true;
            break;
        }
        cur_slot = cur_slot->next_ptr;
    }
    return search_result;
}

// delete an item from the bucket and return the value of the item.
bool Bucket::DeleteItem(const char * item)
{
    bool delete_result = false;

    struct Slot * prev_slot = NULL;
    struct Slot * cur_slot = slot_array;
    while(NULL != cur_slot)
    {
        if(!strcmp(item, cur_slot->item))
        {
            delete_result = true;

            if(NULL == prev_slot)
            {
                slot_array = cur_slot->next_ptr;
                delete cur_slot;
                num_slots--;
            }
            else
            {
                prev_slot->next_ptr = cur_slot->next_ptr;
                delete cur_slot;
                num_slots--;
            }

            break;
        }
        else
        {
            prev_slot = cur_slot;
            cur_slot = cur_slot->next_ptr;
        }
    }
    return delete_result;
}

/**
 *
 * This is the implementation of class HashTable.
 *
 **/
HashTable::HashTable(uint32_t max_items)
{
    if(max_items == 0)
    {
        cout << "Error: Zero item in the set." << endl;
        exit(1);
    }

    num_buckets = (uint32_t) ceil(1.0 * max_items / BUCKET_AVG_ITEMS);
    bucket_array = new class Bucket [num_buckets];

    num_items = 0;
    hash_seed = 0;
    GenerateHashSeed();
}

HashTable::~HashTable()
{
    delete [] bucket_array;
}

// generate a random hash seed for the hash table.
void HashTable::GenerateHashSeed()
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

    // randomly select an item hash seed for each item.
    srand(RAND_SEED);
    uint32_t index = rand() % 128;
    hash_seed = salt_array[index];
}

// compute a hash bucket index of an item.
uint32_t HashTable::ComputeHashBucketIndex(const char * item)
{
    return (MurmurHash3(item, strlen(item), hash_seed) % num_buckets);
}

// insert an item into the hash table.
bool HashTable::Insert(const char * item)
{
    // compute a hash bucket index of the item.
    uint32_t bucket_index = ComputeHashBucketIndex(item);

    // insert the item into the bucket.
    if(true == bucket_array[bucket_index].InsertItem(item))
    {
        num_items++;
        return true;
    }
    else
    {
        return false;
    }
}

// lookup whether an item is in the hash table.
bool HashTable::Lookup(const char * item)
{
    // compute a hash bucket index of the item.
    uint32_t bucket_index = ComputeHashBucketIndex(item);

    // search whether the item is in the bucket.
    return (bucket_array[bucket_index].SearchItem(item));
}

// delete an item from the hash table.
bool HashTable::Delete(const char * item)
{
    // compute a hash bucket index of the item.
    uint32_t bucket_index = ComputeHashBucketIndex(item);

    // delete the item from the bucket.
    if(true == bucket_array[bucket_index].DeleteItem(item))
    {
        num_items--;
        return true;
    }
    else
    {
        return false;
    }
}

// calculate the memory usage in bytes of the hash table.
uint32_t HashTable::CalculateMemoryUsage()
{
    uint32_t hash_table_size = sizeof(class HashTable);

    for(size_t i = 0; i < num_buckets; i++)
    {
        uint32_t bucket_size = sizeof(class Bucket);
        bucket_size += (sizeof(struct Slot) * bucket_array[i].num_slots);
        hash_table_size += bucket_size;
    }

    return hash_table_size;
}

// write the log of the hash table.
void HashTable::WriteLog()
{
    ofstream hash_result_txt;
    hash_result_txt.open("results/hash_result.txt", ios_base::app);

    uint32_t num_used_buckets = 0;
    uint32_t used_bucket_max_slots = 0;
    uint32_t used_bucket_min_slots = num_items;
    uint32_t used_bucket_avg_slots = 0;

    for(size_t i = 0; i < num_buckets; i++)
    {
        if(bucket_array[i].num_slots > 0)
        {
            num_used_buckets++;

            if(bucket_array[i].num_slots >= used_bucket_max_slots)
            {
                used_bucket_max_slots = bucket_array[i].num_slots;
            }

            if(bucket_array[i].num_slots <= used_bucket_min_slots)
            {
                used_bucket_min_slots = bucket_array[i].num_slots;
            }
        }
    }

    if(num_used_buckets != 0)
    {
        used_bucket_avg_slots = num_items / num_used_buckets;
    }
    else
    {
        used_bucket_min_slots = 0;
    }

    //hash_result_txt << "--------Hash Table Summary--------" << endl;
    hash_result_txt << "num_items: " << num_items
        << ", num_buckets: " << num_buckets
        << ", bucket_avg_items: " << (num_items / num_buckets)
        << ", memory_size (bytes): " << CalculateMemoryUsage()
        << ", num_used_buckets: " << num_used_buckets
        << ", used_bucket_max_items: " << used_bucket_max_slots
        << ", used_bucket_min_items: " << used_bucket_min_slots
        << ", used_bucket_avg_items: " << used_bucket_avg_slots << endl;

    hash_result_txt.close();
}

