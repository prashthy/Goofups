using namespace std;
#include <stdio.h>
#include <string>
#include <vector>
#include <map>

typedef struct output {
    string *key;
    int count;
    string *value_hst; // value with highest sort order
} Output;

typedef struct file {
    char filename[20];
    FILE *fp;
    char line[256];
    bool eof;
} File;

class Aggregator
{
    // Maximum number of records that can be stored in-memory at any time
    int _maxRecords; 
    // Number of records in the current iteration that has been processed
    int _curRecords;
    // Each time we flush to disk is considered an iteration
    int _iterations;
    // Map that maintains the key => output mapping. Using an std::map
    // This isn't O(1) for lookups and inserts, but given std:map is an 
    // RB tree implementation, it is O(log n). Alternatively, a hash map 
    // can give O(1), provided the hash is computed correctly
    std::map<string, Output *> records;
    // Vector of output files. 
    std::vector<File *> op_files;
    public: 
    Aggregator(int max) : 
        _maxRecords(max), 
        _curRecords(0),
        _iterations(0)
    {
    }

    // Invoked once an input record has been processed
    void increment() {
        _curRecords++;
    }
    // Invoked before the start of a new iteration
    void reset() {
        _curRecords = 0;
        _iterations++;
    }
    bool max_read() {
        return _curRecords >= _maxRecords;
    }
    void injest(const char *fname);
    // Generate and prepare the output record from the "key" and "value"
    // read from input file
    Output * init_output(char *key, char *value);
    // Given current value, compare with the new "value" read from input
    // file. If the value read from input file is lexicographically greater 
    // than the current value, update this in our record (value_hst)
    void cmp_update_value(Output *o, string value);
    // Write out in-memory contents to disk in the "output" format
    void flush();
    // XXX
    void merge();
};
