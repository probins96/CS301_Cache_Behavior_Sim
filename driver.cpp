// Name: Palmer Robins

#include "Cache.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

/* This method is passed the name of a configuration file containing
 * the capacity (in bytes), block size (in bytes), and set associativity.
 * It is also passed a pointer to the cache to configure.  It configures
 * the specified cache using the configuration specified in the input file.
 */
void configCache(char* config, Cache* c) {
    
    int capacity, blocksize, associativity;

    ifstream in;

    // Open the file for reading
    in.open(config);
    if (in.is_open() == false) {
        cerr << "Unable to open configuration file " << config << endl;
        exit(1);
    }

    // Read values from the file
    in >> capacity;
    in >> blocksize;
    in >> associativity;

    // Determine if there were any errors while reading configuration file
    if (in.fail()) {
        cerr << "Error while reading configuration file " << endl;
        exit(1);
    }
    in.close();

    // Initialize the cache
    c->initialize(capacity, blocksize, associativity);

}

/* This method takes a configured cache and the name of an input file
 * containing byte memory addresses.  It adds the memory addresses from 
 * the file to the specified cache.
 */
void processInputs(Cache* c, char* input) {

    ifstream in;

    // Open the file for reading
    in.open(input);
    if (in.is_open() == false) {
        cerr << "Unable to open input file " << input << endl;
        exit(1);
    }

    // while there are more things in the file, read entries and add to cache
    while (in.good()) {
        long long address;

        in >> dec >> address;

        if (in.fail()) // last read from file was unsuccessful
            break;

        cout << "ADD: \t" << dec << address << "\t 0x" << hex << address << endl;
        c->addAccess(address);

        // Print contents of cache
        c->printContents();
    }
    in.close();

}

/* 
 * This program creates an instance of a memory cache and simulates the
 * execution of the cache given a series of memory accesses.  It 
 * prints out the contents of the cache after each memory access.  It
 * prints statistics about the cache accesses after the entire memory
 * access file has been processed.
 * 
 * The program expects two command line arguments in the following order:  
 *      1.) name of cache configuration file
 *      2.) name of file containing memory accesses, one per line.
 * 
 * The configuration file should contain the capacity (in bytes), 
 *     block size (in bytes), and set-associativity, in that order.
 * 
 * The memory access file should specify byte addresses.
 */
int main(int argc, char* argv[]) {

    if (argc < 3) {
        cerr << "Need to enter: config input" << endl;
        exit(1);
    }

    // Create and configure cache
    Cache* myCache = new Cache();
    configCache(argv[1], myCache);

    // Process memory accesses
    processInputs(myCache, argv[2]);

    // Print final cache statistics
    myCache->printStatistics();

    delete myCache;

}
