#include <iostream>
#include <map>
#include "defs.h"
#include "DataRecord.h"

#include <vector>
#include <fstream>


using namespace std;

class StorageDevice {
    public:
    int last_run;
    long long int ssdSize;
    map<int, int> run_offset;
    std::string device_path;
    StorageDevice(string device_path);
    int getTotalRuns();
    void spillRecordsToDisk(bool ifNewFile, vector<DataRecord *> &records);
    void spillRecordListToDisk(vector<vector<DataRecord*> > record_lists);
    void commitRun();
    vector<vector<DataRecord*> >  getRecordsFromRunsOnDisk(int numRecords);
};

