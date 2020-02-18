// Name: Palmer Robins

#include "Cache.h"

#include <iostream>

/* 
* This method just creates an instance of the class.  However, there is
* no default configuration of the class, so the initialize() method must
* be called to actually create a cache that is properly configured.
*/
Cache::Cache() {
    
    myMisses = 0;
    myHits = 0;
    myAccesses = 0;

}

/* 
* Destructor for the class releases all dynamically allocated memory.
*/
Cache::~Cache() {}

/* 
* This method configures the cache.  The capacity and block size need
* to be specified in bytes.  The set associativity is by default set
* to 1 (direct mapped) if no argument is specified.  All of the three
* arguments must be powers of two.  Additionally, the capacity divided
* by the (block_size * associativity) must not have a fractional 
* component.  Finally, the method prints out information about the
* configuration of the cache (see example output files).
*/
void Cache::initialize(int capacity, int block_size, int associativity) {

    // Set up basic cache configuration
    myCapacity = capacity;
    myBlockSize = block_size;
    myAssociativity = associativity;
    myNumBlocks = myCapacity / myBlockSize;
    myNumSets = myNumBlocks / myAssociativity;
    int numBlocksInSet = myNumBlocks / myNumSets;

    // Check to ensure the information configures a valid cache
    bool success = checkInputValidity();
    if (!success) {
        cerr << "The input values are not valid. Failed to set up cache." << endl;
        exit(1);
    }
    else {

        // Print out the cache configuration
        cout << "Capacity " << myCapacity << endl;
        cout << "Block size " << myBlockSize << endl;
        cout << "Associativity " << myAssociativity << endl;
        cout << "Num Sets " << myNumSets << endl;

        // Create a "starter" block
        BlockEntry block;
        block.index = 0;
        block.lru = -1;
        block.tag = 0;
        block.valid = 0;
        block.values = BlockValues(myBlockSize, -1);

        // Create sets to put in the cache, and insert the basic 
        // starter block into each set
        for (int i = 0; i < myNumSets; i++) {
            Set set;
            for (int i = 0; i < numBlocksInSet; i++)
                set.push_back(block);
            myCache.push_back(set);
        }
    }
}

/* 
* Adds the address specified as an access to the current cache contents, 
* potentially evicting an existing entry in the cache.  The address
* is specified in bytes.
*/
void Cache::addAccess(long long addr) {

    myAccesses++;
    int num = addr;
    int blockAddr = addr / myBlockSize;
    int index = blockAddr % myNumSets;
    Set* set = &myCache.at(index);

    // Stores the location of the addr
    int hitIndex = -1;

    // Check for a hit
    bool isHit = hitChecker(set, hitIndex, num);

    // Check whether the set is already full
    bool blockHasSpace = checkSetContents(set);

    // Update block lru's within the set
    updateLRUs(set, hitIndex, blockHasSpace);

    // If the hit checker returned true, increment our counter and return
    if (isHit) {
        myHits++;
        return;
    }
    // We didn't score a hit, so we'll need to figure out where to place a new block
    else {

        // Find the block in the specified set due to be exiled
        BlockEntry maxLRU = set->front();
        maxLRU.index = 0;
        for (unsigned int i = 0; i < set->size(); i++)
            if (maxLRU.lru < set->at(i).lru)
                maxLRU = set->at(i);

        // Set the set of words to be put into the block
        BlockValues values;
        for (int i = 0; i < myBlockSize; i++) {
            values.push_back(num);
            num++;
        }

        BlockEntry* block;
        int blockLocation;  // Where the new entry will be placed

        // For each block in the set
        for (unsigned int i = 0; i < set->size(); i++) {
            // If an empty block is found, thats the location the new block is stored
            if (set->at(i).lru == -1) {
                blockLocation = i;
                break;
            }
            // If it isn't empty, the block will be placed where the maxLRU is located
            else if (i == set->size() - 1)
                blockLocation = maxLRU.index;
        }
        block = &set->at(blockLocation);
        block->setBlock(values, 1, (blockAddr / myNumSets), 0, index); // Set the new block information

        myMisses++; // Count the update as a miss
    }
}

/* 
* Prints the current contents of the cache.  The output is organized
* by cache sets and displays the index, tag, valid bit, and lru state.
*/
void Cache::printContents() {

    BlockEntry currBlock;

    // For each set in the cache, print out the data in its block(s)
    for (int i = 0; i < myNumSets; i++) {
        Set set = myCache.at(i);
        cout << "****** " << "SET " << to_string(i) << "******" << endl;
        for (unsigned int j = 0; j < set.size(); j++) {
            currBlock = set.at(j);
            cout << "Index " << to_string(j) << ":";
            cout << " tag " << hex << currBlock.tag;
            cout << " valid " << to_string(currBlock.valid);
            cout << " lru " << to_string(currBlock.lru) << endl;
        }
        cout << "*****************" << endl;
    }
}

/* 
* Prints the statistics about cache accesses including the number of
* accesses, hits, misses, and hit rate.
*/
void Cache::printStatistics() {

    // Print the counters keeping track of hits, misses, etc. 
    cout << "ACCESSES " << to_string(myAccesses) << endl;
    cout << "HITS " << to_string(myHits) << endl;
    cout << "MISSES " << to_string(myMisses) << endl;

    float myHitRate = (double)myHits / myAccesses;

    string outputHitRate = to_string(myHitRate);
    outputHitRate.erase(outputHitRate.find_last_not_of('0') + 1, string::npos);
    if (outputHitRate.back() == '.')
        outputHitRate.push_back('0');

    cout << "HIT RATE " << outputHitRate << endl;
}

/*
* This function is used in the case of an associative cache.
* If it finds a hit in the cache, it sets that block's lru to 0
* and saves the index of the block where it occurred. 
* Parameters: The specified set, ref to a variable to store the index of a hit, and the addr
* Returns true if a hit occurred
*/
bool Cache::hitChecker(Set* set, int& hitIndex, int num) {

    BlockValues oldEntry; // We'll use this to see if the addr is in the block

    // Check to see if the addr is present in the cache
    for (unsigned int i = 0; i < set->size(); i++) {
        oldEntry = set->at(i).values;
        for (int j = 0; j < myBlockSize; j++)
            // If the addr is found, count it as a hit,
            // set the hit index, and set the lru
            if (oldEntry[j] == num) {
                hitIndex = i;
                set->at(i).lru = 0;
                return true;
            }
    }
    return false;
}

/*
* This function checks whether the set, inputed as a param,
* has an empty blocks left inside it. A boolean is returned indicating
* whether or not it had an empty block
*/
bool Cache::checkSetContents(Set* set) {

    for (unsigned int i = 0; i < set->size(); i++) {
        // If a valid bit is 0, nothing is in that block
        if (set->at(i).valid == 0)
            return true;
    }
    return false;

}

/*
* This function updates block lru's within a set
* The blockHasSpace parameter allows us to distiguish between
* two scenarios: a full set and a set with open block(s).
* If we didn't record a hit, we'll reset the new block's lru to 0
* Other Params: hitIndex stores the index of a hit location (-1 if N/A), and set is the set we're updating
*/
void Cache::updateLRUs(Set* set, int& hitIndex, bool blockHasSpace) {
    // If there was space in the cache when we got a hit
    if (blockHasSpace) {
        for (unsigned int i = 0; i < set->size(); i++)
            // Increment lru only if block was not just reset to 0,
            // and isn't empty
            if (i != (unsigned)hitIndex && set->at(i).lru != -1)
                set->at(i).lru++;
    }
    // If the cache was full when we got a hit
    else
        for (unsigned int i = 0; i < set->size(); i++)
            // Increment lru online if the block was not just reset to 0,
            // and but had already equalled 0
            if (set->at(i).lru == 0 && i != (unsigned)hitIndex)
                set->at(i).lru++;
}

/*
* This function checks that the configuration submitted by the user 
* is valid for our cache simulation program. Returns a boolean of validity.
*/
bool Cache::checkInputValidity() {

    // Ensure the input configuration is valid
    if (myCapacity % 2 != 0) 
        return false;
    else if (myBlockSize % 2 != 0)
        return false;
    else if (myCapacity % (myBlockSize * myAssociativity) != 0)
        return false;

    return true;

}

/*
* This function takes a new set of values, valid bit, tag, lru, and index and updates
* the block's variables to their new values.
*/
void BlockEntry::setBlock(BlockValues newValues, int newValid, int newTag, int newLRU, int newIndex) {
    values = newValues;
    valid = newValid;
    tag = newTag;
    lru = newLRU;
    index = newIndex;
}