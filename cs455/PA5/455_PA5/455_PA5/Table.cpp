// Name:    Junzhe Liu
// USC NetID:   2270250947
// CSCI 455 PA5
// Fall 2019

// Table.cpp  Table class implementation


#include "Table.h"

#include <iostream>
#include <string>
#include <cassert>

using namespace std;


// listFuncs.h has the definition of Node and its methods.  -- when
// you complete it it will also have the function prototypes for your
// list functions.  With this #include, you can use Node type (and
// Node*, and ListType), and call those list functions from inside
// your Table methods, below.

#include "listFuncs.h"


//*************************************************************************


Table::Table()
{
    hashTable = new ListType[HASH_SIZE]();      // (Underlying hash table is HASH_SIZE.)
    hashSize = HASH_SIZE;
    numEntry = 0;                               // create an empty table, i.e., one where numEntries() is 0
}


Table::Table(unsigned int hSize)
{
    hashTable = new ListType[hSize]();          // the underlying hash table is hSize
    hashSize = hSize;
    numEntry = 0;                               // create an empty table, i.e., one where numEntries() is 0
}


/**
    Table Insert function: insert a key-value pair into the hash table
    @param key the key string
    @param value the corresponding value
    @return false if the key is already present in the hash table, true otherwise.
 */
bool Table::insert(const string &key, int value)
{
    unsigned int code = Table::hashCode(key);               // get the hash code of the key
    bool flag = listInsert(hashTable[code], key, value);    // hashTable[code] returns a link list, insert the pair into the list
    if(flag)
    {
        numEntry++;         // If the Insertion succeeded, the number of entry increased by 1
    }
    return flag;
}


/**
   Table lookup function: look up a given key in the hash table
   @param key the key string
   @return the address of the value that goes with this key, or NULL if key is not present.
*/
int * Table::lookup(const string &key)
{
    unsigned int code = Table::hashCode(key);
    ListType list = listLookUp(hashTable[code], key);       // get the hash code of the key, and search the key in the corresponding list
    if(list == NULL)            // If we do not find the key in the list, return NULL
    {
        return NULL;
    }
    return &(list->value);      // else, get the node's value, and return the address of the value (using &)
}


/**
   Table remove function: remove a given key in the hash table
   @param key the key string
   @return false if the key is not present in the table, true otherwise.
*/
bool Table::remove(const string &key)
{
    unsigned int code = Table::hashCode(key);
    bool flag = listRemove(hashTable[code], key);           // get the hash code, and try to remove the key in the corresponding list
    if(flag)
    {
        numEntry--;             // If the removal succeeded, the number of entry should decrease by 1.
    }
    return flag;
}


/**
    Table printAll function: print out all the key-value pair in the hash table
    The order is based on the hash code of key (increasing order) and the insertion order (First In First Out)
 */
void Table::printAll() const
{
    for(int i=0; i<hashSize; i++)
    {
        listPrint(hashTable[i]);            // Traverse the hash table and print out each link list
    }
}


/**
    Table numEntries function: return the number of key-value pair in the table
    @return the number of entries in the table
 */
int Table::numEntries() const
{
    return numEntry;
}


/**
    Table hashStats function: prints out info to let us know if we're getting a good distribution of values in the hash table.
    @param out ostream (cout / cerr etc.)
 */
void Table::hashStats(ostream &out) const
{
    unsigned int num = 0;
    unsigned int longest = 0;
    for(int i=0; i<hashSize; i++)
    {
        int len = listSize(hashTable[i]);
        if(len != 0)
        {
            num++;                          // The Number of Non-empty Buckets
            if(len > longest)
            {
                longest = len;              // The Length of the Longest Chain
            }
        }
    }
    out << "number of buckets: " << hashSize << endl;
    out << "number of entries: " << numEntry << endl;
    out << "number of non-empty buckets: " << num << endl;
    out << "longest chain: " << longest << endl;
}


// add definitions for your private methods here

