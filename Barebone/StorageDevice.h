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
    map<int, int> run_offset;
    std::string device_path;
    StorageDevice(string device_path);
    StorageDevice();
    int getTotalRuns();
    void spillRecordsToDisk(bool ifNewFile, vector<DataRecord *> &records, int);
    void spillRecordListToDisk(vector<RecordDetails* > record_lists);
    void commitRun();
    vector<RecordDetails*> getRecordsFromRunsOnDisk(int numRecords);
    StorageDevice(const StorageDevice &other);
};
