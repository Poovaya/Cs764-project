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
    map<int, int> run_offset;
    std::string device_path;
    StorageDevice(string device_path);
    void spillRecordsToDisk(bool ifNewFile, int runNumber, vector<DataRecord *> &records);
    void spillRecordListToDisk(vector<vector<DataRecord*> > record_lists);
    vector<vector<DataRecord*> >  getRecordsFromRunsOnDisk(int numRecords);
};

