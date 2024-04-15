#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "DataRecord.h"
#include "defs.h"

using namespace std;

class StorageDevice {
   public:
    int last_run;
    long long int ssdSize;
    map<int, int> run_offset;
    std::string device_path;
    StorageDevice(string device_path);
    StorageDevice();
    int getTotalRuns();
    void spillRecordsToDisk(bool ifNewFile, vector<DataRecord *> &records);
    void spillRecordListToDisk(vector<vector<DataRecord *> > record_lists);
    void commitRun();
    vector<vector<DataRecord *> > getRecordsFromRunsOnDisk(int numRecords);
};
