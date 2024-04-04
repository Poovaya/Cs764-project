#include "StorageDevice.h"

void StorageDevice:: spillRecordsToDisk(bool ifNewFile, int runNumber, vector<DataRecord*> &records) {
    string runPath = this->device_path;
    if (ifNewFile) {
        runPath += "/sorted_run" + to_string(runNumber);
        this->last_run++;
    } else {
        runPath += "/merged_runs";
    }

    fstream runfile;
    string str_records = "";

    runfile.open(runPath, ios_base::app);
    if (!runfile.is_open())
        return;

    for (uint ii = 0 ; ii < records.size() ; ii++) {
        DataRecord *record = records[ii];
        string str_record = record->getRecord();
        str_records += str_record + "|";
    }
    runfile << str_records;

    runfile.close();
    return;
}

vector<vector<DataRecord> >  StorageDevice::  getRecordsFromRunsOnDisk(int numRecords) {
	vector<vector<DataRecord> > recordLists;

	for (uint ii = 1 ; ii <= this->last_run ; ii++) {
		vector<DataRecord> records;
        fstream runfile;	
        string runPath = this->device_path + "/sorted_run" + to_string(ii);
        uint recordSize = 4*4 + 3 + 1;
        char *runs = new char[numRecords * recordSize + 1];

        runfile.open(runPath, ios::in);
        if (!runfile.is_open())
            return recordLists;

        runfile.seekg(this->run_offset[ii], ios::beg);

        runfile.get(runs, numRecords * recordSize + 1);
        runfile.close();
        this->run_offset[ii] += strlen(runs);

        if (strlen(runs) != (numRecords * recordSize)) {
            remove(runPath.c_str());
        }

        string s(runs);

        int start = 0;
        while (start < strlen(runs))
        {
            string record_str;

            record_str = s.substr(start, recordSize);
            string col_value1 = record_str.substr(0, 4);
            string col_value2 = record_str.substr(5, 4);
            string col_value3 = record_str.substr(4 * 2 + 2, 4);
            string col_value4 = record_str.substr(4 * 3 + 3, 4);

            DataRecord record = DataRecord(col_value1, col_value2, col_value3, col_value4);
            records.push_back(record);
            start += recordSize;
        }

        recordLists.push_back(records);
	}

    return recordLists;
} 


