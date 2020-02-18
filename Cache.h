// Name: Palmer Robins

#ifndef _CACHE_H_
#define _CACHE_H_

#include <vector>

using namespace std;

struct BlockEntry;

typedef vector<int> BlockValues;    // Stores words in a block
typedef vector<BlockEntry> Set;     // Defines a set as a vector of blocks

/*
* This struct represents a block, containing all needed information
* for that block. 
*/
struct BlockEntry {

    BlockValues values;             // Stores the 64 values present in the block
    int valid;                      // Stores valid bit of the block
    int tag;                        // Stores current block tag
    int lru;                        // Stores the blocks recent-ness value
    int index;                      // Stores block index

    /*
    * This function takes a new set of values, valid bit, tag, lru, and index and updates
    * the block's variables to their new values.
    */
    void setBlock(BlockValues newValues, int newValid, int newTag, int newLRU, int newIndex);
    
};

/*
* This class represents a configurable memory cache.  To configure the
* cache, the user must specify the cache capacity in bytes, the block
* size in bytes, and the set-associativity.  The default set-associativity
* is 1, meaning direct-mapped.
*
* The user can add memory accesses to the cache by specifying the byte
* address for the memory location.  Each access is assumed to be 4 
* bytes wide.  
*
* The user has the ability to print out the current contents of the cache
* as well as query the cache about access and hit/miss statistics.
*/
class Cache {

    public:
        /* 
        * This method just creates an instance of the class.  However, there is
        * no default configuration of the class, so the initialize() method must
        * be called to actually create a cache that is properly configured.
        */
        Cache();

        /* 
        * Destructor for the class releases all dynamically allocated memory.
        */
        ~Cache();

        /* 
        * This method configures the cache.  The capacity and block size need
        * to be specified in bytes.  The set associativity is by default set
        * to 1 (direct mapped) if no argument is specified.  All of the three
        * arguments must be powers of two.  Additionally, the capacity divided
        * by the (block_size * associativity) must not have a fractional 
        * component.  Finally, the method prints out information about the
        * configuration of the cache (see example output files).
        */
        void initialize(int capacity, int block_size, int associativity = 1);

        /* 
        * Adds the address specified as an access to the current cache contents, 
        * potentially evicting an existing entry in the cache.  The address
        * is specified in bytes.
        */
        void addAccess(long long addr);

        /* 
        * Prints the current contents of the cache.  The output is organized
        * by cache sets and displays the index, tag, valid bit, and lru state.
        */
        void printContents();

        /* 
        * Prints the statistics about cache accesses including the number of
        * accesses, hits, misses, and hit rate.
        */
        void printStatistics();

        /* 
        * Returns the number of accesses made to the cache. 
        */
        long long getNumberAccesses() { return myAccesses; }

        /* 
        * Returns the number of access that resulted in cache misses.
        */
        long long getNumberMisses() { return myMisses; }

    private:

        /*
        * This function is used in the case of a 4-way associative cache.
        * If it finds a hit in the cache, it sets that block's lru to 0
        * and saves the index of the block where it occurred. 
        * Parameters: The specified set, ref to a variable to store the index of a hit, and the addr
        * Returns true if a hit occurred
        */
        bool hitChecker(Set* set, int &hitIndex, int num);

        /*
        * This function checks whether the set, inputed as a param,
        * has an empty blocks left inside it. A boolean is returned indicating
        * whether or not it had an empty block
        */
        bool checkSetContents(Set* set);

        /*
        * This function updates block lru's within a set in the case of a 4-way associative cache
        * The blockHasSpace parameter allows us to distiguish between
        * two scenarios: a full set and a set with open block(s).
        * If we didn't record a hit, we'll reset the new block's lru to 0
        * Other Params: hitIndex stores the index of a hit location (-1 if N/A), and set is the set we're updating
        */
        void updateLRUs(Set* set, int& hitIndex, bool blockHasSpace);

        /*
        * This function checks that the configuration submitted by the user 
        * is valid for our cache simulation program. Returns a boolean of validity.
        */
        bool checkInputValidity();

        int myCapacity;         // Total cache size
        int myBlockSize;        // Cache block size
        int myAssociativity;    // Type of cache associativity
        int myNumBlocks;        // Total blocks in cache
        int myNumSets;          // Total sets in cache

        // These variables keep track of the statistics.
        // They will be printed to the user.
        long long myAccesses;
        long long myMisses;
        long long myHits;

        // The cache, represented as an array of sets
        vector<Set> myCache;

};

#endif