Proposed approach: 
1. Read row by row from the input file and for each row, create an "output"
   data structure that has the following data: 
   - Key
   - Count
   - Value (contains the highest sort order for the corresponding key)

2. Create an inmemory hash table to insert and lookup items efficiently 
   - For a good hash-table implementation, insert and lookup can be O(1). In 
     my implementation, I decided to use the C++ std::map, which implements an
     RB tree, which is O(log n) asymptotic run time
   - The key for the hash table is the "key" from the input file
   - The value in the hash table is the "output" data structure described above

3. Once the in-memory hash table has processed enough records (I am using 
   the # of records as input to indicate the upper limit on memory), the 
   contents of the in-memory hash table is flushed to a new file sorted by the 
   "key". So, the contents in the output file are always in sorted order

4. We clear the contents of the in-memory hash table to start injesting new
   data from the input file. This essentially commences a new "iteration".

5. Go back to step 2 until all the data has been read from the input file

6. Once all the data from the input file has been read, what we have now is a 
   set of output files. The final step is the process of merging all the output
   files into a single file. 

