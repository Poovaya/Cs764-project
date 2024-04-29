#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "DataRecord.h"
#include "RecordDetails.h"
#include "defs.h"

using namespace std;

class StorageDevice {
   public:
    int last_run;
    long long int ssdSize;
    int pageSize;
    long long int total_reads;
	long long int total_writes;
    map<long long int, long long int> run_offset;
    std::string device_path;
    StorageDevice(string device_path);
    StorageDevice();
    int getTotalRuns();
    void spillRecordsToDisk(bool ifNewFile, vector<DataRecord *> &records, int);
    void spillRecordListToDisk(vector<RecordDetails *> record_lists);
    void commitRun();
    void get_device_access_stats();
    vector<RecordDetails *> getRecordsFromRunsOnDisk(int numRecords);
    StorageDevice(const StorageDevice &other);
    string lastValidString;
};
