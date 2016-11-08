using namespace std;

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <malloc.h>
#include <string.h>
#include "aggregator.h"

#define LARGEST_HST "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"


void Aggregator::injest(const char *filename)
{
    char key[32];
    char value[100];
    FILE *fp = fopen(filename, "r");
    std::map<string, Output *>::iterator it;
    if (fp == NULL) {
        printf("Unable to open %s\n", filename);
        return;
    }
    while (1)
    {
        fscanf(fp, "%s %s\n", key, value);
        //printf("Key: %s, Value: %s\n", key, value);
        Output *op;
        it = records.find(key);
        if (it != records.end()) {
            //cout << "Found entry: " << (*it).first << endl;
            (*it).second->count++;
            cmp_update_value((*it).second, value);
        } else {
            //printf("Inserting key: %s, value: %s\n", key, value);
            op = init_output(key, value);
            // Insert key/value into map
            std::string str(key);
            records[str] = op;
        }

        increment();
        if (max_read()) {
            flush();
        }
        if (feof(fp)) 
            break;
    }

    // Let's flush the remaining records to the disk
    flush();

    // Post processing: 
    // merge() will take all the sorted output files and merge them into one single
    // output file
    merge();
    return;
}

Output *Aggregator::init_output(char *key, char *value)
{ 
    Output *o = new output(); 
    o->key = new std::string(key);
    o->count = 1;
    o->value_hst = new std::string(value);
    return o;
}

void Aggregator::cmp_update_value(Output *o, string value)
{
    if (o->value_hst->compare(value) < 0) {
        delete o->value_hst;
        o->value_hst = new std::string(value);
    }
}

void Aggregator::flush()
{
    std::map<string, Output *>::iterator it;
    char fname[20];
    int count = 0; 
    sprintf(fname, "file%d.out", _iterations);
    std::ofstream out(fname);
    for(it = records.begin(); it != records.end(); ++it) {
        // Commit output to file
        Output *op = (*it).second;
        out << *(op->key) << " ";
        out << op->count << " ";
        out << *(op->value_hst) << endl;
        count++;

        // Memory clean up: 
        // Delete all heap members and then 
        // delete the key/value pair from the std::map
        delete op->key;
        delete op->value_hst;
        delete op;
    }
    records.clear();
    out.close();
    printf("Iterations(%d): Wrote %d records to file %s\n", 
            _iterations, count, fname);

    File *f = new File();
    strcpy(f->filename, fname);
    f->fp = NULL;
    strcpy(f->line, "");
    f->eof = false;
    op_files.push_back(f);
    reset();
}

void Aggregator::merge()
{
    std::vector<File *>::iterator it;
    it = op_files.begin();
    char old_key[32], old_val[100];
    int old_cnt = 0;
    // Initialize all file handles
    for(it = op_files.begin(); it != op_files.end(); ++it) {
        File *f = *it;
        f->fp = fopen(f->filename, "r");
        if (f->fp == NULL) {
            printf("Error reading %s\n", f->filename);
            return;
        }
        fgets(f->line, 256, f->fp);
        if (feof(f->fp))
            f->eof = true;
    }
    FILE *output = fopen("myoutput.out", "w");
    bool done = false;
    int count = 0;
    while(!done)
    {
        done = true;
        char key[32];
        char val[100];
        File *obj = NULL;
        strcpy(old_key, LARGEST_HST);
        for(it = op_files.begin(); it != op_files.end(); ++it) {
            File *elem = (*it);
            if (elem->eof)
                continue;
            done = false;
            sscanf(elem->line, "%s %d %s\n", key, &count, val);
            int ret = strcmp(old_key, key);
            if (ret > 0) {
                    strcpy(old_key, key);
                    strcpy(old_val, val);
                    obj = *it;
                    old_cnt = count;
            } else if  (ret == 0) {
                old_cnt += count;
                if (strcmp(old_val, val) < 0)
                    strcpy(old_val, val);
                // This "key"has already been accounted for under "old_key"
                // So, skip this row since we need to deduplicate the output file
                fgets(elem->line, 256, elem->fp);
                if (feof(elem->fp))
                    elem->eof = true;
            }

        }
        if (obj != NULL) {
            fprintf(output, "%s %d %s\n", old_key, old_cnt, old_val);
            //printf("Lowest Key: %s\n", old_key);
            fgets(obj->line, 256, obj->fp);
            if (feof(obj->fp))
                obj->eof = true;
        }
    }
    fclose(output);
}

